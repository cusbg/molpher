/*
 Copyright (c) 2012 Petr Koupy
 Copyright (c) 2012 Vladimir Fiklik

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

#include <string>
#include <iostream>
#include <tr1/functional>

#include <tbb/task.h>
#include <tbb/compat/thread>

#include <boost/program_options.hpp>

#include <RDGeneral/RDLog.h>

#include "inout.h"
#include "core/PathFinder.h"
#include "iteration_serializer.hpp"

#define MOLPH_VERSION "2.0.0"

void print_legal()
{
    std::cout << "This program is free software: you can redistribute it and/or modify" << std::endl;
    std::cout << "it under the terms of the GNU General Public License as published by" << std::endl;
    std::cout << "the Free Software Foundation, either version 3 of the License, or" << std::endl;
    std::cout << "(at your option) any later version." << std::endl;
    std::cout << std::endl;
    std::cout << "This program is distributed in the hope that it will be useful," << std::endl;
    std::cout << "but WITHOUT ANY WARRANTY; without even the implied warranty of" << std::endl;
    std::cout << "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the" << std::endl;
    std::cout << "GNU General Public License for more details." << std::endl;
    std::cout << std::endl;
    std::cout << "You should have received a copy of the GNU General Public License" << std::endl;
    std::cout << "along with this program.  If not, see http://www.gnu.org/licenses/." << std::endl;
    std::cout << std::endl;
    std::cout << "ACKNOWLEDGEMENTS" << std::endl;
    std::cout << std::endl;
    std::cout << "This program was developed under supervision of:" << std::endl;
    std::cout << "RNDr. David Hoksza, Ph.D. | SIRET research group, MFF UK, Prague" << std::endl;
    std::cout << "Doc. Daniel Svozil, Ph.D. | LICH, ICT, Prague" << std::endl;
    std::cout << std::endl;
    std::cout << "This program uses following GNU GPL compatible third-party software:" << std::endl;
    std::cout << "TBB by Intel Corporation" << std::endl;
    std::cout << "Boost by boost.org" << std::endl;
    std::cout << "RCF by Delta V Software" << std::endl;
    std::cout << "RDKit by Rational Discovery LLC" << std::endl;
    std::cout << "zlib by Jean-loup Gailly and Mark Adler" << std::endl;
}

void run(IterationSnapshot &snp, int threads)
{
    tbb::task_group_context pathFinderTbbCtx;
    PathFinder pathFinder(&pathFinderTbbCtx, snp, threads);
    std::thread pathFinderThread(std::tr1::ref(pathFinder));
    SynchCout(std::string("Backend initialized, working ..."));
    pathFinderThread.join();

    // Here we can still have some threads running in parallel.
    SynchCout(std::string("Halting..."));
    pathFinderTbbCtx.cancel_group_execution();
}

int main(int argc, char *argv[])
{
    std::cout << "Molpher version " << MOLPH_VERSION << " built on "
            << __DATE__ << ", " << __TIME__ << std::endl;
    std::cout << "Copyright (c) 2012 Vladimir Fiklik, "
            << "David Hoksza, Petr Koupy, Peter Szepe" << std::endl
            << "          (c) 2014 Marek Mikes" << std::endl
            << "          (c) 2017 Petr Skoda" << std::endl
            << std::endl;

    // Read input options.

    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
            ("help", "Show help")
            ("legal", "Show legal information")
            ("storage-path,S", boost::program_options::value<std::string>(), "Storage path for results")
            ("threads,T", boost::program_options::value<int>()->default_value(1), "Limit number of worker threads")
            ("iterations,I", boost::program_options::value<int>()->default_value(-1), "Number of iteration to execute.")
            ("job-file,J", boost::program_options::value<std::string>(), "Path to the job file.")
            ;

    boost::program_options::variables_map varMap;
    try {
        boost::program_options::store(
                boost::program_options::parse_command_line(argc, argv, desc),
                varMap);
    } catch (boost::program_options::error &exc) {
        std::cout << desc << std::endl;
        std::cout << "Error:" << exc.what() << std::endl;
        return 1;
    }
    boost::program_options::notify(varMap);

    if (varMap.count("help")) {
        std::cout << desc << std::endl;
        return 0;
    }

    if (varMap.count("legal")) {
        print_legal();
        return 0;
    }

    std::string jobFile;
    if (varMap.count("job-file")) {
        jobFile = varMap["job-file"].as<std::string>();
    } else {
        std::cout << "Missing job file!" << std::endl;
        return 1;
    }

    // Read job.
    std::cout << "Reading snapshot ..." << std::endl;
    IterationSnapshot snp;
    if (!ReadSnapshotFromFile(jobFile, snp)) {
        std::cout << "Invalid job file!" << std::endl;
        return 1;
    }

    // Modify job from command line arguments.
    int iterationsCnt = varMap["iterations"].as<int>();
    if (iterationsCnt != -1) {
        snp.params.cntIterations = snp.iterIdx + iterationsCnt;
    }
    if (varMap.count("storage-path")) {
        snp.storagePath = varMap["storage-path"].as<std::string>();
    }

    run(snp, varMap["threads"].as<int>());

    std::cout << "Backend terminated." << std::endl;

    return 0;
}
