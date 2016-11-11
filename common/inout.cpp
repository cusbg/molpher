/*
 Copyright (c) 2012 Petr Koupy
 Copyright (c) 2012 Vladimir Fiklik
 Copyright (c) 2012 Peter Szepe

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

#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include <boost/thread/mutex.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/archive_exception.hpp>

#include <GraphMol/FileParsers/MolSupplier.h>
#include <GraphMol/FileParsers/MolWriters.h>
#include <GraphMol/Descriptors/MolDescriptors.h>
#include <GraphMol/SmilesParse/SmilesParse.h>
#include <GraphMol/SmilesParse/SmilesWrite.h>
#include <GraphMol/MolOps.h>
#include <RDGeneral/BadFileException.h>

#include "chemoper_selectors.h"
#include "scaffold_selectors.hpp"
#include "inout.h"
#include "iteration_serializer.hpp"

boost::mutex gIoMutex;

void SynchCout(const std::string &s) {
    boost::mutex::scoped_lock lock(gIoMutex);
    std::cout << s << std::endl;
}

std::string GenerateSnaphostFilenameWithoutExtension(
        std::string &base, JobId jobId, unsigned int iterIdx) {
    std::stringstream filename;
    filename << base << "/" << jobId << "/" << std::setfill('0')
            << std::setw(3) << iterIdx;
    return filename.str();
}

std::string GenerateSmilesFilename(
        std::string &base, unsigned int iterIdx) {
    std::stringstream filename;
    filename << base << "/" << iterIdx << ".smi";
    return filename.str();
}

std::string GenerateFilename(
    std::string &base, JobId jobId, std::string name) {
    std::stringstream filename;
    filename << base << "/" << jobId << "/" << name;
    return filename.str();
}

std::string GenerateDirname(std::string &base, JobId jobId) {
    std::stringstream dirname;
    dirname << base << "/" << jobId;
    return dirname.str();
}

std::string GenerateDirname(std::string &base, JobId jobId, const std::string& targetId) {
    std::stringstream dirname;
    dirname << base << "/" << jobId << "/" << targetId;
    return dirname.str();
}

std::string GenerateDirname(std::string &base, const std::string& targetId) {
    std::stringstream dirname;
    dirname << base << "/" << targetId;
    return dirname.str();
}

void WriteMolpherPath(const std::string &file, const std::string &targetSmile,
        const IterationSnapshot::CandidateMap &candidates,
        const std::vector<MolpherMolecule> &pathMols) {
    std::vector<std::string> smiles;
    std::vector<const MolpherMolecule*> mols;
    std::vector<ScaffoldSelector> scaffs;
    std::vector<ChemOperSelector> opers;

    IterationSnapshot::CandidateMap::const_iterator it;
    it = candidates.find(targetSmile);
    assert(it != candidates.end());
    smiles.push_back(targetSmile);
    mols.push_back(&(it->second));
    scaffs.push_back((ScaffoldSelector) it->second.scaffoldLevelCreation);

    while (!it->second.parentSmile.empty()) {
        opers.push_back((ChemOperSelector) it->second.parentChemOper);
        it = candidates.find(it->second.parentSmile);
        assert(it != candidates.end());
        smiles.push_back(it->second.smile);
        mols.push_back(&(it->second));
        scaffs.push_back((ScaffoldSelector) it->second.scaffoldLevelCreation);
    }

    if (!pathMols.empty()) { // scaffold hopping is enabled
        assert(pathMols.size() >= 2); // at least source and target are in
        std::vector<MolpherMolecule>::const_reverse_iterator rIt;
        rIt = pathMols.rbegin();
        // last path molecule is target
        assert(rIt->smile.compare(targetSmile) == 0);
        ++rIt;
        // next-to-last path molecule is temporary source
        assert(rIt->smile.compare(it->second.smile) == 0);
        while (!rIt->parentSmile.empty()) {
            opers.push_back((ChemOperSelector) rIt->parentChemOper);
            ++rIt;
            assert(rIt != pathMols.rend());
            smiles.push_back(rIt->smile);
            scaffs.push_back((ScaffoldSelector) rIt->scaffoldLevelCreation);
        }
    }

    assert(smiles.size() == scaffs.size() && scaffs.size() == opers.size() + 1);

    std::ofstream outStream;
    outStream.open(file.c_str());


    while (!smiles.empty() || !opers.empty()) {
        if (!outStream.good()) {
            SynchCout(std::string("Cannot write to file: ").append(file));
            break;
        }

        if (!smiles.empty()) {
            assert(!scaffs.empty());
            outStream << smiles.back()
                    << ";" << "Dist to etalon: " << mols.back()->distToEtalon
                    << ";" << "SAScore: " << mols.back()->sascore
                    << ";" << ScaffoldLongDesc(scaffs.back()) << std::endl;
            smiles.pop_back();
            mols.pop_back();
            scaffs.pop_back();
        }

        if (!opers.empty()) {
            outStream << opers.back() << ":" <<
                ChemOperShortDesc(opers.back()) << std::endl;
            opers.pop_back();
        }
    }

    outStream.close();
}

void WriteSnapshotToFile(const std::string &file, const IterationSnapshot &snp) {
    (molpher::iteration::IterationSerializer()).save(file + ".snp", snp);
    (molpher::iteration::IterationSerializer()).save(file + ".json", snp);
}

bool ReadSnapshotFromFile(const std::string &file, IterationSnapshot &snp) {
    return (molpher::iteration::IterationSerializer()).load(file, snp);
}

void GatherMolphMols(const IterationSnapshot::CandidateMap &toGather,
        std::map<std::string, MolpherMolecule> &gathered) {
    IterationSnapshot::CandidateMap::const_iterator it;
    for (it = toGather.begin(); it != toGather.end(); ++it) {
        gathered.insert(std::make_pair(it->first, it->second)); // filter duplicates
    }
}

void GatherMolphMols(const std::vector<MolpherMolecule> &toGather,
        std::map<std::string, MolpherMolecule> &gathered) {
    std::vector<MolpherMolecule>::const_iterator it;
    for (it = toGather.begin(); it != toGather.end(); ++it) {
        gathered.insert(std::make_pair(it->smile, *it)); // filter duplicates
    }
}

void WriteMolphMolsToSDF(const std::string &file,
        const std::map<std::string, MolpherMolecule> &mols) {
    std::vector<RDKit::RWMol *> rdkMols;
    rdkMols.reserve(mols.size());

    std::map<std::string, MolpherMolecule>::const_iterator it;
    for (it = mols.begin(); it != mols.end(); ++it) {
        RDKit::RWMol *mol = NULL;
        try {
            mol = RDKit::SmilesToMol(it->second.smile);
            if (mol) {
                RDKit::MolOps::Kekulize(*mol);
            } else {
                throw ValueErrorException("");
            }
        } catch (const ValueErrorException &exc) {
            SynchCout("Cannot kekulize output molecule.");
        }
        if (mol) {
            rdkMols.push_back(mol);
        }
    }

    WriteRWMolsToSDF(file, rdkMols);
    for (size_t i = 0; i < rdkMols.size(); ++i) {
        delete rdkMols[i];
    }
}

void WriteMolphMolsToSDF(const std::string &file,
        const std::vector<MolpherMolecule> &mols) {
    std::vector<RDKit::RWMol *> rdkMols;
    rdkMols.reserve(mols.size());

    std::vector<MolpherMolecule>::const_iterator it;
    for (it = mols.begin(); it != mols.end(); ++it) {
        RDKit::RWMol *mol = NULL;
        try {
            mol = RDKit::SmilesToMol(it->smile);
            if (mol) {
                RDKit::MolOps::Kekulize(*mol);
            } else {
                throw ValueErrorException("");
            }
        } catch (const ValueErrorException &exc) {
            SynchCout("Cannot kekulize output molecule.");
        }
        if (mol) {
            rdkMols.push_back(mol);
        }
    }

    WriteRWMolsToSDF(file, rdkMols);
    for (size_t i = 0; i < rdkMols.size(); ++i) {
        delete rdkMols[i];
    }
}

/**
 * Read molecules from txt file where each line contains just one smile
 * without white spaces.
 * @param file
 * @param mols
 */
