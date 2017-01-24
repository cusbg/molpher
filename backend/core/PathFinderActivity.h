/*
 * File:   PathFinderActivity.h
 * Author: Krwemil
 *
 * Created on 11. říjen 2014, 15:13
 */

#ifdef NOT_DEF

#pragma once

#include <string>
#include <vector>
#include <ctime>

#include <tbb/task.h>
#include <tbb/blocked_range.h>
#include <tbb/concurrent_vector.h>
#include <tbb/concurrent_hash_map.h>
#include <tbb/atomic.h>
#include <tbb/parallel_scan.h>
#include <tbb/parallel_do.h>

#include "MolpherParam.h"
#include "MolpherMolecule.h"
#include "IterationSnapshot.h"



class JobManager;

class PathFinderActivity
{
public:
    PathFinderActivity(tbb::task_group_context *tbbCtx,
        JobManager *jobManager, int threadCnt = 0);
    ~PathFinderActivity();

    void operator()();

// protected:
public:
    typedef tbb::concurrent_vector<MolpherMolecule> MoleculeVector;
    typedef tbb::concurrent_vector<std::string> SmileVector;
    typedef tbb::concurrent_hash_map<std::string, bool /*dummy*/> SmileSet;

    class FindLeaves
    {
    public:
        FindLeaves(MoleculeVector &leaves);
        void operator()(
            const PathFinderContext::CandidateMap::range_type &candidates) const;

    private:
        MoleculeVector &mLeaves;
    };

    class FindNextBag
    {
    public:
        FindNextBag(MoleculeVector &nextBag);
        void operator()(
            const PathFinderContext::CandidateMap::range_type &candidates) const;

    private:
        MoleculeVector &mNext;
    };

    friend void MorphCollector2(MolpherMolecule *morph, void *functor);

    class CollectMorphs
    {
    public:
        CollectMorphs(MoleculeVector &morphs);
        void operator()(const MolpherMolecule &morph);
        unsigned int WithdrawCollectAttemptCount();

    private:
        SmileSet mDuplicateChecker;
        MoleculeVector &mMorphs;
        tbb::atomic<unsigned int> mCollectAttemptCount;
    };

    class CompareMorphs
    {
    public:
        bool operator()(const MolpherMolecule &a, const MolpherMolecule &b) const;
    };

    class FilterMorphs
    {
    public:
        FilterMorphs(PathFinderContext &ctx, size_t globalMorphCount,
            MoleculeVector &morphs, std::vector<bool> &survivors);
        void operator()(const tbb::blocked_range<size_t> &r) const;

    private:
        PathFinderContext &mCtx;
        size_t mGlobalMorphCount;
        size_t mMorphCount;
        MoleculeVector &mMorphs;
        std::vector<bool> &mSurvivors;
    };

    class MOOPFilter
    {
    public:
        MOOPFilter(MoleculeVector &morphs, std::vector<bool> &survivors
                , std::vector<bool> &next);
        void operator()(const tbb::blocked_range<size_t> &r) const;

    private:
        size_t mMorphCount;
        MoleculeVector &mMorphs;
        std::vector<bool> &mSurvivors;
        std::vector<bool> &mNext;
    };

    class AcceptMorphs
    {
    public:
        AcceptMorphs(MoleculeVector &morphs, std::vector<bool> &survivors,
            PathFinderContext &ctx, SmileSet &modifiedParents);
        AcceptMorphs(AcceptMorphs &toSplit, tbb::split);
        void operator()(const tbb::blocked_range<size_t> &r, tbb::pre_scan_tag);
        void operator()(const tbb::blocked_range<size_t> &r, tbb::final_scan_tag);
        void reverse_join(AcceptMorphs &toJoin);
        void assign(AcceptMorphs &toAssign);
        unsigned int mSurvivorCount;

    private:
        MoleculeVector &mMorphs;
        std::vector<bool> &mSurvivors;
        PathFinderContext &mCtx;
        SmileSet &mModifiedParents;
    };

    class UpdateTree
    {
    public:
        UpdateTree(PathFinderContext &ctx);
        void operator()(const SmileSet::range_type &modifiedParents) const;

    private:
        PathFinderContext &mCtx;
    };

    class PruneTree
    {
    public:
        PruneTree(PathFinderContext &ctx, SmileSet &deferred);
        void operator()(const std::string &smile,
            tbb::parallel_do_feeder<std::string> &feeder) const;

    protected:
        void EraseSubTree(const std::string &root) const;

    private:
        PathFinderContext &mCtx;
        SmileSet &mDeferred;
    };

    class AccumulateTime
    {
    public:
        AccumulateTime(PathFinderContext &ctx);
        unsigned int GetElapsedSeconds(bool reset = false);
        void ReportElapsedMiliseconds(
            const std::string &consumer, bool reset = false);
        void Reset();

    private:
        PathFinderContext &mCtx;
        clock_t mTimestamp;
    };

    bool Cancelled();

    class SaveIterationData
    {
    public:
        static std::pair<double, double> getClosestTestActives(
            MolpherMolecule &mol
            , PathFinderContext::CandidateMap& probedMols
            , PathFinderContext& ctx
            , MolpherMolecule*& testActiveStruct
            , MolpherMolecule*& testActiveActivity);
        static void saveCSVData(
            MolpherMolecule& mol
            , PathFinderContext::CandidateMap& probedMols
            , PathFinderContext& ctx
            , CSVparse::CSV& morphingData);
    };

private:
    tbb::task_group_context *mTbbCtx;
    JobManager *mJobManager;
    int mThreadCnt;

    PathFinderContext mCtx;
};

#endif