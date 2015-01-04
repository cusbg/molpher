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

#include "ScaffoldDatabase.hpp"

#include "Oprea1Scaffold.hpp"
#include "Murcko2Scaffold.hpp"
#include "RingsWithLinkers1Scaffold.hpp"
#include "OriginalMoleculeScaffold.hpp"

Scaffold *ScaffoldDatabase::Get(ScaffoldSelector sel)
{
    Scaffold *scaff = NULL;

    switch (sel) {
    case SF_OPREA_1:
        scaff = new Oprea1Scaffold();
        break;
    case SF_MURCKO_2:
        scaff = new Murcko2Scaffold();
        break;
    case SF_RINGS_WITH_LINKERS_1:
        scaff = new RingsWithLinkers1Scaffold();
        break;
    case SF_ORIGINAL_MOLECULE:
        scaff = new OriginalMoleculeScaffold();
        break;
    default:
        assert(false && "Unknown scaffold");
        break;
    }

    return scaff;
}
