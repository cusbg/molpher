#include <fstream>
#include <GraphMol/SmilesParse/SmilesWrite.h>

#include "inout.h"
#include "CalculateDistances.hpp"

CalculateDistances::CalculateDistances(
        RDKit::RWMol **newMols,
        SimCoefCalculator &scCalc,
        Fingerprint *targetFp,
        std::vector<Fingerprint *> &decoysFp,
        double *distToTarget,
        double *distToClosestDecoy,
        int nextDecoy
        ) :
mNewMols(newMols),
mScCalc(scCalc),
mTargetFp(targetFp),
mDecoysFp(decoysFp),
mDistToTarget(distToTarget),
mDistToClosestDecoy(distToClosestDecoy),
mNextDecoy(nextDecoy)
{
    // no-op
}

void CalculateDistances::operator()(const tbb::blocked_range<int> &r) const
{
    Fingerprint *fp;
    double dist, minDist;
    for (int i = r.begin(); i != r.end(); ++i) {
        if (mNewMols[i]) {
            fp = mScCalc.GetFingerprint(mNewMols[i]);
            mDistToTarget[i] = mTargetFp ?
                    mScCalc.ConvertToDistance(mScCalc.GetSimCoef(mTargetFp, fp)) :
                    1.0; // set max. distance if target fingerprint does not exist

            dist = 0;
            minDist = DBL_MAX;
            for (int j = 0; j < mDecoysFp.size(); ++j) {
                dist = mScCalc.ConvertToDistance(
                        mScCalc.GetSimCoef(mDecoysFp[j], fp));
                if (dist < minDist) {
                    minDist = dist;
                }
            }
            mDistToClosestDecoy[i] = minDist;

            delete fp;
        }
    }
}

CalculateDistancesScript::CalculateDistancesScript(
        tbb::concurrent_vector<MolpherMolecule>& molecules,
        const std::string& script,
        const std::string& workingDirectory,
        unsigned int iteration,
        bool useDistance)
: mMolecules(molecules),
mScript(script),
mUseDistance(useDistance)
{
    std::stringstream ss;
    ss << workingDirectory << "/" << iteration << "_similarity/";
    mWorkingDirectory = ss.str();
    // Create working directory.
    try {
        boost::filesystem::create_directories(mWorkingDirectory);
    } catch (boost::filesystem::filesystem_error &exc) {
        SynchCout(exc.what());
    }
}

void CalculateDistancesScript::operator()() const
{
    // Create file with smiles.
    std::string smiles_file_path = mWorkingDirectory + "/smiles.smi";
    std::ofstream smiles_file(smiles_file_path);
    for (auto iter = mMolecules.begin(); iter != mMolecules.end(); ++iter) {
        smiles_file << iter->smiles << std::endl;
    }
    smiles_file.close();
    // Run external script.
    std::string distances_file_path = mWorkingDirectory + "/distances";
    std::string call = mScript + " -i \"" + smiles_file_path + "\""
            " -o \"" + distances_file_path + "\"";
    SynchCout("Executing command ... \n\t " + call);
    int ret = system(call.c_str());
    SynchCout("Executing command ... done ");
    assert(ret == 0);
    // Read results (CSV).
    std::ifstream distances_file(distances_file_path.c_str());

    double distance;
    std::string smiles = "";

    distances_file >> smiles >> distance;
    for (tbb::concurrent_vector<MolpherMolecule>::iterator
        iter = mMolecules.begin(); iter != mMolecules.end(); ++iter) {
        if (iter->smiles != smiles) {
            SynchCout("[WARNING] Missing record for: " + iter->smiles);
            // We use min/max value, this should cause
            // that the molecule will be skipped as it is sorted
            // at the end of the list.
            if (mUseDistance) {
                // Distance.
                iter->score = std::numeric_limits<float>::max();;
            } else {
                // Similarity.
                iter->score = std::numeric_limits<float>::min();;
            }
            continue;
        }
        iter->score = distance;
        distances_file >> smiles >> distance;
    }
    distances_file.close();

}