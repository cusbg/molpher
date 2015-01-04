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

#include <GraphMol/MolOps.h>
#include <GraphMol/QueryOps.h>
#include <GraphMol/SanitException.h>
#include <GraphMol/SmilesParse/SmilesParse.h>
#include <GraphMol/SmilesParse/SmilesWrite.h>
#include <GraphMol/Substruct/SubstructMatch.h>

#include "inout.h"
#include "chem/ChemicalAuxiliary.h"
#include "Scaffold.hpp"
#include "auxiliary/SynchRand.h"

void Scaffold::GetScaffold(std::string &mol, std::string *scaff)
{
    scaff->clear();

    RDKit::RWMol *s = NULL;
    GetScaffold(mol, &s);
    if (!s) {
        return;
    }

    // scaffold is not empty molecule

    *scaff = RDKit::MolToSmiles(*s);

    delete s;
}

RDKit::RWMol *Scaffold::GetMol(std::string &smile)
{
    RDKit::RWMol *mol = NULL;
    try {
        mol = RDKit::SmilesToMol(smile);
        if (mol) {
            RDKit::MolOps::Kekulize(*mol);
        } else {
            throw ValueErrorException("");
        }
    } catch (const ValueErrorException &exc) {
        SynchCout("Scaffold creation failed.");
        delete mol;
        mol = NULL;
    }

    return mol;
}

bool Scaffold::IsEndStanding(const RDKit::Atom *atom, bool keepExocyclicDB, bool onlyToOxygen)
{
    if (RDKit::queryIsAtomInRing(atom)) {
        return false;
    }

    if (atom->getDegree() == 0) {
        return true;
    }

    if (atom->getDegree() == 1) {
        if (keepExocyclicDB && onlyToOxygen) {
            if (IsFirstAtomOfSmartsPattern(atom, "[$(O=[R]),$(O=[#6;D3;v4]),$(O=[#16;D4;v6]=O),$(O=[#16;D3;v4])]")) {
                return false;
            } else {
                return true;
            }
        }
        if (keepExocyclicDB && !onlyToOxygen) {
            if (IsFirstAtomOfSmartsPattern(atom, "[$([D1]=[R]),$([D1]=[#6;D3;v4]),$(O=[R]),$(O=[#6;D3;v4]),$(O=[#16;D4;v6]=O),$(O=[#16;D3;v4])]")) {
                return false;
            } else {
                return true;
            }
        }
        return true;
    }

    return false;
}

bool Scaffold::IsFirstAtomOfSmartsPattern(const RDKit::Atom *atom, const std::string &smarts)
{
    RDKit::RWMol *patternMol = NULL;
    try {
        patternMol = RDKit::SmartsToMol(smarts);
        if (patternMol) {
            RDKit::MolOps::Kekulize(*patternMol);
        }
    } catch (const ValueErrorException &exc) {
        SynchCout("Cannot kekulize output molecule.");
        delete patternMol;
        patternMol = NULL;
    } catch (const RDKit::MolSanitizeException &exc) {
        SynchCout("MolSanitizeException");
        delete patternMol;
        patternMol = NULL;
    } catch (const std::exception &exc) {
        SynchCout("std::exception");
        delete patternMol;
        patternMol = NULL;
    }

    RDKit::ROMol &atomMol = atom->getOwningMol();

    RDKit::MatchVectType machVect;
    if (RDKit::SubstructMatch(atomMol, *patternMol, machVect)) {

        // TODO get to know whether this atom matches the first atom in a given SMARTS pattern

        return true;
    }

    return false;
}

void Scaffold::Sanitize(RDKit::RWMol &mol)
{
    try {
        RDKit::MolOps::sanitizeMol(mol);
    } catch (const ValueErrorException &exc) {
        SynchCout(std::string("Cannot sanitize input molecule").append(exc.what()).append(",").append(exc.message()).append(")."));
    } catch (const std::exception &exc) {
        SynchCout(std::string("Cannot sanitize input molecule (").append(exc.what()).append(")."));
    }
}

void Scaffold::Kekulize(RDKit::RWMol &mol)
{
    try {
        RDKit::MolOps::Kekulize(mol);
    } catch (const ValueErrorException &exc) {
        SynchCout(std::string("Cannot kekulize input molecule").append(exc.what()).append(",").append(exc.message()).append(")."));
    } catch (const std::exception &exc) {
        SynchCout(std::string("Cannot kekulize input molecule (").append(exc.what()).append(")."));
    }
}

void Scaffold::RecreateMol(RDKit::RWMol **mol)
{
    RDKit::RWMol *copy = new RDKit::RWMol();
    CopyMol(**mol, *copy);
    delete *mol;
    *mol = copy;

    (*mol)->clearComputedProps();
    RDKit::MolOps::cleanUp(**mol);
    (*mol)->updatePropertyCache();
}

void Scaffold::SanitizeAndKekulize(RDKit::RWMol **mol)
{
    RecreateMol(mol);
    Sanitize(**mol);
    Kekulize(**mol);
}

void Scaffold::RemoveAtomAndJoinAllTwoNeighbors(RDKit::Atom *atom, RDKit::RWMol *mol)
{
    std::vector<unsigned int> nbrAtoms;
    GetNeighbors(*atom, nbrAtoms);
    assert(nbrAtoms.size() == 2);
    assert(nbrAtoms[0] != nbrAtoms[1]);

    mol->removeBond(nbrAtoms[0], atom->getIdx());
    assert(!mol->getBondBetweenAtoms(nbrAtoms[0], atom->getIdx()));
    assert(!mol->getBondBetweenAtoms(atom->getIdx(), nbrAtoms[0]));
    mol->removeBond(nbrAtoms[1], atom->getIdx());
    assert(!mol->getBondBetweenAtoms(nbrAtoms[1], atom->getIdx()));
    assert(!mol->getBondBetweenAtoms(atom->getIdx(), nbrAtoms[1]));

    mol->addBond(nbrAtoms[0], nbrAtoms[1], RDKit::Bond::SINGLE);
    mol->removeAtom(atom);
}
