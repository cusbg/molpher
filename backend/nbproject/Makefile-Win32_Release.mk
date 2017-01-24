#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=MinGW-Windows
CND_DLIB_EXT=dll
CND_CONF=Win32_Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/b4460d1a/activity_data_processing.o \
	${OBJECTDIR}/_ext/b4460d1a/chemoper_selectors.o \
	${OBJECTDIR}/_ext/b4460d1a/fingerprint_selectors.o \
	${OBJECTDIR}/_ext/b4460d1a/inout.o \
	${OBJECTDIR}/_ext/b4460d1a/iteration_serializer.o \
	${OBJECTDIR}/_ext/b4460d1a/simcoeff_selectors.o \
	${OBJECTDIR}/auxiliary/SynchRand.o \
	${OBJECTDIR}/chem/ChemicalAuxiliary.o \
	${OBJECTDIR}/chem/SimCoefCalculator.o \
	${OBJECTDIR}/chem/fingerprintStrategy/AtomPairsFngpr.o \
	${OBJECTDIR}/chem/fingerprintStrategy/FingerprintStrategy.o \
	${OBJECTDIR}/chem/fingerprintStrategy/MorganFngpr.o \
	${OBJECTDIR}/chem/fingerprintStrategy/TopolTorsFngpr.o \
	${OBJECTDIR}/chem/morphing/CalculateDistances.o \
	${OBJECTDIR}/chem/morphing/FilterScript.o \
	${OBJECTDIR}/chem/morphing/Morphing.o \
	${OBJECTDIR}/chem/morphing/MorphingData.o \
	${OBJECTDIR}/chem/morphing/MorphingFtors.o \
	${OBJECTDIR}/chem/morphingStrategy/OpAddAtom.o \
	${OBJECTDIR}/chem/morphingStrategy/OpAddBond.o \
	${OBJECTDIR}/chem/morphingStrategy/OpBondContraction.o \
	${OBJECTDIR}/chem/morphingStrategy/OpBondReroute.o \
	${OBJECTDIR}/chem/morphingStrategy/OpInterlayAtom.o \
	${OBJECTDIR}/chem/morphingStrategy/OpMutateAtom.o \
	${OBJECTDIR}/chem/morphingStrategy/OpRemoveAtom.o \
	${OBJECTDIR}/chem/morphingStrategy/OpRemoveBond.o \
	${OBJECTDIR}/chem/simCoefStrategy/TanimotoSimCoef.o \
	${OBJECTDIR}/core/AcceptMorphs.o \
	${OBJECTDIR}/core/CollectMorphs.o \
	${OBJECTDIR}/core/CompareMorphs.o \
	${OBJECTDIR}/core/FilterMorphs.o \
	${OBJECTDIR}/core/FindLeaves.o \
	${OBJECTDIR}/core/PathFinder.o \
	${OBJECTDIR}/core/PathFinderActivity.o \
	${OBJECTDIR}/core/PruneTree.o \
	${OBJECTDIR}/core/UpdateTree.o \
	${OBJECTDIR}/csv.o \
	${OBJECTDIR}/descriptor/BioChemLibDescriptors.o \
	${OBJECTDIR}/descriptor/CSV.o \
	${OBJECTDIR}/descriptor/DataConverter.o \
	${OBJECTDIR}/descriptor/DescriptorSource.o \
	${OBJECTDIR}/descriptor/PaDELDescriptorCalculator.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/molpher_molecule.o \
	${OBJECTDIR}/script_execution.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-m32 -DNOT_NETBEANS -Wno-deprecated -Wno-write-strings -Wno-attributes -Wno-strict-aliasing -fpermissive
CXXFLAGS=-m32 -DNOT_NETBEANS -Wno-deprecated -Wno-write-strings -Wno-attributes -Wno-strict-aliasing -fpermissive

