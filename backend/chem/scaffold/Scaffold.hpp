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

#pragma once

#include <string>
#include <vector>

#include <GraphMol/Atom.h>
#include <GraphMol/RWMol.h>

#include "../../../common/chemoper_selectors.h"
#include "../../../common/scaffold_selectors.hpp"

class Scaffold
{
public:
    void GetScaffold(std::string &mol, std::string *scaff);
    // caller is responsible for deallocation
    virtual void GetScaffold(std::string &mol, RDKit::RWMol **scaff) = 0;
    virtual ScaffoldSelector GetSelector() = 0;
    virtual std::vector<ChemOperSelector> GetUsefulOperators() = 0;

protected:
    RDKit::RWMol *GetMol(std::string& smile); // caller is responsible for deallocation
    bool IsEndStanding(const RDKit::Atom *atom, bool keepExocyclicDB = false,
        bool onlyToOxygen = false);
    void RemoveAtomAndJoinAllTwoNeighbors(RDKit::Atom *atom, RDKit::RWMol *mol);

    void SanitizeAndKekulize(RDKit::RWMol **mol);
    void RecreateMol(RDKit::RWMol **mol);
    void Sanitize(RDKit::RWMol &mol);
    void Kekulize(RDKit::RWMol &mol);

private:
    bool IsFirstAtomOfSmartsPattern(const RDKit::Atom *atom, const std::string &smarts);
};
