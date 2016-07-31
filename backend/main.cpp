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

#include "inout.h"
#include "core/PathFinder.h"
#include "core/PathFinderActivity.h"
#include "core/NeighborhoodGenerator.h"
#include "core/JobManager.h"
#include "core/NeighborhoodTaskQueue.h"
#include "tests/MorphingTest.h"
// syntetize feasibility
#include "extensions/SAScore.h"

#include "Version.hpp"
#include "revision.h"

#include "iteration_serializer.hpp"

#ifndef RDKIT_LOGGING
    #define RDKIT_LOGGING 1
#endif

void Run(int argc, char *argv[])
{
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
        ("help", "Show help")
        ("legal", "Show legal information")
        ("storage-path,S", boost::program_options::value<std::string>(), "Storage path for results")
        ("job-list,L", boost::program_options::value<std::string>(), "Path to the job list file")
        ("interactive,I", boost::program_options::value<bool>(), "Enable/disable interactive mode")
        ("threads,T", boost::program_options::value<int>(), "Limit number of worker threads")
        ("job-file,J", boost::program_options::value<std::string>(), "Path to the job file.")
            ;

    boost::program_options::variables_map varMap;
    try {
        boost::program_options::store(
            boost::program_options::parse_command_line(argc, argv, desc), varMap);
    } catch (boost::program_options::error &exc) {
        std::cout << desc << std::endl;
        return;
    }
    boost::program_options::notify(varMap);

    if (varMap.count("help")) {
        std::cout << desc << std::endl;
        return;
    }

    if (varMap.count("legal")) {
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
        return;
    }

    // Default parameter values.
    std::string storagePath("Results");
    std::string jobListFile;
    std::string jobFile;
    bool interactiveSession = true;
    int threadCnt = 0;


    if (varMap.count("threads")) {
        threadCnt = varMap["threads"].as<int>();
    }

    if (varMap.count("interactive")) {
        interactiveSession = varMap["interactive"].as<bool>();
    }

    if (varMap.count("storage-path")) {
        storagePath = varMap["storage-path"].as<std::string>();
    }

    if (varMap.count("job-list")) {
        jobListFile = varMap["job-list"].as<std::string>();
    }

    if (varMap.count("job-file")) {
        jobFile = varMap["job-file"].as<std::string>();
    }

    std::cout << "Initializing..." << std::endl;

    tbb::task_group_context pathFinderTbbCtx;
    JobManager jobManager(
            &pathFinderTbbCtx, storagePath, jobListFile, interactiveSession);
    if (!jobFile.empty()) {
        jobManager.AddJobFromFile(jobFile);
    }

    // PathFinder pathFinder(&pathFinderTbbCtx, &jobManager, threadCnt);
    PathFinderActivity pathFinder(&pathFinderTbbCtx, &jobManager, threadCnt);
    std::thread pathFinderThread(std::tr1::ref(pathFinder));
    SynchCout(std::string("Backend initialized.\nWorking..."));
    pathFinderThread.join();
    SynchCout(std::string("Halting..."));
    jobManager.Halt();
    std::cout << "Backend terminated." << std::endl;
}

int main(int argc, char *argv[]) {
#if MORPHING_TEST_MODE == 1
    MorphingSandbox();
    return 0;
#endif

    // print just those information that we have
    std::cout << "Molpher backend version " << MOLPH_VERSION;
    if (MOLPH_REVISION != "")
        std::cout << " revision " << MOLPH_REVISION;
    std::cout << " built on " << MOLPH_DATE << ", " << MOLPH_TIME << std::endl;

    std::cout << "Copyright (c) 2012 Vladimir Fiklik, David Hoksza, Petr Koupy, Peter Szepe" << std::endl;
    std::cout << "          (c) 2014 Marek Mikes" << std::endl;
    std::cout << "          (c) 2016 Petr Skoda" << std::endl;

    SAScore::loadData(); // load data for prediction of synthetic feasibility

    Run(argc, argv);
    SAScore::destroyInstance(); // should free data, maybe not necessary

    return 0;
}