# Fortran Compiler Flags
FFLAGS=-s

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=../dependencies/rdkit/lib/libDistGeomHelpers.a ../dependencies/rdkit/lib/libMolAlign.a ../dependencies/rdkit/lib/libAlignment.a ../dependencies/rdkit/lib/libFragCatalog.a ../dependencies/rdkit/lib/libMolCatalog.a ../dependencies/rdkit/lib/libCatalogs.a ../dependencies/rdkit/lib/libChemReactions.a ../dependencies/rdkit/lib/libDepictor.a ../dependencies/rdkit/lib/libDescriptors.a ../dependencies/rdkit/lib/libDistGeometry.a ../dependencies/rdkit/lib/libFileParsers.a ../dependencies/rdkit/lib/libFingerprints.a ../dependencies/rdkit/lib/libSubgraphs.a ../dependencies/rdkit/lib/libForceFieldHelpers.a ../dependencies/rdkit/lib/libChemTransforms.a ../dependencies/rdkit/lib/libMolChemicalFeatures.a ../dependencies/rdkit/lib/libSubstructMatch.a ../dependencies/rdkit/lib/libSmilesParse.a ../dependencies/rdkit/lib/libPartialCharges.a ../dependencies/rdkit/lib/libShapeHelpers.a ../dependencies/rdkit/lib/libMolTransforms.a ../dependencies/rdkit/lib/libSLNParse.a ../dependencies/rdkit/lib/libGraphMol.a ../dependencies/rdkit/lib/libForceField.a ../dependencies/rdkit/lib/libRDGeometryLib.a ../dependencies/rdkit/lib/libDataStructs.a ../dependencies/rdkit/lib/libOptimizer.a ../dependencies/rdkit/lib/libEigenSolvers.a ../dependencies/rdkit/lib/libChemicalFeatures.a ../dependencies/rdkit/lib/libSimDivPickers.a ../dependencies/rdkit/lib/libRDGeneral.a ../dependencies/rdkit/lib/libhc.a ../dependencies/rcf/bin/libRcfLib.a ../dependencies/boost/stage/lib/libboost_date_time-mgw46-mt-1_49.a ../dependencies/boost/stage/lib/libboost_filesystem-mgw46-mt-1_49.a ../dependencies/boost/stage/lib/libboost_program_options-mgw46-mt-1_49.a ../dependencies/boost/stage/lib/libboost_regex-mgw46-mt-1_49.a ../dependencies/boost/stage/lib/libboost_wserialization-mgw46-mt-1_49.a ../dependencies/boost/stage/lib/libboost_serialization-mgw46-mt-1_49.a ../dependencies/boost/stage/lib/libboost_signals-mgw46-mt-1_49.a ../dependencies/boost/stage/lib/libboost_thread-mgw46-mt-1_49.a ../dependencies/boost/stage/lib/libboost_system-mgw46-mt-1_49.a ../dependencies/zlib/libz.a ../dependencies/tbb/build/windows_ia32_gcc_mingw_release/tbb.dll ../dependencies/tbb/build/windows_ia32_gcc_mingw_release/tbbmalloc_proxy.dll ../dependencies/tbb/build/windows_ia32_gcc_mingw_release/tbbmalloc.dll -lws2_32 -lmswsock -lcrypt32

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ../dependencies/rdkit/lib/libDistGeomHelpers.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ../dependencies/rdkit/lib/libMolAlign.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ../dependencies/rdkit/lib/libAlignment.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ../dependencies/rdkit/lib/libFragCatalog.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ../dependencies/rdkit/lib/libMolCatalog.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ../dependencies/rdkit/lib/libCatalogs.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ../dependencies/rdkit/lib/libChemReactions.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ../dependencies/rdkit/lib/libDepictor.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ../dependencies/rdkit/lib/libDescriptors.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ../dependencies/rdkit/lib/libDistGeometry.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ../dependencies/rdkit/lib/libFileParsers.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ../dependencies/rdkit/lib/libFingerprints.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ../dependencies/rdkit/lib/libSubgraphs.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ../dependencies/rdkit/lib/libForceFieldHelpers.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ../dependencies/rdkit/lib/libChemTransforms.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ../dependencies/rdkit/lib/libMolChemicalFeatures.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ../dependencies/rdkit/lib/libSubstructMatch.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ../dependencies/rdkit/lib/libSmilesParse.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ../dependencies/rdkit/lib/libPartialCharges.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ../dependencies/rdkit/lib/libShapeHelpers.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ../dependencies/rdkit/lib/libMolTransforms.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ../dependencies/rdkit/lib/libSLNParse.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ../dependencies/rdkit/lib/libGraphMol.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ../dependencies/rdkit/lib/libForceField.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ../dependencies/rdkit/lib/libRDGeometryLib.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ../dependencies/rdkit/lib/libDataStructs.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ../dependencies/rdkit/lib/libOptimizer.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ../dependencies/rdkit/lib/libEigenSolvers.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ../dependencies/rdkit/lib/libChemicalFeatures.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ../dependencies/rdkit/lib/libSimDivPickers.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ../dependencies/rdkit/lib/libRDGeneral.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ../dependencies/rdkit/lib/libhc.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ../dependencies/rcf/bin/libRcfLib.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ../dependencies/boost/stage/lib/libboost_date_time-mgw46-mt-1_49.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ../dependencies/boost/stage/lib/libboost_filesystem-mgw46-mt-1_49.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ../dependencies/boost/stage/lib/libboost_program_options-mgw46-mt-1_49.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ../dependencies/boost/stage/lib/libboost_regex-mgw46-mt-1_49.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ../dependencies/boost/stage/lib/libboost_wserialization-mgw46-mt-1_49.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ../dependencies/boost/stage/lib/libboost_serialization-mgw46-mt-1_49.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ../dependencies/boost/stage/lib/libboost_signals-mgw46-mt-1_49.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ../dependencies/boost/stage/lib/libboost_thread-mgw46-mt-1_49.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ../dependencies/boost/stage/lib/libboost_system-mgw46-mt-1_49.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ../dependencies/zlib/libz.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ../dependencies/tbb/build/windows_ia32_gcc_mingw_release/tbb.dll

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ../dependencies/tbb/build/windows_ia32_gcc_mingw_release/tbbmalloc_proxy.dll

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ../dependencies/tbb/build/windows_ia32_gcc_mingw_release/tbbmalloc.dll

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv ${OBJECTFILES} ${LDLIBSOPTIONS} -s

${OBJECTDIR}/_ext/b4460d1a/activity_data_processing.o: ../common/activity_data_processing.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/b4460d1a
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DBOOST_ALL_NO_LIB -DBOOST_THREAD_USE_LIB -DNETBEANS_HACK -DQTMOC_HACK -DRCF_MULTI_THREADED -DRCF_NO_AUTO_INIT_DEINIT -DRCF_USE_BOOST_ASIO -DRCF_USE_BOOST_SERIALIZATION -DRCF_USE_BOOST_THREADS -DRCF_USE_ZLIB -DTBB_USE_DEBUG=0 -DWIN32_LEAN_AND_MEAN -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -I../common -I../dependencies/boost -I../dependencies/rcf/include -I../dependencies/zlib -I../dependencies/rdkit/Code -I../dependencies/tbb/include -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b4460d1a/activity_data_processing.o ../common/activity_data_processing.cpp

${OBJECTDIR}/_ext/b4460d1a/chemoper_selectors.o: ../common/chemoper_selectors.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/b4460d1a
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DBOOST_ALL_NO_LIB -DBOOST_THREAD_USE_LIB -DNETBEANS_HACK -DQTMOC_HACK -DRCF_MULTI_THREADED -DRCF_NO_AUTO_INIT_DEINIT -DRCF_USE_BOOST_ASIO -DRCF_USE_BOOST_SERIALIZATION -DRCF_USE_BOOST_THREADS -DRCF_USE_ZLIB -DTBB_USE_DEBUG=0 -DWIN32_LEAN_AND_MEAN -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -I../common -I../dependencies/boost -I../dependencies/rcf/include -I../dependencies/zlib -I../dependencies/rdkit/Code -I../dependencies/tbb/include -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b4460d1a/chemoper_selectors.o ../common/chemoper_selectors.cpp

${OBJECTDIR}/_ext/b4460d1a/fingerprint_selectors.o: ../common/fingerprint_selectors.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/b4460d1a
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DBOOST_ALL_NO_LIB -DBOOST_THREAD_USE_LIB -DNETBEANS_HACK -DQTMOC_HACK -DRCF_MULTI_THREADED -DRCF_NO_AUTO_INIT_DEINIT -DRCF_USE_BOOST_ASIO -DRCF_USE_BOOST_SERIALIZATION -DRCF_USE_BOOST_THREADS -DRCF_USE_ZLIB -DTBB_USE_DEBUG=0 -DWIN32_LEAN_AND_MEAN -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -I../common -I../dependencies/boost -I../dependencies/rcf/include -I../dependencies/zlib -I../dependencies/rdkit/Code -I../dependencies/tbb/include -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b4460d1a/fingerprint_selectors.o ../common/fingerprint_selectors.cpp

