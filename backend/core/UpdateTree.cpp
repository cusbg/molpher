#include "UpdateTree.hpp"

#include <assert.h>
#include <limits.h>

UpdateTree::UpdateTree(CandidateMap &candidates) : mCandidates(candidates)
{
}

void UpdateTree::operator()(
        const SmileSet::range_type &modifiedParents) const
{
    for (auto itParent = modifiedParents.begin();
            itParent != modifiedParents.end(); itParent++) {

        // Determine what child is the closest to the target.
        double minDistance = std::numeric_limits<double>::max();
        CandidateMap::accessor acParent;
        if (mCandidates.find(acParent, itParent->first)) {

            for (auto itChild = acParent->second.descendants.begin();
                    itChild != acParent->second.descendants.end();
                    itChild++) {

                CandidateMap::const_accessor acChild;
                if (mCandidates.find(acChild, (*itChild))) {
                    if (acChild->second.score < minDistance) {
                        minDistance = acChild->second.score;
                    }
                } else {
                    assert(false);
                }

            }

        } else {
            assert(false);
        }

        // Update the tree branch towards root.
        while (!acParent->second.parentSmile.empty()) {
            if (minDistance < acParent->second.score) {
                acParent->second.itersWithoutDistImprovement = 0;
            }
            std::string smile = acParent->second.parentSmile;
            acParent.release();
            mCandidates.find(acParent, smile);
            assert(!acParent.empty());
        }

    }
}