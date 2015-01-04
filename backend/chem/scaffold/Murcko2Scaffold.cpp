/*
 Copyright (c) 2014 Marek Mikes

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

#include <GraphMol/Bond.h>
#include <GraphMol/QueryOps.h>
#include <GraphMol/RDKitBase.h>

#include "Murcko2Scaffold.hpp"

void Murcko2Scaffold::GetScaffold(std::string &mol, RDKit::RWMol **scaff)
{
    *scaff = GetMol(mol);
    if (!*scaff)  {
        return;
    }

    RDKit::Atom *atom;
    RDKit::ROMol::AtomIterator aIter;
    bool removed = true;
    while (removed) {
        SanitizeAndKekulize(scaff);
        removed = false;
        for (aIter = (*scaff)->beginAtoms(); aIter != (*scaff)->endAtoms(); ++aIter) {
            atom = *aIter;
            if (IsEndStanding(atom, false, false)) {
                (*scaff)->removeAtom(atom);
                removed = true;
                break;
            }
        }
    }

    // Make all atoms as neutral C and all bond orders equal to 1
    for (aIter = (*scaff)->beginAtoms(); aIter != (*scaff)->endAtoms(); ++aIter) {
        atom = *aIter;
        atom->setAtomicNum(6);
        atom->setFormalCharge(0);
    }
    SanitizeAndKekulize(scaff);
    RDKit::Bond *bond;
    RDKit::ROMol::BondIterator bIter;
    for (bIter = (*scaff)->beginBonds(); bIter != (*scaff)->endBonds(); ++bIter) {
        bond = *bIter;
        bond->setBondType(RDKit::Bond::SINGLE);
    }

    // Transform all neighboring linker atoms into a single bond
    removed = true;
    while (removed) {
        SanitizeAndKekulize(scaff);
        removed = false;
        for (aIter = (*scaff)->beginAtoms(); aIter != (*scaff)->endAtoms(); ++aIter) {
            atom = *aIter;
            if (!RDKit::queryIsAtomInRing(atom) && (atom->getDegree() == 2)) {
                RemoveAtomAndJoinAllTwoNeighbors(atom, *scaff);
                removed = true;
                break;
            }
        }
    }

    if ((*scaff)->getNumAtoms() == 0) {
        delete *scaff;
        *scaff = NULL;
    }
}

ScaffoldSelector Murcko2Scaffold::GetSelector()
{
    return SF_MURCKO_2;
}

std::vector<ChemOperSelector> Murcko2Scaffold::GetUsefulOperators()
{
    std::vector<ChemOperSelector> result;

    result.push_back(OP_ADD_BOND);
    result.push_back(OP_BOND_CONTRACTION);
    result.push_back(OP_BOND_REROUTE);
    result.push_back(OP_INTERLAY_ATOM);
    result.push_back(OP_REMOVE_BOND);

    // operators which does not change scaffold:
    // Add Atom
    // Mutate Atom
    // Remove Atom

    return result;
}