${OBJECTDIR}/_ext/b4460d1a/inout.o: ../common/inout.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/b4460d1a
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DBOOST_ALL_NO_LIB -DBOOST_THREAD_USE_LIB -DNETBEANS_HACK -DQTMOC_HACK -DRCF_MULTI_THREADED -DRCF_NO_AUTO_INIT_DEINIT -DRCF_USE_BOOST_ASIO -DRCF_USE_BOOST_SERIALIZATION -DRCF_USE_BOOST_THREADS -DRCF_USE_ZLIB -DTBB_USE_DEBUG=0 -DWIN32_LEAN_AND_MEAN -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -I../common -I../dependencies/boost -I../dependencies/rcf/include -I../dependencies/zlib -I../dependencies/rdkit/Code -I../dependencies/tbb/include -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b4460d1a/inout.o ../common/inout.cpp

${OBJECTDIR}/_ext/b4460d1a/iteration_serializer.o: ../common/iteration_serializer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/b4460d1a
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DBOOST_ALL_NO_LIB -DBOOST_THREAD_USE_LIB -DNETBEANS_HACK -DQTMOC_HACK -DRCF_MULTI_THREADED -DRCF_NO_AUTO_INIT_DEINIT -DRCF_USE_BOOST_ASIO -DRCF_USE_BOOST_SERIALIZATION -DRCF_USE_BOOST_THREADS -DRCF_USE_ZLIB -DTBB_USE_DEBUG=0 -DWIN32_LEAN_AND_MEAN -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -I../common -I../dependencies/boost -I../dependencies/rcf/include -I../dependencies/zlib -I../dependencies/rdkit/Code -I../dependencies/tbb/include -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b4460d1a/iteration_serializer.o ../common/iteration_serializer.cpp

${OBJECTDIR}/_ext/b4460d1a/simcoeff_selectors.o: ../common/simcoeff_selectors.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/b4460d1a
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DBOOST_ALL_NO_LIB -DBOOST_THREAD_USE_LIB -DNETBEANS_HACK -DQTMOC_HACK -DRCF_MULTI_THREADED -DRCF_NO_AUTO_INIT_DEINIT -DRCF_USE_BOOST_ASIO -DRCF_USE_BOOST_SERIALIZATION -DRCF_USE_BOOST_THREADS -DRCF_USE_ZLIB -DTBB_USE_DEBUG=0 -DWIN32_LEAN_AND_MEAN -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -I../common -I../dependencies/boost -I../dependencies/rcf/include -I../dependencies/zlib -I../dependencies/rdkit/Code -I../dependencies/tbb/include -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b4460d1a/simcoeff_selectors.o ../common/simcoeff_selectors.cpp

${OBJECTDIR}/auxiliary/SynchRand.o: auxiliary/SynchRand.cpp 
	${MKDIR} -p ${OBJECTDIR}/auxiliary
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DBOOST_ALL_NO_LIB -DBOOST_THREAD_USE_LIB -DNETBEANS_HACK -DQTMOC_HACK -DRCF_MULTI_THREADED -DRCF_NO_AUTO_INIT_DEINIT -DRCF_USE_BOOST_ASIO -DRCF_USE_BOOST_SERIALIZATION -DRCF_USE_BOOST_THREADS -DRCF_USE_ZLIB -DTBB_USE_DEBUG=0 -DWIN32_LEAN_AND_MEAN -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -I../common -I../dependencies/boost -I../dependencies/rcf/include -I../dependencies/zlib -I../dependencies/rdkit/Code -I../dependencies/tbb/include -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/auxiliary/SynchRand.o auxiliary/SynchRand.cpp

${OBJECTDIR}/chem/ChemicalAuxiliary.o: chem/ChemicalAuxiliary.cpp 
	${MKDIR} -p ${OBJECTDIR}/chem
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DBOOST_ALL_NO_LIB -DBOOST_THREAD_USE_LIB -DNETBEANS_HACK -DQTMOC_HACK -DRCF_MULTI_THREADED -DRCF_NO_AUTO_INIT_DEINIT -DRCF_USE_BOOST_ASIO -DRCF_USE_BOOST_SERIALIZATION -DRCF_USE_BOOST_THREADS -DRCF_USE_ZLIB -DTBB_USE_DEBUG=0 -DWIN32_LEAN_AND_MEAN -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -I../common -I../dependencies/boost -I../dependencies/rcf/include -I../dependencies/zlib -I../dependencies/rdkit/Code -I../dependencies/tbb/include -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/chem/ChemicalAuxiliary.o chem/ChemicalAuxiliary.cpp

${OBJECTDIR}/chem/SimCoefCalculator.o: chem/SimCoefCalculator.cpp 
	${MKDIR} -p ${OBJECTDIR}/chem
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DBOOST_ALL_NO_LIB -DBOOST_THREAD_USE_LIB -DNETBEANS_HACK -DQTMOC_HACK -DRCF_MULTI_THREADED -DRCF_NO_AUTO_INIT_DEINIT -DRCF_USE_BOOST_ASIO -DRCF_USE_BOOST_SERIALIZATION -DRCF_USE_BOOST_THREADS -DRCF_USE_ZLIB -DTBB_USE_DEBUG=0 -DWIN32_LEAN_AND_MEAN -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -I../common -I../dependencies/boost -I../dependencies/rcf/include -I../dependencies/zlib -I../dependencies/rdkit/Code -I../dependencies/tbb/include -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/chem/SimCoefCalculator.o chem/SimCoefCalculator.cpp

${OBJECTDIR}/chem/fingerprintStrategy/AtomPairsFngpr.o: chem/fingerprintStrategy/AtomPairsFngpr.cpp 
	${MKDIR} -p ${OBJECTDIR}/chem/fingerprintStrategy
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DBOOST_ALL_NO_LIB -DBOOST_THREAD_USE_LIB -DNETBEANS_HACK -DQTMOC_HACK -DRCF_MULTI_THREADED -DRCF_NO_AUTO_INIT_DEINIT -DRCF_USE_BOOST_ASIO -DRCF_USE_BOOST_SERIALIZATION -DRCF_USE_BOOST_THREADS -DRCF_USE_ZLIB -DTBB_USE_DEBUG=0 -DWIN32_LEAN_AND_MEAN -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -I../common -I../dependencies/boost -I../dependencies/rcf/include -I../dependencies/zlib -I../dependencies/rdkit/Code -I../dependencies/tbb/include -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/chem/fingerprintStrategy/AtomPairsFngpr.o chem/fingerprintStrategy/AtomPairsFngpr.cpp

