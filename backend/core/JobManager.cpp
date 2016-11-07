/*
 Copyright (c) 2012 Petr Koupy
 Copyright (c) 2012 Vladimir Fiklik

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cassert>
#include <fstream>

#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "inout.h"
#include "JobManager.h"
#include "../chem/scaffold/Scaffold.hpp"
#include "../chem/scaffold/ScaffoldDatabase.hpp"

JobManager::JobManager(tbb::task_group_context *pathFinderStopper,
        std::string &storagePath, std::string &jobListFile, bool interactive
        ) :
mHalted(false),
mInteractive(interactive),
mPathFinderStopper(pathFinderStopper),
mJobIdCounter(0),
mDeferredFingerprintSelectorIsSet(false),
mDeferredSimCoeffSelectorIsSet(false),
mDeferredDimRedSelectorIsSet(false),
mDeferredChemOperSelectorsIsSet(false),
mDeferredMolpherParamsIsSet(false),
mDeferredDecoysIsSet(false) {
    // Create the storage at the given storagePath. Name of the directory is
    // the timestamp of creation of jobManager
    if (!storagePath.empty()) {
        mStorageDir = storagePath + "/";
    }
    boost::posix_time::ptime timestamp = boost::posix_time::second_clock::local_time();
    mStorageDir += boost::posix_time::to_iso_string(timestamp);
    mBackendId = boost::posix_time::to_iso_string(timestamp);
    try {
        boost::filesystem::create_directories(mStorageDir);
    } catch (boost::filesystem::filesystem_error &exc) {
        SynchCout(exc.what());
    }

    if (!jobListFile.empty()) {
        std::string line;
        std::ifstream inFile(jobListFile.c_str());

        if (!inFile.is_open()) {
            SynchCout(std::string("Cannot open job list file: ").append(jobListFile));
        }

        while (inFile.is_open() && inFile.good()) {
            std::getline(inFile, line);
            IterationSnapshot snp;
            if (ReadSnapshotFromFile(line, snp)) {
                std::string emptyPassword;
                CreateJob(snp, emptyPassword);
            } else {
                SynchCout(std::string("Cannot open job: ").append(line));
            }
        }

        inFile.close();
    }

    SynchCout(std::string("JobManager initialized."));
}

void JobManager::AddJobFromFile(const std::string &jobFile) {
    IterationSnapshot snp;
    if (ReadSnapshotFromFile(jobFile, snp)) {
        std::string emptyPassword;
        CreateJob(snp, emptyPassword);
    } else {
        SynchCout(std::string("Cannot open job: ").append(jobFile));
    }
}

void JobManager::Halt() {
    Lock lock(mJobManagerGuard);
    mHalted = true;

    // Flush all live jobs apart from the running job.
    if (!mJobs.mLiveJobQueue.empty()) {
        JobId runningJob = mJobs.mLiveJobQueue.front();
        mJobs.mLiveJobQueue.clear();
        mJobs.mLiveJobQueue.push_back(runningJob);
    }

    mPathFinderStopper->cancel_group_execution(); // Request sleep of running job.
    // Running job will be put to sleep in CommitIteration.
    mJobReadyCondition.notify_all(); // In case the path finder is sleeping.

    SynchCout(std::string("JobManager halted."));
}

JobManager::~JobManager() {
    // no-op
}

bool JobManager::GetJob(PathFinderContext &ctx) {
    Lock lock(mJobManagerGuard);
    while (mJobs.mLiveJobQueue.empty()) {
        if (mHalted || !mInteractive) {
            return false; // Path finder thread will terminate.
        } else {
            mJobReadyCondition.wait(lock); // Yields lock until signalled.
        }
    }

    IterationSnapshot snp;
    // Fill the snapshot according to queued job
    GetFirstLiveJob(snp);
    PathFinderContext::SnapshotToContext(snp, ctx); // Insert snapshot into path finder.

    try {
        // Directory may already exist if this job already run or was cancelled
        // before finishing even the first iteration.
        boost::filesystem::create_directories(GenerateDirname(mStorageDir, snp.jobId));
    } catch (boost::filesystem::filesystem_error &exc) {
        // no-op (directory already exists)
    }

    // This job might not run before, therefore save the initial snapshot.
    WriteSnapshotToFile(
            GenerateFilename(mStorageDir, snp.jobId, snp.iterIdx), snp);
    PublishIteration(snp);

    return true;
}

std::string JobManager::GetStorageDir() {return mStorageDir;}

bool JobManager::CommitIteration(PathFinderContext &ctx, bool canContinue, bool &pathFound) {
    Lock lock(mJobManagerGuard);

    JobId jobId = ctx.jobId;

    bool flushJob = false;
    if (mPathFinderStopper->is_group_execution_cancelled()) {
        mPathFinderStopper->reset();
        flushJob = true;
    }

    if (!flushJob) {
        IterationSnapshot snp;
        PathFinderContext::ContextToSnapshot(ctx, snp);
        if (pathFound) {
            bool lastScaffPath = (snp.scaffoldSelector == SF_MOST_SPECIFIC);
            if (!ctx.ScaffoldMode() || lastScaffPath) {
                WriteMolpherPath(
                        GenerateFilename(mStorageDir, snp.jobId, "path.txt"),
                        snp.target.smile, snp.candidates, snp.pathMolecules);
                // "final_mols.sdf" has no sense in scaffold hopping calculation mode
                if (!ctx.ScaffoldMode()) {
                    WriteMolphMolsToSDF(
                            GenerateFilename(mStorageDir, snp.jobId, "final_mols.sdf"),
                            snp.candidates);
                }

                std::map<std::string, MolpherMolecule> gathered;
                for (unsigned int i = 1; i <= snp.iterIdx; ++i) {
                    IterationSnapshot historicSnp;
                    bool loaded = ReadSnapshotFromFile(
                            GenerateFilename(mStorageDir, snp.jobId, i), historicSnp);
                    if (loaded) {
                        GatherMolphMols(historicSnp.candidates, gathered);
                    }
                }
                WriteMolphMolsToSDF(
                        GenerateFilename(mStorageDir, snp.jobId, "all_mols.sdf"),
                        gathered);
            } else {
                // there was found (NOT last) path during scaffold hopping.
                // So the scaffold level was not the most specific.

                ctx.morphDerivations.clear();

                std::vector<MolpherMolecule> exploredPathMolecules;
                PathFinderContext::ScaffoldSmileMap::accessor acScaff;
                ctx.candidateScaffoldMolecules.find(acScaff, ctx.target.scaffoldSmile);
                assert(!acScaff.empty());
                PathFinderContext::CandidateMap::accessor acCand;
                ctx.candidates.find(acCand, acScaff->second);
                assert(!acCand.empty());
                acScaff.release();
                std::string targetInCand = acCand->first;
                while (!acCand->second.parentSmile.empty() &&
                        acCand->second.parentSmile != ctx.tempSource.smile) {
                    std::string descendant = acCand->first;
                    ctx.candidates.find(acCand, acCand->second.parentSmile);
                    assert(!acCand.empty());
                    MolpherMolecule parentMol = acCand->second;
                    parentMol.historicDescendants.clear();
                    parentMol.descendants.clear();
                    parentMol.descendants.insert(descendant); // just one descendant on path
                    exploredPathMolecules.push_back(parentMol);
                }

                // all candidates (except explored path molecules) are marked as pruned
                PathFinderContext::CandidateMap::const_iterator itCandidates;
                std::vector<MolpherMolecule>::const_iterator itPath;
                for (itCandidates = ctx.candidates.begin();
                        itCandidates != ctx.candidates.end(); itCandidates++) {
                    bool isInNewPathMols = false;
                    for (itPath = exploredPathMolecules.begin();
                            itPath != exploredPathMolecules.end(); itPath++) {
                        if (itCandidates->first.compare(itPath->smile) == 0) {
                            isInNewPathMols = true;
                        }
                    }
                    if (itCandidates->first.compare(targetInCand) == 0) {
                        snp.target.parentChemOper = itCandidates->second.parentChemOper;
                        if (itCandidates->first.compare(snp.tempSource.smile) != 0) {
                            snp.target.parentSmile = itCandidates->second.parentSmile;
                        } else {
                            // scaffolds of target and temporary source are the same
                            snp.target.parentSmile = itCandidates->first;
                        }
                    }
                    if (!isInNewPathMols) {
                        snp.prunedDuringThisIter.push_back(itCandidates->first);
                    }
                }
                snp.candidates.insert(std::pair<std::string, MolpherMolecule>(
                    snp.target.smile, snp.target));

                if (!exploredPathMolecules.empty()) {
                    std::vector<MolpherMolecule> newPathMolecules;
                    for (int i = 0; i < ctx.pathMolecules.size(); ++i) {
                        newPathMolecules.push_back(ctx.pathMolecules[i]);
                        if (ctx.pathMolecules[i].smile.compare(ctx.tempSource.smile) == 0) {
                            // update descendant of temporary source
                            assert(newPathMolecules.back().descendants.size() == 1);
                            assert(exploredPathMolecules.back().parentSmile.compare(
                                    newPathMolecules.back().smile) == 0);
                            newPathMolecules.back().descendants.clear();
                            newPathMolecules.back().descendants.insert(exploredPathMolecules.back().smile);
                            // add newly explored path molecules
                            std::vector<MolpherMolecule>::const_reverse_iterator rItPath;
                            for (rItPath = exploredPathMolecules.rbegin();
                                    rItPath != exploredPathMolecules.rend(); ++rItPath) {
                                newPathMolecules.push_back(*rItPath);
                            }
                            // update descendant of molecule before target and parent of target
                            assert(ctx.pathMolecules.size() == i + 2);
                            assert(ctx.pathMolecules[i+1].smile.compare(ctx.target.smile) == 0);
                            newPathMolecules.back().descendants.clear();
                            newPathMolecules.back().descendants.insert(ctx.pathMolecules[i+1].smile);
                            ctx.pathMolecules[i+1].parentSmile = newPathMolecules.back().smile;
                        }
                    }
                    ctx.pathMolecules = newPathMolecules;
                }

                assert(ctx.pathMolecules.size() >= 2);
                assert(ctx.pathMolecules[0].smile.compare(ctx.source.smile) == 0);

                int lastPathMolIndex = ctx.pathMolecules.size() - 1;
                assert(ctx.pathMolecules[lastPathMolIndex].smile.compare(ctx.target.smile) == 0);
                ctx.tempSource = MolpherMolecule(ctx.pathMolecules[lastPathMolIndex-1].smile,
                        ctx.pathMolecules[lastPathMolIndex-1].formula);
                ctx.tempSource.scaffoldLevelCreation =
                        ctx.pathMolecules[lastPathMolIndex-1].scaffoldLevelCreation;

                ctx.scaffoldSelector = static_cast<ScaffoldSelector>(ctx.scaffoldSelector + 1);

                Scaffold *scaff = ScaffoldDatabase::Get(ctx.scaffoldSelector);

                ctx.pathScaffoldMolecules.clear();
                for (int i = 0; i < ctx.pathMolecules.size(); ++i) {
                    std::string scaffMol;
                    scaff->GetScaffold(ctx.pathMolecules[i].smile, &scaffMol);
                    PathFinderContext::ScaffoldSmileMap::accessor ac;
                    ctx.pathScaffoldMolecules.insert(ac, scaffMol);
                    ac->second = ctx.pathMolecules[i].smile;
                }

                std::vector<MolpherMolecule>::iterator it;
                for (it = ctx.decoys.begin(); it != ctx.decoys.end(); ++it) {
                    std::string scaffDecoy;
                    scaff->GetScaffold(it->smile, &scaffDecoy);
                    it->scaffoldSmile = scaffDecoy;
                    it->scaffoldLevelCreation = ctx.scaffoldSelector;
                }

                // debugging output
                std::ostringstream stream;
                stream << "--- New path will be computed between " << ctx.tempSource.smile <<
                        " and " << ctx.target.smile << " on level " << ScaffoldLongDesc(ctx.scaffoldSelector) << ".";
                SynchCout(stream.str());

                std::string scaffTempSource;
                scaff->GetScaffold(ctx.tempSource.smile, &scaffTempSource);
                ctx.tempSource.scaffoldSmile = scaffTempSource;
                std::string scaffTarget;
                scaff->GetScaffold(ctx.target.smile, &scaffTarget);
                ctx.target.scaffoldSmile = scaffTarget;

                ctx.candidates.clear();
                ctx.candidates.insert(acCand, ctx.tempSource.smile);
                acCand->second = ctx.tempSource;

                ctx.candidateScaffoldMolecules.clear();
                ctx.candidateScaffoldMolecules.insert(acScaff, scaffTempSource);
                acScaff->second = ctx.tempSource.smile;

                assert(ctx.candidates.size() == ctx.candidateScaffoldMolecules.size());

                delete scaff;

                pathFound = false;
            }
        }

        // Save the snapshot to storage dir
        WriteSnapshotToFile(
                GenerateFilename(mStorageDir, snp.jobId, snp.iterIdx), snp);

        // update the old snapshot in job map
        JobGroup::JobMap::iterator it = mJobs.mJobMap.find(jobId);
        if (it != mJobs.mJobMap.end()) {
            it->second = snp;
        } else {
            // this should never occur
            assert(false);
        }

        PublishIteration(snp);
    }

    bool stayAlive = !flushJob && canContinue && !pathFound;
    if (!stayAlive) {
        bool putToSleep = flushJob || !pathFound;
        DeleteFromQueue(mJobs.mLiveJobQueue, jobId);
        if (putToSleep) {
            mJobs.mSleepingJobQueue.push_back(jobId);
        } else {
            mJobs.mFinishedJobQueue.push_back(jobId);
        }

        Lock lock(mDeferredActionsGuard); // this is not necessary here, but let's be formal
        ClearDeferredActions();
        PublishJobs();
    }

    return stayAlive;
}

bool JobManager::GetFingerprintSelector(FingerprintSelector &selector) {
    // mJobManagerGuard does not have to be locked.
    Lock lock(mDeferredActionsGuard);
    if (mDeferredFingerprintSelectorIsSet) {
        mDeferredFingerprintSelectorIsSet = false;
        selector = mDeferredFingerprintSelector;
        return true;
    } else {
        return false;
    }
}

bool JobManager::GetSimCoeffSelector(SimCoeffSelector &selector) {
    // mJobManagerGuard does not have to be locked.
    Lock lock(mDeferredActionsGuard);
    if (mDeferredSimCoeffSelectorIsSet) {
        mDeferredSimCoeffSelectorIsSet = false;
        selector = mDeferredSimCoeffSelector;
        return true;
    } else {
        return false;
    }
}

bool JobManager::GetDimRedSelector(DimRedSelector &selector) {
    // mJobManagerGuard does not have to be locked.
    Lock lock(mDeferredActionsGuard);
    if (mDeferredDimRedSelectorIsSet) {
        mDeferredDimRedSelectorIsSet = false;
        selector = mDeferredDimRedSelector;
        return true;
    } else {
        return false;
    }
}

bool JobManager::GetChemOperSelectors(std::vector<ChemOperSelector> &selectors) {
    // mJobManagerGuard does not have to be locked.
    Lock lock(mDeferredActionsGuard);
    if (mDeferredChemOperSelectorsIsSet) {
        mDeferredChemOperSelectorsIsSet = false;
        selectors = mDeferredChemOperSelectors;
        return true;
    } else {
        return false;
    }
}

bool JobManager::GetParams(MolpherParam &params) {
    // mJobManagerGuard does not have to be locked.
    Lock lock(mDeferredActionsGuard);
    if (mDeferredMolpherParamsIsSet) {
        mDeferredMolpherParamsIsSet = false;
        params = mDeferredMolpherParams;
        return true;
    } else {
        return false;
    }
}

bool JobManager::GetDecoys(std::vector<MolpherMolecule> &decoys) {
    // mJobManagerGuard does not have to be locked.
    Lock lock(mDeferredActionsGuard);
    if (mDeferredDecoysIsSet) {
        mDeferredDecoysIsSet = false;
        decoys = mDeferredDecoys;
        return true;
    } else {
        return false;
    }
}

void JobManager::GetPruned(std::vector<MolpherMolecule> &pruned) {
    // mJobManagerGuard does not have to be locked.
    Lock lock(mDeferredActionsGuard);
    pruned = mDeferredPrunedAccumulator; // copy the vector
    mDeferredPrunedAccumulator.clear();
}

void JobManager::OnConnect(std::string &backendId) {
    Lock lock(mJobManagerGuard);
    backendId = mBackendId;
    PublishJobs();
}

JobId JobManager::CreateJob(IterationSnapshot &snp, std::string &password) {
    Lock lock(mJobManagerGuard);
    JobId jobId = ++mJobIdCounter;
    snp.jobId = jobId;

    if (snp.chemOperSelectors.empty()) {
        snp.chemOperSelectors.push_back(OP_ADD_ATOM);
        snp.chemOperSelectors.push_back(OP_ADD_BOND);
        snp.chemOperSelectors.push_back(OP_BOND_CONTRACTION);
        snp.chemOperSelectors.push_back(OP_BOND_REROUTE);
        snp.chemOperSelectors.push_back(OP_INTERLAY_ATOM);
        snp.chemOperSelectors.push_back(OP_MUTATE_ATOM);
        snp.chemOperSelectors.push_back(OP_REMOVE_ATOM);
        snp.chemOperSelectors.push_back(OP_REMOVE_BOND);
    }

    if (snp.IsValid() && snp.IsActivityMorphingOn()) {
        snp.PrepareActivityData();
    }

    if (snp.IsValid()) { // Prevents backend crash (should be ensured by frontend).
        // Add the job to the live job queue
        mPasswordMap.insert(std::make_pair(jobId, password));
        mJobs.mJobMap.insert(std::make_pair(jobId, snp));
        mJobs.mLiveJobQueue.push_back(jobId);

        PublishJobs();
    } else {
        SynchCout("Ignoring invalid job.");
    }

    lock.unlock(); // Unlock to prevent deadlock when signalling the condition.
    mJobReadyCondition.notify_all(); // Live queue might had been empty.

    return jobId;
}

void JobManager::WakeJob(JobId jobId, std::string &password) {
    Lock lock(mJobManagerGuard);

    if (!VerifyPassword(jobId, password)) {
        return;
    }

    // If the job is sleeping, wake it up (put it to the end of live queue)
    if (mJobs.IsSleeping(jobId)) {
        DeleteFromQueue(mJobs.mSleepingJobQueue, jobId);
        mJobs.mLiveJobQueue.push_back(jobId);
    }

    PublishJobs();

    lock.unlock(); // Unlock to prevent deadlock when signalling the condition.
    mJobReadyCondition.notify_all(); // Live queue might had been empty.
}

void JobManager::SleepJob(JobId jobId, std::string &password) {
    Lock lock(mJobManagerGuard);

    if (!VerifyPassword(jobId, password)) {
        return;
    }

    if (mJobs.IsLive(jobId)) {
        if (IsJobRunning(jobId)) {
            mPathFinderStopper->cancel_group_execution();
            // Job will be put to sleep in CommitIteration.
        } else {
            // Move job to the end of sleep job queue
            DeleteFromQueue(mJobs.mLiveJobQueue, jobId);
            mJobs.mSleepingJobQueue.push_back(jobId);

            PublishJobs();
        }
    }
}

void JobManager::RemoveJob(JobId jobId, std::string &password) {
    Lock lock(mJobManagerGuard);

    if (!VerifyPassword(jobId, password)) {
        return;
    }

    if (!IsJobRunning(jobId)) {
        DeleteJob(jobId);
        PublishJobs();
    }
}

void JobManager::ChangeJobOrder(
        JobId jobId, int queuePosDiff, std::string &password) {
    Lock lock(mJobManagerGuard);

    if (!VerifyPassword(jobId, password)) {
        return;
    }

    // Negative diff means moving towards the beginning of the queue,
    // positive towards the end of queue
    bool forward = queuePosDiff < 0;
    queuePosDiff = abs(queuePosDiff);

    JobGroup::JobQueue &queue = mJobs.mLiveJobQueue;
    JobGroup::JobQueue::iterator curr = std::find(queue.begin(), queue.end(), jobId);
    // Move only live job which is not running
    if (IsJobRunning(jobId) || curr == queue.end()) {
        return;
    }

    JobGroup::JobQueue::iterator target;
    for (int i = 0; i < queuePosDiff; ++i) {
        target = curr;

        if (forward) {
            // Check whether the current position is not first already
            if (target == queue.begin()) {
                break;
            }
            // Move forward
            target--;

            // Verify the password of the target job
            if (!VerifyPassword(*target, password)) {
                break;
            }

            // Running job cannot be overtaken
            if (IsJobRunning(*target)) {
                break;
            }
        } else {
            // Move backwards and verify the target is not out of the queue
            // While moving backwards, verifying the target's password is not necessary
            target++;
            if (target == queue.end()) {
                break;
            }
        }

        // Swap current and target elements
        JobId tmp = *curr;
        *curr = *target;
        *target = tmp;

        curr = target;
    }

    PublishJobs();
}

bool JobManager::ValidateJobPassword(JobId jobId, std::string &password) {
    Lock lock(mJobManagerGuard);
    return VerifyPassword(jobId, password);
}

bool JobManager::GetJobHistory(JobId jobId, IterIdx iterIdx, IterationSnapshot &snp) {
    Lock lock(mJobManagerGuard);

    return ReadSnapshotFromFile(
            GenerateFilename(mStorageDir, jobId, iterIdx), snp);
}

bool JobManager::SetFingerprintSelector(
        JobId jobId, FingerprintSelector selector, std::string &password) {
    Lock lock(mJobManagerGuard);

    if (!VerifyPassword(jobId, password)) {
        SynchCout("SetFingerprintSelector: Invalid password");
        return false;
    }

    if (mJobs.IsLive(jobId) || mJobs.IsSleeping(jobId)) {
        if (IsJobRunning(jobId)) {
            Lock lock(mDeferredActionsGuard);
            mDeferredFingerprintSelector = selector;
            mDeferredFingerprintSelectorIsSet = true;
        } else {
            JobGroup::JobMap::iterator it = mJobs.mJobMap.find(jobId);

            if (it == mJobs.mJobMap.end()) {
                SynchCout("SetFingerprintSelector: Job does not exist");
                return false;
            }

            it->second.fingerprintSelector = selector;
            PublishJobs();
        }
    }

    return true;
}

bool JobManager::SetSimCoeffSelector(
        JobId jobId, SimCoeffSelector selector, std::string &password) {
    Lock lock(mJobManagerGuard);

    if (!VerifyPassword(jobId, password)) {
        SynchCout("SetSimCoeffSelector: Invalid password");
        return false;
    }

    if (mJobs.IsLive(jobId) || mJobs.IsSleeping(jobId)) {
        if (IsJobRunning(jobId)) {
            Lock lock(mDeferredActionsGuard);
            mDeferredSimCoeffSelector = selector;
            mDeferredSimCoeffSelectorIsSet = true;
        } else {
            JobGroup::JobMap::iterator it = mJobs.mJobMap.find(jobId);

            if (it == mJobs.mJobMap.end()) {
                SynchCout("SetSimCoeffSelector: Job does not exist");
                return false;
            }

            it->second.simCoeffSelector = selector;
            PublishJobs();
        }
    }

    return true;
}

bool JobManager::SetDimRedSelector(
        JobId jobId, DimRedSelector selector, std::string &password) {
    Lock lock(mJobManagerGuard);

    if (!VerifyPassword(jobId, password)) {
        SynchCout("SetDimRedSelector: Invalid password");
        return false;
    }

    if (mJobs.IsLive(jobId) || mJobs.IsSleeping(jobId)) {
        if (IsJobRunning(jobId)) {
            Lock lock(mDeferredActionsGuard);
            mDeferredDimRedSelector = selector;
            mDeferredDimRedSelectorIsSet = true;
        } else {
            JobGroup::JobMap::iterator it = mJobs.mJobMap.find(jobId);

            if (it == mJobs.mJobMap.end()) {
                SynchCout("SetDimRedSelector: Job does not exist");
                return false;
            }

            it->second.dimRedSelector = selector;
            PublishJobs();
        }
    }

    return true;
}

bool JobManager::SetChemOperSelectors(
        JobId jobId, std::vector<ChemOperSelector> &selectors, std::string &password) {
    Lock lock(mJobManagerGuard);

    if (!VerifyPassword(jobId, password)) {
        SynchCout("SetChemOperSelectors: Invalid password");
        return false;
    }

    if (selectors.empty()) {
        // Prevents backend crash (should be ensured by frontend).
        return false;
    }

    if (mJobs.IsLive(jobId) || mJobs.IsSleeping(jobId)) {
        if (IsJobRunning(jobId)) {
            Lock lock(mDeferredActionsGuard);
            mDeferredChemOperSelectors = selectors;
            mDeferredChemOperSelectorsIsSet = true;
        } else {
            JobGroup::JobMap::iterator it = mJobs.mJobMap.find(jobId);

            if (it == mJobs.mJobMap.end()) {
                SynchCout("SetChemOperSelectors: Job does not exist");
                return false;
            }

            it->second.chemOperSelectors.clear();
            it->second.chemOperSelectors.resize(selectors.size(), 0);
            for (size_t i = 0; i < selectors.size(); ++i) {
                it->second.chemOperSelectors[i] = selectors[i];
            }

            PublishJobs();
        }
    }

    return true;
}

bool JobManager::SetParams(
        JobId jobId, MolpherParam &params, std::string &password) {
    Lock lock(mJobManagerGuard);

    if (!VerifyPassword(jobId, password)) {
        SynchCout("SetParams: Invalid password");
        return false;
    }

    if (!params.IsValid()) {
        // Prevents backend crash (should be ensured by frontend).
        return false;
    }

    if (mJobs.IsLive(jobId) || mJobs.IsSleeping(jobId)) {
        if (IsJobRunning(jobId)) {
            Lock lock(mDeferredActionsGuard);
            mDeferredMolpherParams = params;
            mDeferredMolpherParamsIsSet = true;
        } else {
            JobGroup::JobMap::iterator it = mJobs.mJobMap.find(jobId);

            if (it == mJobs.mJobMap.end()) {
                SynchCout("SetParams: Job does not exist");
                return false;
            }

            it->second.params = params;
            PublishJobs();
        }
    }

    return true;
}

bool JobManager::SetDecoys(
        JobId jobId, std::vector<MolpherMolecule> &decoys, std::string &password) {
    Lock lock(mJobManagerGuard);

    if (!VerifyPassword(jobId, password)) {
        SynchCout("SetDecoys: Invalid password");
        return false;
    }

    bool decoysValid = true;
    std::vector<MolpherMolecule>::iterator it;
    for (it = decoys.begin(); it != decoys.end(); it++) {
        if (!it->IsValid()) {
            decoysValid = false;
            break;
        }
    }
    if (!decoysValid) {
        // Prevents backend crash (should be ensured by frontend).
        return false;
    }

    if (mJobs.IsLive(jobId) || mJobs.IsSleeping(jobId)) {
        if (IsJobRunning(jobId)) {
            Lock lock(mDeferredActionsGuard);
            mDeferredDecoys = decoys;
            mDeferredDecoysIsSet = true;
        } else {
            JobGroup::JobMap::iterator it = mJobs.mJobMap.find(jobId);

            if (it == mJobs.mJobMap.end()) {
                SynchCout("SetDecoys: Job does not exist");
                return false;
            }

            it->second.decoys = decoys;
            PublishJobs();
        }
    }

    return true;
}

bool JobManager::AddPruned(
        JobId jobId, std::vector<MolpherMolecule> &pruned, std::string &password) {
    Lock lock(mJobManagerGuard);

    if (!VerifyPassword(jobId, password)) {
        SynchCout("AddPruned: Invalid password");
        return false;
    }

    bool prunedValid = true;
    std::vector<MolpherMolecule>::iterator it;
    for (it = pruned.begin(); it != pruned.end(); it++) {
        if (!it->IsValid()) {
            prunedValid = false;
            break;
        }
    }
    if (!prunedValid) {
        // Prevents backend crash (should be ensured by frontend).
        return false;
    }

    if (IsJobRunning(jobId)) {
        Lock lock(mDeferredActionsGuard);

        mDeferredPrunedAccumulator.insert(
                mDeferredPrunedAccumulator.end(), pruned.begin(), pruned.end());

        return true;
    } else {
        SynchCout("AddPruned: Cannot prune molecules for non-running job");
        return false;
    }
}

void JobManager::PublishJobs() {
    // Already locked by caller.
}

void JobManager::PublishIteration(IterationSnapshot &snp) {
    // Already locked by caller.
}

bool JobManager::VerifyPassword(JobId jobId, std::string &password) {
    // Already locked by caller.
    PasswordMap::iterator it = mPasswordMap.find(jobId);

    if (it == mPasswordMap.end()) {
        return false;
    }

    return (it->second == password);
}

bool JobManager::DeleteFromQueue(JobGroup::JobQueue &queue, JobId jobId) {
    // Already locked by caller.
    JobGroup::JobQueue::iterator it = std::find(queue.begin(), queue.end(), jobId);
    if (it != queue.end()) {
        queue.erase(it);
        return true;
    }

    return false;
}

bool JobManager::GetFirstLiveJob(IterationSnapshot &snp) {
    // Already locked by caller.
    JobId jobId = mJobs.mLiveJobQueue.front();
    JobGroup::JobMap::iterator it = mJobs.mJobMap.find(jobId);
    bool found = false;

    if (it != mJobs.mJobMap.end()) {
        snp = it->second;
        found = true;
    }

    return found;
}

void JobManager::DeleteJob(JobId jobId) {
    // Already locked by caller.
    mJobs.mJobMap.erase(jobId);
    DeleteFromQueue(mJobs.mLiveJobQueue, jobId);
    DeleteFromQueue(mJobs.mSleepingJobQueue, jobId);
    DeleteFromQueue(mJobs.mFinishedJobQueue, jobId);
}

void JobManager::ClearDeferredActions() {
    // Already locked by caller.
    mDeferredFingerprintSelectorIsSet = false;
    mDeferredSimCoeffSelectorIsSet = false;
    mDeferredDimRedSelectorIsSet = false;
    mDeferredChemOperSelectorsIsSet = false;
    mDeferredMolpherParamsIsSet = false;
    mDeferredDecoysIsSet = false;

    // Clearing the atomic deferred actions, ChemOperSelectors and Decoys is not necessary.
    // These actions are set as a whole structure.
    // Just the PrunedAccumulator needs to be cleared.
    mDeferredPrunedAccumulator.clear();
}

bool JobManager::IsJobRunning(JobId jobId) {
    // Already locked by caller.
    JobGroup::JobQueue::iterator it = mJobs.mLiveJobQueue.begin();
    if (it != mJobs.mLiveJobQueue.end())
        return (*it) == jobId;

    return false;
}
