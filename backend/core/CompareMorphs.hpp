#pragma once

#include "MolpherMolecule.h"

/**
 * Sort molecules in decreasing order.
 *
 * Use molecule score property for sorting.
 */
class CompareMorphs
{
public:
    /**
     *
     * @param useDistance If true sort in increasing order else in decreasing.
     */
    CompareMorphs(bool increasing);
    bool operator()(const MolpherMolecule &left,
            const MolpherMolecule &right) const;
private:
    bool mIncreasing;
};