${OBJECTDIR}/chem/fingerprintStrategy/FingerprintStrategy.o: chem/fingerprintStrategy/FingerprintStrategy.cpp 
	${MKDIR} -p ${OBJECTDIR}/chem/fingerprintStrategy
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DBOOST_ALL_NO_LIB -DBOOST_THREAD_USE_LIB -DNETBEANS_HACK -DQTMOC_HACK -DRCF_MULTI_THREADED -DRCF_NO_AUTO_INIT_DEINIT -DRCF_USE_BOOST_ASIO -DRCF_USE_BOOST_SERIALIZATION -DRCF_USE_BOOST_THREADS -DRCF_USE_ZLIB -DTBB_USE_DEBUG=0 -DWIN32_LEAN_AND_MEAN -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -I../common -I../dependencies/boost -I../dependencies/rcf/include -I../dependencies/zlib -I../dependencies/rdkit/Code -I../dependencies/tbb/include -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/chem/fingerprintStrategy/FingerprintStrategy.o chem/fingerprintStrategy/FingerprintStrategy.cpp

${OBJECTDIR}/chem/fingerprintStrategy/MorganFngpr.o: chem/fingerprintStrategy/MorganFngpr.cpp 
	${MKDIR} -p ${OBJECTDIR}/chem/fingerprintStrategy
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DBOOST_ALL_NO_LIB -DBOOST_THREAD_USE_LIB -DNETBEANS_HACK -DQTMOC_HACK -DRCF_MULTI_THREADED -DRCF_NO_AUTO_INIT_DEINIT -DRCF_USE_BOOST_ASIO -DRCF_USE_BOOST_SERIALIZATION -DRCF_USE_BOOST_THREADS -DRCF_USE_ZLIB -DTBB_USE_DEBUG=0 -DWIN32_LEAN_AND_MEAN -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -I../common -I../dependencies/boost -I../dependencies/rcf/include -I../dependencies/zlib -I../dependencies/rdkit/Code -I../dependencies/tbb/include -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/chem/fingerprintStrategy/MorganFngpr.o chem/fingerprintStrategy/MorganFngpr.cpp

${OBJECTDIR}/chem/fingerprintStrategy/TopolTorsFngpr.o: chem/fingerprintStrategy/TopolTorsFngpr.cpp 
	${MKDIR} -p ${OBJECTDIR}/chem/fingerprintStrategy
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DBOOST_ALL_NO_LIB -DBOOST_THREAD_USE_LIB -DNETBEANS_HACK -DQTMOC_HACK -DRCF_MULTI_THREADED -DRCF_NO_AUTO_INIT_DEINIT -DRCF_USE_BOOST_ASIO -DRCF_USE_BOOST_SERIALIZATION -DRCF_USE_BOOST_THREADS -DRCF_USE_ZLIB -DTBB_USE_DEBUG=0 -DWIN32_LEAN_AND_MEAN -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -I../common -I../dependencies/boost -I../dependencies/rcf/include -I../dependencies/zlib -I../dependencies/rdkit/Code -I../dependencies/tbb/include -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/chem/fingerprintStrategy/TopolTorsFngpr.o chem/fingerprintStrategy/TopolTorsFngpr.cpp

${OBJECTDIR}/chem/morphing/CalculateDistances.o: chem/morphing/CalculateDistances.cpp 
	${MKDIR} -p ${OBJECTDIR}/chem/morphing
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DBOOST_ALL_NO_LIB -DBOOST_THREAD_USE_LIB -DNETBEANS_HACK -DQTMOC_HACK -DRCF_MULTI_THREADED -DRCF_NO_AUTO_INIT_DEINIT -DRCF_USE_BOOST_ASIO -DRCF_USE_BOOST_SERIALIZATION -DRCF_USE_BOOST_THREADS -DRCF_USE_ZLIB -DTBB_USE_DEBUG=0 -DWIN32_LEAN_AND_MEAN -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -I../common -I../dependencies/boost -I../dependencies/rcf/include -I../dependencies/zlib -I../dependencies/rdkit/Code -I../dependencies/tbb/include -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/chem/morphing/CalculateDistances.o chem/morphing/CalculateDistances.cpp

${OBJECTDIR}/chem/morphing/FilterScript.o: chem/morphing/FilterScript.cpp 
	${MKDIR} -p ${OBJECTDIR}/chem/morphing
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DBOOST_ALL_NO_LIB -DBOOST_THREAD_USE_LIB -DNETBEANS_HACK -DQTMOC_HACK -DRCF_MULTI_THREADED -DRCF_NO_AUTO_INIT_DEINIT -DRCF_USE_BOOST_ASIO -DRCF_USE_BOOST_SERIALIZATION -DRCF_USE_BOOST_THREADS -DRCF_USE_ZLIB -DTBB_USE_DEBUG=0 -DWIN32_LEAN_AND_MEAN -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -I../common -I../dependencies/boost -I../dependencies/rcf/include -I../dependencies/zlib -I../dependencies/rdkit/Code -I../dependencies/tbb/include -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/chem/morphing/FilterScript.o chem/morphing/FilterScript.cpp

${OBJECTDIR}/chem/morphing/Morphing.o: chem/morphing/Morphing.cpp 
	${MKDIR} -p ${OBJECTDIR}/chem/morphing
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DBOOST_ALL_NO_LIB -DBOOST_THREAD_USE_LIB -DNETBEANS_HACK -DQTMOC_HACK -DRCF_MULTI_THREADED -DRCF_NO_AUTO_INIT_DEINIT -DRCF_USE_BOOST_ASIO -DRCF_USE_BOOST_SERIALIZATION -DRCF_USE_BOOST_THREADS -DRCF_USE_ZLIB -DTBB_USE_DEBUG=0 -DWIN32_LEAN_AND_MEAN -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -I../common -I../dependencies/boost -I../dependencies/rcf/include -I../dependencies/zlib -I../dependencies/rdkit/Code -I../dependencies/tbb/include -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/chem/morphing/Morphing.o chem/morphing/Morphing.cpp

${OBJECTDIR}/chem/morphing/MorphingData.o: chem/morphing/MorphingData.cpp 
	${MKDIR} -p ${OBJECTDIR}/chem/morphing
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DBOOST_ALL_NO_LIB -DBOOST_THREAD_USE_LIB -DNETBEANS_HACK -DQTMOC_HACK -DRCF_MULTI_THREADED -DRCF_NO_AUTO_INIT_DEINIT -DRCF_USE_BOOST_ASIO -DRCF_USE_BOOST_SERIALIZATION -DRCF_USE_BOOST_THREADS -DRCF_USE_ZLIB -DTBB_USE_DEBUG=0 -DWIN32_LEAN_AND_MEAN -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -I../common -I../dependencies/boost -I../dependencies/rcf/include -I../dependencies/zlib -I../dependencies/rdkit/Code -I../dependencies/tbb/include -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/chem/morphing/MorphingData.o chem/morphing/MorphingData.cpp

