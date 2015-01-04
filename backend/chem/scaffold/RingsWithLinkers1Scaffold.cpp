/*
 Copyright (c) 2013 Marek Mikes

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

#include <GraphMol/RDKitBase.h>

#include "RingsWithLinkers1Scaffold.hpp"

void RingsWithLinkers1Scaffold::GetScaffold(std::string &mol, RDKit::RWMol **scaff)
{
    *scaff = GetMol(mol);
    if (!*scaff) {
        return;
    }

    RDKit::Atom *atom;
    RDKit::ROMol::AtomIterator iter;
    bool removed = true;
    while (removed) {
        SanitizeAndKekulize(scaff);
        removed = false;
        for (iter = (*scaff)->beginAtoms(); iter != (*scaff)->endAtoms(); ++iter) {
            atom = *iter;
            if (IsEndStanding(atom, false, false)) {
                (*scaff)->removeAtom(atom);
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

ScaffoldSelector RingsWithLinkers1Scaffold::GetSelector()
{
    return SF_RINGS_WITH_LINKERS_1;
}

std::vector<ChemOperSelector> RingsWithLinkers1Scaffold::GetUsefulOperators()
{
    std::vector<ChemOperSelector> result;

    result.push_back(OP_ADD_BOND);
    result.push_back(OP_BOND_CONTRACTION);
    result.push_back(OP_BOND_REROUTE);
    result.push_back(OP_INTERLAY_ATOM);
    result.push_back(OP_MUTATE_ATOM);
    result.push_back(OP_REMOVE_BOND);

    // operators which does not change scaffold:
    // Add Atom
    // Remove Atom

    return result;
}
