/*
 Copyright (c) 2012 Peter Szepe

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

#include <cstring>
#include <string>
#include <sstream>

#include <GraphMol/GraphMol.h>
#include <GraphMol/SmilesParse/SmilesParse.h>

#include <tbb/atomic.h>
#include <tbb/partitioner.h>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>

#include "main.hpp"
#include "inout.h"
#include "chem/fingerprintStrategy/FingerprintStrategy.h"
#include "chem/simCoefStrategy/SimCoefStrategy.h"
#include "MorphingData.h"
#include "chem/morphingStrategy/MorphingStrategy.h"
#include "chem/morphing/MorphingFtors.hpp"
#include "chem/morphing/Morphing.hpp"

// TODO: merge into one header file ?
#include "chem/morphingStrategy/OpAddAtom.hpp"
#include "chem/morphingStrategy/OpAddBond.hpp"
#include "chem/morphingStrategy/OpBondContraction.hpp"
#include "chem/morphingStrategy/OpBondReroute.hpp"
#include "chem/morphingStrategy/OpInterlayAtom.hpp"
#include "chem/morphingStrategy/OpMutateAtom.hpp"
#include "chem/morphingStrategy/OpRemoveAtom.hpp"
#include "chem/morphingStrategy/OpRemoveBond.hpp"
#include "core/PathFinder.h"

#if MORPHING_REPORTING == 1
#define REPORT_RECOVERY(x) SynchCout((x))
#else
#define REPORT_RECOVERY(x)
#endif

static void InitStrategies(
    std::vector<ChemOperSelector> &chemOperSelectors,
    std::vector<MorphingStrategy *> &strategies)
{
    for (int i = 0; i < chemOperSelectors.size(); ++i) {
        switch (chemOperSelectors[i]) {
        case OP_ADD_ATOM:
            strategies.push_back(new OpAddAtom());
            break;
        case OP_REMOVE_ATOM:
            strategies.push_back(new OpRemoveAtom());
            break;
        case OP_ADD_BOND:
            strategies.push_back(new OpAddBond());
            break;
        case OP_REMOVE_BOND:
            strategies.push_back(new OpRemoveBond());
            break;
        case OP_MUTATE_ATOM:
            strategies.push_back(new OpMutateAtom());
            break;
        case OP_INTERLAY_ATOM:
            strategies.push_back(new OpInterlayAtom());
            break;
        case OP_BOND_REROUTE:
            strategies.push_back(new OpBondReroute());
            break;
        case OP_BOND_CONTRACTION:
            strategies.push_back(new OpBondContraction());
            break;
        default:
            break;
        }
    }
}

void GenerateMorphs(
    MolpherMolecule &candidate,
    unsigned int morphAttempts,
    std::vector<ChemOperSelector> &chemOperSelectors,
    tbb::task_group_context &tbbCtx,
    void *callerState,
    void (*deliver)(MolpherMolecule *, void *),
    int iteration)
{
    RDKit::RWMol *mol = NULL;
    try {
        mol = RDKit::SmilesToMol(candidate.smiles);
        if (mol) {
            RDKit::MolOps::Kekulize(*mol);
        } else {
            throw ValueErrorException("");
        }
    } catch (const ValueErrorException &exc) {
        delete mol;
        return;
    }

//    RDKit::RWMol *targetMol = nullptr;
//    if (target.smiles != "") {
//        try {
//            targetMol = RDKit::SmilesToMol(target.smiles);
//            if (targetMol) {
//                RDKit::MolOps::Kekulize(*targetMol);
//            } else {
//                throw ValueErrorException("");
//            }
//        } catch (const ValueErrorException &exc) {
//            delete targetMol;
//            delete mol;
//            return;
//        }
//    }

    // third and fourth parameter is used only by extended fingerprint and their
    // atoms are loaded. Scaffold morphs can contain atoms, which are not in
    // scaffold of "mol" or "targetMol", so the original molecules are always used.
//    SimCoefCalculator scCalc(simCoeffSelector, fingerprintSelector, mol, targetMol);

//    Fingerprint *targetFp = NULL;
//
//    if (targetMol != nullptr) {
//        targetFp = scCalc.GetFingerprint(targetMol);
//    }

//    std::vector<Fingerprint *> decoysFp;
//    decoysFp.reserve(decoys.size());
//    RDKit::RWMol *decoyMol = NULL;
//    try {
//        for (int i = 0; i < decoys.size(); ++i) {
//            decoyMol = RDKit::SmilesToMol(decoys[i].smiles);
//        }
//    } catch (const ValueErrorException &exc) {
//        REPORT_RECOVERY("Recovered from decoy kekulization failure.");
//        for (int i = 0; i < decoysFp.size(); ++i) {
//            delete decoysFp[i];
//        }
//        delete decoyMol;
//        if (targetMol != nullptr) {
//            delete targetMol;
//        }
//        delete mol;
//        if (targetFp != nullptr) {
//            delete targetFp;
//        }
//        return;
//    }

    std::vector<MorphingStrategy *> strategies;
    InitStrategies(chemOperSelectors, strategies);

    RDKit::RWMol **newMols = new RDKit::RWMol *[morphAttempts];
    std::memset(newMols, 0, sizeof(RDKit::RWMol *) * morphAttempts);
    ChemOperSelector *opers = new ChemOperSelector [morphAttempts];
    std::string *smiles = new std::string [morphAttempts];
//    std::string *formulas = new std::string [morphAttempts];
    double *weights = new double [morphAttempts];
//    double *distToTarget = new double [morphAttempts];
//    double *distToClosestDecoy = new double [morphAttempts];

    // compute new morphs and smiles
    if (!tbbCtx.is_group_execution_cancelled()) {
        tbb::atomic<unsigned int> kekulizeFailureCount;
        tbb::atomic<unsigned int> sanitizeFailureCount;
        tbb::atomic<unsigned int> morphingFailureCount;
        kekulizeFailureCount = 0;
        sanitizeFailureCount = 0;
        morphingFailureCount = 0;
        try {
            // TODO: If the target molecule is not given use current one,
            // this was done in the MOO, it may render some operations
            // unusable -> quick but dirty solution.
            MorphingData data(*mol, *mol, chemOperSelectors);

            CalculateMorphs calculateMorphs(
                data, strategies, opers, newMols, smiles,
                weights, kekulizeFailureCount,
                sanitizeFailureCount, morphingFailureCount);

            tbb::parallel_for(tbb::blocked_range<int>(0, morphAttempts),
                calculateMorphs, tbb::auto_partitioner(), tbbCtx);
        } catch (const std::exception &exc) {
            REPORT_RECOVERY("Recovered from morphing data construction failure.");
        }
        if (kekulizeFailureCount > 0) {
            std::stringstream report;
            report << "Recovered from " << kekulizeFailureCount << " kekulization failures.";
            REPORT_RECOVERY(report.str());
        }
        if (sanitizeFailureCount > 0) {
            std::stringstream report;
            report << "Recovered from " << sanitizeFailureCount << " sanitization failures.";
            REPORT_RECOVERY(report.str());
        }
        if (morphingFailureCount > 0) {
            std::stringstream report;
            report << "Recovered from " << morphingFailureCount << " morphing failures.";
            REPORT_RECOVERY(report.str());
        }
    }

//    // compute distances
//    // we need to announce the decoy which we want to use
//    if (!tbbCtx.is_group_execution_cancelled()) {
//        if (computeDistances) {
//            CalculateDistances calculateDistances(newMols,
//                scCalc, targetFp, decoysFp, distToTarget, distToClosestDecoy,
//                0/*candidate.nextDecoy*/);
//            tbb::parallel_for(tbb::blocked_range<int>(0, morphAttempts),
//                calculateDistances, tbb::auto_partitioner(), tbbCtx);
//        }
//    }

    // return results
    if (!tbbCtx.is_group_execution_cancelled()) {
        ReturnResults returnResults(newMols,
            smiles, candidate.smiles,
            opers, weights,
            callerState, deliver, iteration);
        tbb::parallel_for(tbb::blocked_range<int>(0, morphAttempts),
            returnResults, tbb::auto_partitioner(), tbbCtx);
    }

    for (int i = 0; i < morphAttempts; ++i) {
        delete newMols[i];
    }
    delete[] newMols;
    delete[] opers;
    delete[] smiles;
//    delete[] formulas;
    delete[] weights;
//    delete[] distToTarget;
//    delete[] distToClosestDecoy;
//
//    delete mol;
//    if (targetMol != nullptr) {
//        delete targetMol;
//    }
//    if (targetFp != nullptr) {
//        delete targetFp;
//    }
//
//    for (int i = 0; i < decoysFp.size(); ++i) {
//        delete decoysFp[i];
//    }

    for (int i = 0; i < strategies.size(); ++i) {
        delete strategies[i];
    }

}