${OBJECTDIR}/chem/morphing/MorphingFtors.o: chem/morphing/MorphingFtors.cpp 
	${MKDIR} -p ${OBJECTDIR}/chem/morphing
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DBOOST_ALL_NO_LIB -DBOOST_THREAD_USE_LIB -DNETBEANS_HACK -DQTMOC_HACK -DRCF_MULTI_THREADED -DRCF_NO_AUTO_INIT_DEINIT -DRCF_USE_BOOST_ASIO -DRCF_USE_BOOST_SERIALIZATION -DRCF_USE_BOOST_THREADS -DRCF_USE_ZLIB -DTBB_USE_DEBUG=0 -DWIN32_LEAN_AND_MEAN -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -I../common -I../dependencies/boost -I../dependencies/rcf/include -I../dependencies/zlib -I../dependencies/rdkit/Code -I../dependencies/tbb/include -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/chem/morphing/MorphingFtors.o chem/morphing/MorphingFtors.cpp

${OBJECTDIR}/chem/morphingStrategy/OpAddAtom.o: chem/morphingStrategy/OpAddAtom.cpp 
	${MKDIR} -p ${OBJECTDIR}/chem/morphingStrategy
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DBOOST_ALL_NO_LIB -DBOOST_THREAD_USE_LIB -DNETBEANS_HACK -DQTMOC_HACK -DRCF_MULTI_THREADED -DRCF_NO_AUTO_INIT_DEINIT -DRCF_USE_BOOST_ASIO -DRCF_USE_BOOST_SERIALIZATION -DRCF_USE_BOOST_THREADS -DRCF_USE_ZLIB -DTBB_USE_DEBUG=0 -DWIN32_LEAN_AND_MEAN -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -I../common -I../dependencies/boost -I../dependencies/rcf/include -I../dependencies/zlib -I../dependencies/rdkit/Code -I../dependencies/tbb/include -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/chem/morphingStrategy/OpAddAtom.o chem/morphingStrategy/OpAddAtom.cpp

${OBJECTDIR}/chem/morphingStrategy/OpAddBond.o: chem/morphingStrategy/OpAddBond.cpp 
	${MKDIR} -p ${OBJECTDIR}/chem/morphingStrategy
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DBOOST_ALL_NO_LIB -DBOOST_THREAD_USE_LIB -DNETBEANS_HACK -DQTMOC_HACK -DRCF_MULTI_THREADED -DRCF_NO_AUTO_INIT_DEINIT -DRCF_USE_BOOST_ASIO -DRCF_USE_BOOST_SERIALIZATION -DRCF_USE_BOOST_THREADS -DRCF_USE_ZLIB -DTBB_USE_DEBUG=0 -DWIN32_LEAN_AND_MEAN -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -I../common -I../dependencies/boost -I../dependencies/rcf/include -I../dependencies/zlib -I../dependencies/rdkit/Code -I../dependencies/tbb/include -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/chem/morphingStrategy/OpAddBond.o chem/morphingStrategy/OpAddBond.cpp

${OBJECTDIR}/chem/morphingStrategy/OpBondContraction.o: chem/morphingStrategy/OpBondContraction.cpp 
	${MKDIR} -p ${OBJECTDIR}/chem/morphingStrategy
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DBOOST_ALL_NO_LIB -DBOOST_THREAD_USE_LIB -DNETBEANS_HACK -DQTMOC_HACK -DRCF_MULTI_THREADED -DRCF_NO_AUTO_INIT_DEINIT -DRCF_USE_BOOST_ASIO -DRCF_USE_BOOST_SERIALIZATION -DRCF_USE_BOOST_THREADS -DRCF_USE_ZLIB -DTBB_USE_DEBUG=0 -DWIN32_LEAN_AND_MEAN -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -I../common -I../dependencies/boost -I../dependencies/rcf/include -I../dependencies/zlib -I../dependencies/rdkit/Code -I../dependencies/tbb/include -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/chem/morphingStrategy/OpBondContraction.o chem/morphingStrategy/OpBondContraction.cpp

${OBJECTDIR}/chem/morphingStrategy/OpBondReroute.o: chem/morphingStrategy/OpBondReroute.cpp 
	${MKDIR} -p ${OBJECTDIR}/chem/morphingStrategy
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DBOOST_ALL_NO_LIB -DBOOST_THREAD_USE_LIB -DNETBEANS_HACK -DQTMOC_HACK -DRCF_MULTI_THREADED -DRCF_NO_AUTO_INIT_DEINIT -DRCF_USE_BOOST_ASIO -DRCF_USE_BOOST_SERIALIZATION -DRCF_USE_BOOST_THREADS -DRCF_USE_ZLIB -DTBB_USE_DEBUG=0 -DWIN32_LEAN_AND_MEAN -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -I../common -I../dependencies/boost -I../dependencies/rcf/include -I../dependencies/zlib -I../dependencies/rdkit/Code -I../dependencies/tbb/include -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/chem/morphingStrategy/OpBondReroute.o chem/morphingStrategy/OpBondReroute.cpp

${OBJECTDIR}/chem/morphingStrategy/OpInterlayAtom.o: chem/morphingStrategy/OpInterlayAtom.cpp 
	${MKDIR} -p ${OBJECTDIR}/chem/morphingStrategy
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DBOOST_ALL_NO_LIB -DBOOST_THREAD_USE_LIB -DNETBEANS_HACK -DQTMOC_HACK -DRCF_MULTI_THREADED -DRCF_NO_AUTO_INIT_DEINIT -DRCF_USE_BOOST_ASIO -DRCF_USE_BOOST_SERIALIZATION -DRCF_USE_BOOST_THREADS -DRCF_USE_ZLIB -DTBB_USE_DEBUG=0 -DWIN32_LEAN_AND_MEAN -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -I../common -I../dependencies/boost -I../dependencies/rcf/include -I../dependencies/zlib -I../dependencies/rdkit/Code -I../dependencies/tbb/include -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/chem/morphingStrategy/OpInterlayAtom.o chem/morphingStrategy/OpInterlayAtom.cpp

