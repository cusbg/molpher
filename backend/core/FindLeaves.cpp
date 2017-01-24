#include "FindLeaves.hpp"

FindLeaves::FindLeaves(MoleculeVector &leaves) : mLeaves(leaves)
{
}

void FindLeaves::operator()(const CandidateMap::range_type &candidates) const
{
    CandidateMap::iterator it;
    for (it = candidates.begin(); it != candidates.end(); it++) {
        if (!it->second.parentSmile.empty()) {
            it->second.itersWithoutDistImprovement++;
        }
        bool isLeaf = it->second.descendants.empty();
        if (isLeaf) {
            mLeaves.push_back(it->second);
        }
    }
}