/*
 Copyright (c) 2016 Petr Škoda

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

#include <map>
#include <vector>
#include <memory>

struct MolpherMolecule;

class DescriptorSource
{
public:
    virtual ~DescriptorSource();
public:
    /**
     * Called on every morph before the compute function.
     *
     * @param morph
     */
    virtual void add(MolpherMolecule& morph) = 0;
    /**
     * Called to compute descriptors for all added molecule.
     */
    virtual void compute() = 0;
    /**
     * Called after compute for all added molecule, should return
     * descriptors for given morph.
     *
     * @param morph
     * @return
     */
    virtual std::map<std::string, double> get(const MolpherMolecule& morph) = 0;
public:
    /**
     * Return a shared pointer to PaDEL descriptor object.
     *
     * @param PaDELPath Path to PaDEL.
     * @param workDirPath Path to working directory, to store temporary data in.
     * @param descriptor
     * @param threads Number of thread s to use.
     * @param descriptorsCSV
     * @return
     */
    static std::shared_ptr<DescriptorSource> createPaDEL(
            const std::string &PaDELPath, const std::string &workDirPath
            , const std::vector<std::string> &descriptor
            , const unsigned int threads
            , const std::string &descriptorsCSV);
    /**
     * Return a shared pointer to descriptor script source object.
     *
     * @param scriptPath
     * @param workDirPath
     * @param descriptor
     * @return
     */
    static std::shared_ptr<DescriptorSource> createScript(
            const std::string& scriptPath, const std::string &workDirPath,
            const std::vector<std::string> &descriptor
            );
};