${OBJECTDIR}/chem/morphingStrategy/OpMutateAtom.o: chem/morphingStrategy/OpMutateAtom.cpp 
	${MKDIR} -p ${OBJECTDIR}/chem/morphingStrategy
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DBOOST_ALL_NO_LIB -DBOOST_THREAD_USE_LIB -DNETBEANS_HACK -DQTMOC_HACK -DRCF_MULTI_THREADED -DRCF_NO_AUTO_INIT_DEINIT -DRCF_USE_BOOST_ASIO -DRCF_USE_BOOST_SERIALIZATION -DRCF_USE_BOOST_THREADS -DRCF_USE_ZLIB -DTBB_USE_DEBUG=0 -DWIN32_LEAN_AND_MEAN -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -I../common -I../dependencies/boost -I../dependencies/rcf/include -I../dependencies/zlib -I../dependencies/rdkit/Code -I../dependencies/tbb/include -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/chem/morphingStrategy/OpMutateAtom.o chem/morphingStrategy/OpMutateAtom.cpp

${OBJECTDIR}/chem/morphingStrategy/OpRemoveAtom.o: chem/morphingStrategy/OpRemoveAtom.cpp 
	${MKDIR} -p ${OBJECTDIR}/chem/morphingStrategy
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DBOOST_ALL_NO_LIB -DBOOST_THREAD_USE_LIB -DNETBEANS_HACK -DQTMOC_HACK -DRCF_MULTI_THREADED -DRCF_NO_AUTO_INIT_DEINIT -DRCF_USE_BOOST_ASIO -DRCF_USE_BOOST_SERIALIZATION -DRCF_USE_BOOST_THREADS -DRCF_USE_ZLIB -DTBB_USE_DEBUG=0 -DWIN32_LEAN_AND_MEAN -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -I../common -I../dependencies/boost -I../dependencies/rcf/include -I../dependencies/zlib -I../dependencies/rdkit/Code -I../dependencies/tbb/include -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/chem/morphingStrategy/OpRemoveAtom.o chem/morphingStrategy/OpRemoveAtom.cpp

${OBJECTDIR}/chem/morphingStrategy/OpRemoveBond.o: chem/morphingStrategy/OpRemoveBond.cpp 
	${MKDIR} -p ${OBJECTDIR}/chem/morphingStrategy
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DBOOST_ALL_NO_LIB -DBOOST_THREAD_USE_LIB -DNETBEANS_HACK -DQTMOC_HACK -DRCF_MULTI_THREADED -DRCF_NO_AUTO_INIT_DEINIT -DRCF_USE_BOOST_ASIO -DRCF_USE_BOOST_SERIALIZATION -DRCF_USE_BOOST_THREADS -DRCF_USE_ZLIB -DTBB_USE_DEBUG=0 -DWIN32_LEAN_AND_MEAN -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -I../common -I../dependencies/boost -I../dependencies/rcf/include -I../dependencies/zlib -I../dependencies/rdkit/Code -I../dependencies/tbb/include -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/chem/morphingStrategy/OpRemoveBond.o chem/morphingStrategy/OpRemoveBond.cpp

${OBJECTDIR}/chem/simCoefStrategy/TanimotoSimCoef.o: chem/simCoefStrategy/TanimotoSimCoef.cpp 
	${MKDIR} -p ${OBJECTDIR}/chem/simCoefStrategy
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DBOOST_ALL_NO_LIB -DBOOST_THREAD_USE_LIB -DNETBEANS_HACK -DQTMOC_HACK -DRCF_MULTI_THREADED -DRCF_NO_AUTO_INIT_DEINIT -DRCF_USE_BOOST_ASIO -DRCF_USE_BOOST_SERIALIZATION -DRCF_USE_BOOST_THREADS -DRCF_USE_ZLIB -DTBB_USE_DEBUG=0 -DWIN32_LEAN_AND_MEAN -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -I../common -I../dependencies/boost -I../dependencies/rcf/include -I../dependencies/zlib -I../dependencies/rdkit/Code -I../dependencies/tbb/include -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/chem/simCoefStrategy/TanimotoSimCoef.o chem/simCoefStrategy/TanimotoSimCoef.cpp

${OBJECTDIR}/core/AcceptMorphs.o: core/AcceptMorphs.cpp 
	${MKDIR} -p ${OBJECTDIR}/core
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DBOOST_ALL_NO_LIB -DBOOST_THREAD_USE_LIB -DNETBEANS_HACK -DQTMOC_HACK -DRCF_MULTI_THREADED -DRCF_NO_AUTO_INIT_DEINIT -DRCF_USE_BOOST_ASIO -DRCF_USE_BOOST_SERIALIZATION -DRCF_USE_BOOST_THREADS -DRCF_USE_ZLIB -DTBB_USE_DEBUG=0 -DWIN32_LEAN_AND_MEAN -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -I../common -I../dependencies/boost -I../dependencies/rcf/include -I../dependencies/zlib -I../dependencies/rdkit/Code -I../dependencies/tbb/include -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/core/AcceptMorphs.o core/AcceptMorphs.cpp

${OBJECTDIR}/core/CollectMorphs.o: core/CollectMorphs.cpp 
	${MKDIR} -p ${OBJECTDIR}/core
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DBOOST_ALL_NO_LIB -DBOOST_THREAD_USE_LIB -DNETBEANS_HACK -DQTMOC_HACK -DRCF_MULTI_THREADED -DRCF_NO_AUTO_INIT_DEINIT -DRCF_USE_BOOST_ASIO -DRCF_USE_BOOST_SERIALIZATION -DRCF_USE_BOOST_THREADS -DRCF_USE_ZLIB -DTBB_USE_DEBUG=0 -DWIN32_LEAN_AND_MEAN -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -I../common -I../dependencies/boost -I../dependencies/rcf/include -I../dependencies/zlib -I../dependencies/rdkit/Code -I../dependencies/tbb/include -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/core/CollectMorphs.o core/CollectMorphs.cpp

${OBJECTDIR}/core/CompareMorphs.o: core/CompareMorphs.cpp 
	${MKDIR} -p ${OBJECTDIR}/core
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DBOOST_ALL_NO_LIB -DBOOST_THREAD_USE_LIB -DNETBEANS_HACK -DQTMOC_HACK -DRCF_MULTI_THREADED -DRCF_NO_AUTO_INIT_DEINIT -DRCF_USE_BOOST_ASIO -DRCF_USE_BOOST_SERIALIZATION -DRCF_USE_BOOST_THREADS -DRCF_USE_ZLIB -DTBB_USE_DEBUG=0 -DWIN32_LEAN_AND_MEAN -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -I../common -I../dependencies/boost -I../dependencies/rcf/include -I../dependencies/zlib -I../dependencies/rdkit/Code -I../dependencies/tbb/include -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/core/CompareMorphs.o core/CompareMorphs.cpp

