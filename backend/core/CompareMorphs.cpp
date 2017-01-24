#include "CompareMorphs.hpp"

CompareMorphs::CompareMorphs(bool increasing) : mIncreasing(increasing)
{
}

/**
 * Return true if A is ordered before B.
 */
bool CompareMorphs::operator()(const MolpherMolecule &left,
        const MolpherMolecule &right) const
{
    bool result = left.score < right.score;
    if (mIncreasing) {
        // Smaller goes first, ie. left is before right (left < right).
        return result;
    } else {
        return !result;
    }
}
