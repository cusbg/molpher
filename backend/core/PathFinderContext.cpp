/*
 Copyright (c) 2012 Petr Koupy

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

#include "PathFinderContext.h"

void PathFinderContext::ContextToSnapshot(
    const PathFinderContext &ctx, IterationSnapshot &snp)
{
    snp.jobId = ctx.jobId;
    snp.iterIdx = ctx.iterIdx;
    snp.elapsedSeconds = ctx.elapsedSeconds;

    snp.fingerprintSelector = ctx.fingerprintSelector;
    snp.simCoeffSelector = ctx.simCoeffSelector;
    snp.dimRedSelector = ctx.dimRedSelector;

    snp.chemOperSelectors.clear();
    snp.chemOperSelectors.resize(ctx.chemOperSelectors.size(), 0);
    for (size_t i = 0; i < ctx.chemOperSelectors.size(); ++i) {
        snp.chemOperSelectors[i] = ctx.chemOperSelectors[i];
    }

    snp.params = ctx.params;

    snp.source = ctx.source;
    snp.target = ctx.target;
    snp.decoys = ctx.decoys;

    snp.candidates.clear();
    for (CandidateMap::const_iterator it = ctx.candidates.begin();
            it != ctx.candidates.end(); it++) {
        snp.candidates.insert(*it);
    }

    snp.morphDerivations.clear();
    for (MorphDerivationMap::const_iterator it = ctx.morphDerivations.begin();
            it != ctx.morphDerivations.end(); it++) {
        snp.morphDerivations.insert(*it);
    }

    snp.prunedDuringThisIter.clear();
    for (PrunedMoleculeVector::const_iterator it = ctx.prunedDuringThisIter.begin();
            it != ctx.prunedDuringThisIter.end(); it++) {
        snp.prunedDuringThisIter.push_back(*it);
    }
    
    snp.actives.clear();
    for (PathFinderContext::CandidateMap::const_iterator it = ctx.actives.begin();
            it != ctx.actives.end(); it++) {
        snp.actives.insert(*it);
    }
    
    snp.testActives.clear();
    for (PathFinderContext::CandidateMap::const_iterator it = ctx.testActives.begin();
            it != ctx.testActives.end(); it++) {
        snp.testActives.insert(*it);
    }
    
    snp.activesIDs.clear();
    snp.activesIDs.reserve(ctx.activesIDs.size());
    for (PathFinderContext::ConcStringVector::const_iterator it = ctx.activesIDs.begin();
            it != ctx.activesIDs.end(); it++) {
        snp.activesIDs.push_back(*it);
    }
    
    snp.etalonValues.clear();
    snp.etalonValues.reserve(ctx.etalonValues.size());
    for (PathFinderContext::ConcDoubleVector::const_iterator it = ctx.etalonValues.begin();
            it != ctx.etalonValues.end(); it++) {
        snp.etalonValues.push_back(*it);
    }
    
    snp.normalizationCoefficients.clear();
    snp.normalizationCoefficients.reserve(ctx.normalizationCoefficients.size());
    for (PathFinderContext::ConcDoublePairVector::const_iterator it = ctx.normalizationCoefficients.begin();
            it != ctx.normalizationCoefficients.end(); it++) {
        snp.normalizationCoefficients.push_back(*it);
    }
    
    snp.relevantDescriptorNames.clear();
    snp.relevantDescriptorNames.reserve(ctx.relevantDescriptorNames.size());
    for (PathFinderContext::ConcStringVector::const_iterator it = ctx.relevantDescriptorNames.begin();
            it != ctx.relevantDescriptorNames.end(); it++) {
        snp.relevantDescriptorNames.push_back(*it);
    }
    
//    snp.relevantDescriptorIndices.clear();
//    snp.relevantDescriptorIndices.reserve(ctx.relevantDescriptorIndices.size());
//    for (PathFinderContext::ConcBoolVector::const_iterator it = ctx.relevantDescriptorIndices.begin();
//            it != ctx.relevantDescriptorIndices.end(); it++) {
//        snp.relevantDescriptorIndices.push_back(*it);
//    }
    
    snp.activityMorphingInitialized = ctx.activityMorphingInitialized;
    snp.inputActivityDataDir = ctx.inputActivityDataDir;
    snp.proteinTargetName = ctx.proteinTargetName;
    snp.activesDescriptorsFile = ctx.activesDescriptorsFile;
    snp.saveDataAsCSVs = ctx.saveDataAsCSVs;
    snp.descWeights = ctx.descWeights;

    snp.tempSource = ctx.tempSource;
    snp.scaffoldSelector = ctx.scaffoldSelector;
    snp.pathMolecules = ctx.pathMolecules;

    snp.pathScaffoldMolecules.clear();
    for (ScaffoldSmileMap::const_iterator it = ctx.pathScaffoldMolecules.begin();
            it != ctx.pathScaffoldMolecules.end(); it++) {
        snp.pathScaffoldMolecules.insert(*it);
    }

    snp.candidateScaffoldMolecules.clear();
    for (ScaffoldSmileMap::const_iterator it = ctx.candidateScaffoldMolecules.begin();
            it != ctx.candidateScaffoldMolecules.end(); it++) {
        snp.candidateScaffoldMolecules.insert(*it);
    }
}

void PathFinderContext::SnapshotToContext(
    const IterationSnapshot &snp, PathFinderContext &ctx)
{
    ctx.jobId = snp.jobId;
    ctx.iterIdx = snp.iterIdx;
    ctx.elapsedSeconds = snp.elapsedSeconds;

    ctx.fingerprintSelector = (FingerprintSelector) snp.fingerprintSelector;
    ctx.simCoeffSelector = (SimCoeffSelector) snp.simCoeffSelector;
    ctx.dimRedSelector = (DimRedSelector) snp.dimRedSelector;

    ctx.chemOperSelectors.clear();
    ctx.chemOperSelectors.resize(snp.chemOperSelectors.size(), (ChemOperSelector) 0);
    for (size_t i = 0; i < snp.chemOperSelectors.size(); ++i) {
        ctx.chemOperSelectors[i] = (ChemOperSelector) snp.chemOperSelectors[i];
    }

    ctx.params = snp.params;

    ctx.source = snp.source;
    ctx.target = snp.target;
    ctx.decoys = snp.decoys;

    ctx.candidates.clear();
    for (IterationSnapshot::CandidateMap::const_iterator it = snp.candidates.begin();
            it != snp.candidates.end(); it++) {
        ctx.candidates.insert(*it);
    }

    ctx.morphDerivations.clear();
    for (IterationSnapshot::MorphDerivationMap::const_iterator it = snp.morphDerivations.begin();
            it != snp.morphDerivations.end(); it++) {
        ctx.morphDerivations.insert(*it);
    }

    ctx.prunedDuringThisIter.clear();
    ctx.prunedDuringThisIter.reserve(snp.prunedDuringThisIter.size());
    for (IterationSnapshot::PrunedMoleculeVector::const_iterator it = snp.prunedDuringThisIter.begin();
            it != snp.prunedDuringThisIter.end(); it++) {
        ctx.prunedDuringThisIter.push_back(*it);
    }
    
    ctx.actives.clear();
    for (IterationSnapshot::CandidateMap::const_iterator it = snp.actives.begin();
            it != snp.actives.end(); it++) {
        ctx.actives.insert(*it);
    }
    
    ctx.testActives.clear();
    for (IterationSnapshot::CandidateMap::const_iterator it = snp.testActives.begin();
            it != snp.testActives.end(); it++) {
        ctx.testActives.insert(*it);
    }
    
    ctx.activesIDs.clear();
    ctx.activesIDs.reserve(snp.activesIDs.size());
    for (std::vector<std::string>::const_iterator it = snp.activesIDs.begin();
            it != snp.activesIDs.end(); it++) {
        ctx.activesIDs.push_back(*it);
    }
    
    ctx.etalonValues.clear();
    ctx.etalonValues.reserve(snp.etalonValues.size());
    for (std::vector<double>::const_iterator it = snp.etalonValues.begin();
            it != snp.etalonValues.end(); it++) {
        ctx.etalonValues.push_back(*it);
    }
    
    ctx.normalizationCoefficients.clear();
    ctx.normalizationCoefficients.reserve(snp.normalizationCoefficients.size());
    for (std::vector<std::pair<double, double> >::const_iterator it = snp.normalizationCoefficients.begin();
            it != snp.normalizationCoefficients.end(); it++) {
        ctx.normalizationCoefficients.push_back(*it);
    }
    
    ctx.relevantDescriptorNames.clear();
    ctx.relevantDescriptorNames.reserve(snp.relevantDescriptorNames.size());
    for (std::vector<std::string>::const_iterator it = snp.relevantDescriptorNames.begin();
            it != snp.relevantDescriptorNames.end(); it++) {
        ctx.relevantDescriptorNames.push_back(*it);
    }
    
//    ctx.relevantDescriptorIndices.clear();
//    ctx.relevantDescriptorIndices.reserve(snp.relevantDescriptorIndices.size());
//    for (std::vector<bool>::const_iterator it = snp.relevantDescriptorIndices.begin();
//            it != snp.relevantDescriptorIndices.end(); it++) {
//        ctx.relevantDescriptorIndices.push_back(*it);
//    }
    
    ctx.activityMorphingInitialized = snp.activityMorphingInitialized;
    ctx.inputActivityDataDir = snp.inputActivityDataDir;
    ctx.proteinTargetName = snp.proteinTargetName;
    ctx.activesDescriptorsFile = snp.activesDescriptorsFile;
    ctx.saveDataAsCSVs = snp.saveDataAsCSVs;
    ctx.descWeights = snp.descWeights;
    
    ctx.tempSource = snp.tempSource;
    ctx.scaffoldSelector = (ScaffoldSelector) snp.scaffoldSelector;
    ctx.pathMolecules = snp.pathMolecules;

    ctx.pathScaffoldMolecules.clear();
    for (IterationSnapshot::ScaffoldSmileMap::const_iterator it = snp.pathScaffoldMolecules.begin();
            it != snp.pathScaffoldMolecules.end(); it++) {
        ctx.pathScaffoldMolecules.insert(*it);
    }

    ctx.candidateScaffoldMolecules.clear();
    for (IterationSnapshot::ScaffoldSmileMap::const_iterator it = snp.candidateScaffoldMolecules.begin();
            it != snp.candidateScaffoldMolecules.end(); it++) {
        ctx.candidateScaffoldMolecules.insert(*it);
    }
}

void PathFinderContext::ContextToLightSnapshot(
    const PathFinderContext &ctx, IterationSnapshot &snp)
{
    snp.jobId = ctx.jobId;
    snp.iterIdx = ctx.iterIdx;
    snp.elapsedSeconds = ctx.elapsedSeconds;

    snp.fingerprintSelector = ctx.fingerprintSelector;
    snp.simCoeffSelector = ctx.simCoeffSelector;
    snp.dimRedSelector = ctx.dimRedSelector;

    snp.chemOperSelectors.clear();
    snp.chemOperSelectors.resize(ctx.chemOperSelectors.size(), 0);
    for (size_t i = 0; i < ctx.chemOperSelectors.size(); ++i) {
        snp.chemOperSelectors[i] = ctx.chemOperSelectors[i];
    }

    snp.params = ctx.params;

    snp.source = ctx.source;
    snp.target = ctx.target;
    snp.decoys = ctx.decoys;
}

bool PathFinderContext::ScaffoldMode() const
{
    return (scaffoldSelector != SF_NONE);
}