${OBJECTDIR}/core/FilterMorphs.o: core/FilterMorphs.cpp 
	${MKDIR} -p ${OBJECTDIR}/core
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DBOOST_ALL_NO_LIB -DBOOST_THREAD_USE_LIB -DNETBEANS_HACK -DQTMOC_HACK -DRCF_MULTI_THREADED -DRCF_NO_AUTO_INIT_DEINIT -DRCF_USE_BOOST_ASIO -DRCF_USE_BOOST_SERIALIZATION -DRCF_USE_BOOST_THREADS -DRCF_USE_ZLIB -DTBB_USE_DEBUG=0 -DWIN32_LEAN_AND_MEAN -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -I../common -I../dependencies/boost -I../dependencies/rcf/include -I../dependencies/zlib -I../dependencies/rdkit/Code -I../dependencies/tbb/include -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/core/FilterMorphs.o core/FilterMorphs.cpp

${OBJECTDIR}/core/FindLeaves.o: core/FindLeaves.cpp 
	${MKDIR} -p ${OBJECTDIR}/core
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DBOOST_ALL_NO_LIB -DBOOST_THREAD_USE_LIB -DNETBEANS_HACK -DQTMOC_HACK -DRCF_MULTI_THREADED -DRCF_NO_AUTO_INIT_DEINIT -DRCF_USE_BOOST_ASIO -DRCF_USE_BOOST_SERIALIZATION -DRCF_USE_BOOST_THREADS -DRCF_USE_ZLIB -DTBB_USE_DEBUG=0 -DWIN32_LEAN_AND_MEAN -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -I../common -I../dependencies/boost -I../dependencies/rcf/include -I../dependencies/zlib -I../dependencies/rdkit/Code -I../dependencies/tbb/include -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/core/FindLeaves.o core/FindLeaves.cpp

${OBJECTDIR}/core/PathFinder.o: core/PathFinder.cpp 
	${MKDIR} -p ${OBJECTDIR}/core
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DBOOST_ALL_NO_LIB -DBOOST_THREAD_USE_LIB -DNETBEANS_HACK -DQTMOC_HACK -DRCF_MULTI_THREADED -DRCF_NO_AUTO_INIT_DEINIT -DRCF_USE_BOOST_ASIO -DRCF_USE_BOOST_SERIALIZATION -DRCF_USE_BOOST_THREADS -DRCF_USE_ZLIB -DTBB_USE_DEBUG=0 -DWIN32_LEAN_AND_MEAN -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -I../common -I../dependencies/boost -I../dependencies/rcf/include -I../dependencies/zlib -I../dependencies/rdkit/Code -I../dependencies/tbb/include -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/core/PathFinder.o core/PathFinder.cpp

${OBJECTDIR}/core/PathFinderActivity.o: core/PathFinderActivity.cpp 
	${MKDIR} -p ${OBJECTDIR}/core
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DBOOST_ALL_NO_LIB -DBOOST_THREAD_USE_LIB -DNETBEANS_HACK -DQTMOC_HACK -DRCF_MULTI_THREADED -DRCF_NO_AUTO_INIT_DEINIT -DRCF_USE_BOOST_ASIO -DRCF_USE_BOOST_SERIALIZATION -DRCF_USE_BOOST_THREADS -DRCF_USE_ZLIB -DTBB_USE_DEBUG=0 -DWIN32_LEAN_AND_MEAN -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -I../common -I../dependencies/boost -I../dependencies/rcf/include -I../dependencies/zlib -I../dependencies/rdkit/Code -I../dependencies/tbb/include -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/core/PathFinderActivity.o core/PathFinderActivity.cpp

${OBJECTDIR}/core/PruneTree.o: core/PruneTree.cpp 
	${MKDIR} -p ${OBJECTDIR}/core
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DBOOST_ALL_NO_LIB -DBOOST_THREAD_USE_LIB -DNETBEANS_HACK -DQTMOC_HACK -DRCF_MULTI_THREADED -DRCF_NO_AUTO_INIT_DEINIT -DRCF_USE_BOOST_ASIO -DRCF_USE_BOOST_SERIALIZATION -DRCF_USE_BOOST_THREADS -DRCF_USE_ZLIB -DTBB_USE_DEBUG=0 -DWIN32_LEAN_AND_MEAN -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -I../common -I../dependencies/boost -I../dependencies/rcf/include -I../dependencies/zlib -I../dependencies/rdkit/Code -I../dependencies/tbb/include -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/core/PruneTree.o core/PruneTree.cpp

${OBJECTDIR}/core/UpdateTree.o: core/UpdateTree.cpp 
	${MKDIR} -p ${OBJECTDIR}/core
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DBOOST_ALL_NO_LIB -DBOOST_THREAD_USE_LIB -DNETBEANS_HACK -DQTMOC_HACK -DRCF_MULTI_THREADED -DRCF_NO_AUTO_INIT_DEINIT -DRCF_USE_BOOST_ASIO -DRCF_USE_BOOST_SERIALIZATION -DRCF_USE_BOOST_THREADS -DRCF_USE_ZLIB -DTBB_USE_DEBUG=0 -DWIN32_LEAN_AND_MEAN -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -I../common -I../dependencies/boost -I../dependencies/rcf/include -I../dependencies/zlib -I../dependencies/rdkit/Code -I../dependencies/tbb/include -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/core/UpdateTree.o core/UpdateTree.cpp

${OBJECTDIR}/csv.o: csv.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DBOOST_ALL_NO_LIB -DBOOST_THREAD_USE_LIB -DNETBEANS_HACK -DQTMOC_HACK -DRCF_MULTI_THREADED -DRCF_NO_AUTO_INIT_DEINIT -DRCF_USE_BOOST_ASIO -DRCF_USE_BOOST_SERIALIZATION -DRCF_USE_BOOST_THREADS -DRCF_USE_ZLIB -DTBB_USE_DEBUG=0 -DWIN32_LEAN_AND_MEAN -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -I../common -I../dependencies/boost -I../dependencies/rcf/include -I../dependencies/zlib -I../dependencies/rdkit/Code -I../dependencies/tbb/include -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/csv.o csv.cpp

${OBJECTDIR}/descriptor/BioChemLibDescriptors.o: descriptor/BioChemLibDescriptors.cpp 
	${MKDIR} -p ${OBJECTDIR}/descriptor
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DBOOST_ALL_NO_LIB -DBOOST_THREAD_USE_LIB -DNETBEANS_HACK -DQTMOC_HACK -DRCF_MULTI_THREADED -DRCF_NO_AUTO_INIT_DEINIT -DRCF_USE_BOOST_ASIO -DRCF_USE_BOOST_SERIALIZATION -DRCF_USE_BOOST_THREADS -DRCF_USE_ZLIB -DTBB_USE_DEBUG=0 -DWIN32_LEAN_AND_MEAN -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -I../common -I../dependencies/boost -I../dependencies/rcf/include -I../dependencies/zlib -I../dependencies/rdkit/Code -I../dependencies/tbb/include -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/descriptor/BioChemLibDescriptors.o descriptor/BioChemLibDescriptors.cpp

