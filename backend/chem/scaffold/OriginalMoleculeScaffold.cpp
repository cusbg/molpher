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

#include "OriginalMoleculeScaffold.hpp"

void OriginalMoleculeScaffold::GetScaffold(std::string &mol, RDKit::RWMol **scaff)
{
    *scaff = GetMol(mol); // no modification
}

ScaffoldSelector OriginalMoleculeScaffold::GetSelector()
{
    return SF_ORIGINAL_MOLECULE;
}

std::vector<ChemOperSelector> OriginalMoleculeScaffold::GetUsefulOperators()
{
    // it should not happen - user selects the useful operators
    assert(false);

    return std::vector<ChemOperSelector>();
}
