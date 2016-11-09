
#include "DescriptorSource.hpp"
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
    calculator.addMol(morph.id, morph.smile);
    morph.descriptorsFilePath = calculator.getOutputFilePath();
}

void PaDELDescriptorSource::compute()
{
    calculator.computeDescriptors();
}

std::map<std::string, double> PaDELDescriptorSource::get(
        const MolpherMolecule& morph)
{
    return calculator.getDescValues(morph.id);
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