${OBJECTDIR}/descriptor/CSV.o: descriptor/CSV.cpp 
	${MKDIR} -p ${OBJECTDIR}/descriptor
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DBOOST_ALL_NO_LIB -DBOOST_THREAD_USE_LIB -DNETBEANS_HACK -DQTMOC_HACK -DRCF_MULTI_THREADED -DRCF_NO_AUTO_INIT_DEINIT -DRCF_USE_BOOST_ASIO -DRCF_USE_BOOST_SERIALIZATION -DRCF_USE_BOOST_THREADS -DRCF_USE_ZLIB -DTBB_USE_DEBUG=0 -DWIN32_LEAN_AND_MEAN -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -I../common -I../dependencies/boost -I../dependencies/rcf/include -I../dependencies/zlib -I../dependencies/rdkit/Code -I../dependencies/tbb/include -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/descriptor/CSV.o descriptor/CSV.cpp

${OBJECTDIR}/descriptor/DataConverter.o: descriptor/DataConverter.cpp 
	${MKDIR} -p ${OBJECTDIR}/descriptor
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DBOOST_ALL_NO_LIB -DBOOST_THREAD_USE_LIB -DNETBEANS_HACK -DQTMOC_HACK -DRCF_MULTI_THREADED -DRCF_NO_AUTO_INIT_DEINIT -DRCF_USE_BOOST_ASIO -DRCF_USE_BOOST_SERIALIZATION -DRCF_USE_BOOST_THREADS -DRCF_USE_ZLIB -DTBB_USE_DEBUG=0 -DWIN32_LEAN_AND_MEAN -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -I../common -I../dependencies/boost -I../dependencies/rcf/include -I../dependencies/zlib -I../dependencies/rdkit/Code -I../dependencies/tbb/include -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/descriptor/DataConverter.o descriptor/DataConverter.cpp

${OBJECTDIR}/descriptor/DescriptorSource.o: descriptor/DescriptorSource.cpp 
	${MKDIR} -p ${OBJECTDIR}/descriptor
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DBOOST_ALL_NO_LIB -DBOOST_THREAD_USE_LIB -DNETBEANS_HACK -DQTMOC_HACK -DRCF_MULTI_THREADED -DRCF_NO_AUTO_INIT_DEINIT -DRCF_USE_BOOST_ASIO -DRCF_USE_BOOST_SERIALIZATION -DRCF_USE_BOOST_THREADS -DRCF_USE_ZLIB -DTBB_USE_DEBUG=0 -DWIN32_LEAN_AND_MEAN -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -I../common -I../dependencies/boost -I../dependencies/rcf/include -I../dependencies/zlib -I../dependencies/rdkit/Code -I../dependencies/tbb/include -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/descriptor/DescriptorSource.o descriptor/DescriptorSource.cpp

${OBJECTDIR}/descriptor/PaDELDescriptorCalculator.o: descriptor/PaDELDescriptorCalculator.cpp 
	${MKDIR} -p ${OBJECTDIR}/descriptor
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DBOOST_ALL_NO_LIB -DBOOST_THREAD_USE_LIB -DNETBEANS_HACK -DQTMOC_HACK -DRCF_MULTI_THREADED -DRCF_NO_AUTO_INIT_DEINIT -DRCF_USE_BOOST_ASIO -DRCF_USE_BOOST_SERIALIZATION -DRCF_USE_BOOST_THREADS -DRCF_USE_ZLIB -DTBB_USE_DEBUG=0 -DWIN32_LEAN_AND_MEAN -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -I../common -I../dependencies/boost -I../dependencies/rcf/include -I../dependencies/zlib -I../dependencies/rdkit/Code -I../dependencies/tbb/include -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/descriptor/PaDELDescriptorCalculator.o descriptor/PaDELDescriptorCalculator.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DBOOST_ALL_NO_LIB -DBOOST_THREAD_USE_LIB -DNETBEANS_HACK -DQTMOC_HACK -DRCF_MULTI_THREADED -DRCF_NO_AUTO_INIT_DEINIT -DRCF_USE_BOOST_ASIO -DRCF_USE_BOOST_SERIALIZATION -DRCF_USE_BOOST_THREADS -DRCF_USE_ZLIB -DTBB_USE_DEBUG=0 -DWIN32_LEAN_AND_MEAN -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -I../common -I../dependencies/boost -I../dependencies/rcf/include -I../dependencies/zlib -I../dependencies/rdkit/Code -I../dependencies/tbb/include -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/molpher_molecule.o: molpher_molecule.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DBOOST_ALL_NO_LIB -DBOOST_THREAD_USE_LIB -DNETBEANS_HACK -DQTMOC_HACK -DRCF_MULTI_THREADED -DRCF_NO_AUTO_INIT_DEINIT -DRCF_USE_BOOST_ASIO -DRCF_USE_BOOST_SERIALIZATION -DRCF_USE_BOOST_THREADS -DRCF_USE_ZLIB -DTBB_USE_DEBUG=0 -DWIN32_LEAN_AND_MEAN -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -I../common -I../dependencies/boost -I../dependencies/rcf/include -I../dependencies/zlib -I../dependencies/rdkit/Code -I../dependencies/tbb/include -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/molpher_molecule.o molpher_molecule.cpp

${OBJECTDIR}/script_execution.o: script_execution.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -s -DBOOST_ALL_NO_LIB -DBOOST_THREAD_USE_LIB -DNETBEANS_HACK -DQTMOC_HACK -DRCF_MULTI_THREADED -DRCF_NO_AUTO_INIT_DEINIT -DRCF_USE_BOOST_ASIO -DRCF_USE_BOOST_SERIALIZATION -DRCF_USE_BOOST_THREADS -DRCF_USE_ZLIB -DTBB_USE_DEBUG=0 -DWIN32_LEAN_AND_MEAN -DWINVER=0x0501 -D_WIN32_WINNT=0x0501 -I../common -I../dependencies/boost -I../dependencies/rcf/include -I../dependencies/zlib -I../dependencies/rdkit/Code -I../dependencies/tbb/include -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/script_execution.o script_execution.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/molpher-srv.exe

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
