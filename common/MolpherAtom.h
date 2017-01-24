#pragma once

#include "GraphMol/Atom.h"

typedef int AtomicNum;

/**
 * Represent a single atom.
 */
struct MolpherAtom
{

    MolpherAtom(RDKit::Atom *atom) :
    atomicNum(atom->getAtomicNum()),
    formalCharge(atom->getFormalCharge()),
    isotope(atom->getIsotope())
    {
    }

    /**
     * @param atom
     * @return True if two atoms are equal.
     */
    bool equal(const MolpherAtom &atom)
    {
        return atomicNum == atom.atomicNum &&
                formalCharge == atom.formalCharge &&
                isotope == atom.formalCharge;
    }

public:

    AtomicNum atomicNum;

    int formalCharge;

    double isotope;

};


