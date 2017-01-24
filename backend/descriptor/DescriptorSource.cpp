
#include "DescriptorSource.hpp"
#include "MolpherMolecule.h"
#include "PaDELDescriptorCalculator.h"

DescriptorSource::~DescriptorSource()
{

}

/**
 * Implementation of PaDEL as a descriptor source.
 */
class PaDELDescriptorSource : DescriptorSource
{
public:
    PaDELDescriptorSource(const std::string &PaDELPath
            , const std::string &workDirPath, const std::vector<std::string> &descriptor
            , const unsigned int threads, const std::string &descriptorsCSV
            );
public:
    virtual void add(MolpherMolecule& morph);
    virtual void compute();
    virtual std::map<std::string, double> get(const MolpherMolecule& morph);
private:
    PaDELdesc::PaDELDescriptorCalculator calculator;
};

PaDELDescriptorSource::PaDELDescriptorSource(const std::string &PaDELPath
        , const std::string &workDirPath, const std::vector<std::string> &descriptor
        , const unsigned int threads, const std::string &descriptorsCSV)
: calculator(PaDELdesc::PaDELDescriptorCalculator(PaDELPath, workDirPath,
descriptor, threads, descriptorsCSV))
{

}

void PaDELDescriptorSource::add(MolpherMolecule& morph)
{
    calculator.addMol(morph.smiles, morph.smiles);
}

void PaDELDescriptorSource::compute()
{
    calculator.computeDescriptors();
}

std::map<std::string, double> PaDELDescriptorSource::get(
        const MolpherMolecule& morph)
{
    return calculator.getDescValues(morph.smiles);
}

std::shared_ptr<DescriptorSource> DescriptorSource::createPaDEL(
        const std::string &PaDELPath, const std::string &workDirPath
        , const std::vector<std::string> &descriptor
        , const unsigned int threads
        , const std::string &descriptorsCSV)
{
    PaDELDescriptorSource* source = new PaDELDescriptorSource(
            PaDELPath, workDirPath, descriptor, threads, descriptorsCSV);
    return std::shared_ptr<DescriptorSource>((DescriptorSource*) source);
}

class ScriptDescriptorSource : DescriptorSource
{
public:
    ScriptDescriptorSource(const std::string& scriptPath,
            const std::string &workDirPath,
            const std::vector<std::string> &descriptor);
public:
    virtual void add(MolpherMolecule& morph);
    virtual void compute();
    virtual std::map<std::string, double> get(const MolpherMolecule& morph);
private:
    std::vector<std::string> mSmiles;
    std::map<std::string, std::map<std::string, double>> mDescriptors;
    const std::string& mScriptPath;
    const std::string &mWorkDirPath;
    const std::vector<std::string> &mDescriptorNames;
};

ScriptDescriptorSource::ScriptDescriptorSource(const std::string& scriptPath,
        const std::string &workDirPath,
        const std::vector<std::string> &descriptor)
: mScriptPath(scriptPath), mWorkDirPath(workDirPath),
mDescriptorNames(descriptor)
{

}

void ScriptDescriptorSource::add(MolpherMolecule& morph)
{
    mSmiles.push_back(morph.smiles);
}

void ScriptDescriptorSource::compute()
{
    // @TODO Provide implementation !
    assert(false);

    // Write data to the file.

    // Execute script.

    // Load result data.

}

std::map<std::string, double> ScriptDescriptorSource::get(
        const MolpherMolecule& morph)
{
    std::map<std::string, double> result;
    for (auto iter = mDescriptorNames.begin(); iter != mDescriptorNames.end();
            ++iter) {
        result.insert(std::make_pair(*iter, 0.0));
    }
    return result;
    //
    return mDescriptors.at(morph.smiles);
}

std::shared_ptr<DescriptorSource> DescriptorSource::createScript(
        const std::string& scriptPath, const std::string &workDirPath,
        const std::vector<std::string> &descriptor)
{
    ScriptDescriptorSource* source = new ScriptDescriptorSource(scriptPath,
            workDirPath, descriptor);
    return std::shared_ptr<DescriptorSource>((DescriptorSource*) source);
}