void ReadRWMolFromTxtFile(const std::string &file,
        std::vector<RDKit::RWMol *> &mols) {
    std::ifstream stream;
    stream.open(file.c_str());

    if (stream.is_open()) {
		while (stream.good())  {
            std::string line;
			std::getline(stream, line);
            if (line.empty()) {
                // skip empty line
                continue;
            }
			// parse smiles ..
            try {
                RDKit::RWMol* mol = RDKit::SmilesToMol(line);
                // add to result
                mols.push_back(mol);
            } catch (std::exception e) {
                // failed to load the moll
                SynchCout("Failed to load mol from smile: " + line);
            }
		}
		stream.close();
	} else {
        // failed to open file
    }
}

void ReadMolphMolsFromFile(const std::string &file,
        std::vector<MolpherMolecule> &mols) {
    std::vector<RDKit::RWMol *> rdkMols;

    std::string fileLowerCase = file;
    assert(false);
//    FIXME this line doesn't compile with g++ 4.6 for some reason
//    this function is not needed by backend -> will fix later
//    std::transform(fileLowerCase.begin(), fileLowerCase.end(), fileLowerCase.begin(), tolower);
    if (fileLowerCase.find(".sdf") != std::string::npos ||
        fileLowerCase.find(".mol") != std::string::npos) {
        // sdf, mol file ..
        ReadRWMolsFromSDF(file, rdkMols);
    } else if (fileLowerCase.find(".txt") != std::string::npos) {
        // txt file with smiles
        ReadRWMolFromTxtFile(file, rdkMols);
    } else {
        // unknown format
        return;
    }

    mols.reserve(rdkMols.size());
    std::vector<RDKit::RWMol *>::iterator it;
    for (it = rdkMols.begin(); it != rdkMols.end(); ++it) {
        RDKit::RWMol *mol = *it;
        try {
            RDKit::MolOps::Kekulize(*mol);
        } catch (const ValueErrorException &exc) {
            SynchCout("Cannot kekulize input molecule.");
        }
        std::string smile(RDKit::MolToSmiles(*mol));
        std::string formula(RDKit::Descriptors::calcMolFormula(*mol));
        mols.push_back(MolpherMolecule(smile, formula));
        delete mol;
    }
}

void WriteRWMolsToSDF(const std::string &file,
        std::vector<RDKit::RWMol *> &mols) {
    try {
        RDKit::SDWriter writer(file);

        std::vector<RDKit::RWMol *>::iterator it;
        for (it = mols.begin(); it != mols.end(); ++it) {
            RDKit::RWMol *mol = *it;
            writer.write(*mol);
        }

        writer.close();
    } catch (RDKit::BadFileException &exc) {
        SynchCout(std::string("Cannot write to file: ").append(file));
    }
}

void ReadRWMolsFromSDF(const std::string &file,
        std::vector<RDKit::RWMol *> &mols) {
    try {
        RDKit::SDMolSupplier supplier(file);

        mols.reserve(supplier.length());
        for (size_t i = 0; i < supplier.length(); ++i) {
            RDKit::ROMol *mol = supplier.next();
            if (mol) {
                RDKit::RWMol *rwMol = new RDKit::RWMol(*mol);
                if (rwMol) {
                    mols.push_back(rwMol);
                }
                delete mol;
            }
        }
    } catch (RDKit::BadFileException &exc) {
        SynchCout(std::string("Cannot read from file: ").append(file));
    }
}
