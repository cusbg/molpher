#!/bin/sh

#### Path to used tools. ###
CMAKE_PATH="MINGW STYLE PATH TO CMAKE DIRECTORY WITH BIN SUBDIRECTORY"
MINGW_PATH="MINGW STYLE PATH TO MINGW DIRECTORY WITH BIN SUBDIRECTORY"

###

# Dependency list.
link_TBB=https://www.threadingbuildingblocks.org/sites/default/files/software_releases/source/tbb44_20160526oss_src_0.tgz

link_BOOST=http://downloads.sourceforge.net/project/boost/boost/1.61.0/boost_1_61_0.zip

link_ZLIB=http://zlib.net/zlib128.zip

# https://github.com/rdkit/rdkit/archive/Release_2016_03_3.zip
# We need to use older version as newer version require xlocale.h which is not presented in MinGW.
# See http://comments.gmane.org/gmane.science.chemistry.rdkit.user/4222

# --> Release_2015_09_2.zip
#Scanning dependencies of target SubstructMatch
#[ 24%] Building CXX object Code/GraphMol/Substruct/CMakeFiles/SubstructMatch.dir/SubstructMatch.cpp.obj
#[ 24%] Building CXX object Code/GraphMol/Substruct/CMakeFiles/SubstructMatch.dir/SubstructUtils.cpp.obj
#[ 24%] Linking CXX shared library ../../../bin/libSubstructMatch.dll
#D:/Projects/Molpher/dependencies/boost/stage/lib/libboost_thread-mgw61-mt-1_61.a(thread.o):thread.cpp:(.text+0x16c0): multiple definition of `boost::thread::start_thread_noexcept()'
#../../../lib/libGraphMol.dll.a(d000981.o):(.text+0x0): first defined here
#D:/Projects/Molpher/dependencies/boost/stage/lib/libboost_thread-mgw61-mt-1_61.a(thread.o):thread.cpp:(.text+0x1800): multiple definition of `boost::thread::get_id() const'
#../../../lib/libGraphMol.dll.a(d001273.o):(.text+0x0): first defined here
#D:/Projects/Molpher/dependencies/boost/stage/lib/libboost_thread-mgw61-mt-1_61.a(thread.o):thread.cpp:(.text+0x1860): multiple definition of `boost::thread::joinable() const'
#../../../lib/libGraphMol.dll.a(d001274.o):(.text+0x0): first defined here
#D:/Projects/Molpher/dependencies/boost/stage/lib/libboost_thread-mgw61-mt-1_61.a(thread.o):thread.cpp:(.text+0x18c0): multiple definition of `boost::thread::detach()'
#../../../lib/libGraphMol.dll.a(d000983.o):(.text+0x0): first defined here
#D:/Projects/Molpher/dependencies/boost/stage/lib/libboost_thread-mgw61-mt-1_61.a(thread.o):thread.cpp:(.text+0x1a40): multiple definition of `boost::thread::hardware_concurrency()'
#../../../lib/libGraphMol.dll.a(d000978.o):(.text+0x0): first defined here
#D:/Projects/Molpher/dependencies/boost/stage/lib/libboost_thread-mgw61-mt-1_61.a(thread.o):thread.cpp:(.text+0x1c00): multiple definition of `boost::this_thread::get_id()'
#../../../lib/libGraphMol.dll.a(d000795.o):(.text+0x0): first defined here
#D:/Projects/Molpher/dependencies/boost/stage/lib/libboost_thread-mgw61-mt-1_61.a(thread.o):thread.cpp:(.text+0x2230): multiple definition of `boost::detail::thread_data_base::~thread_data_base()'
#../../../lib/libGraphMol.dll.a(d000897.o):(.text+0x0): first defined here
#D:/Projects/Molpher/dependencies/boost/stage/lib/libboost_thread-mgw61-mt-1_61.a(thread.o):thread.cpp:(.text+0x5080): multiple definition of `boost::thread::join_noexcept()'
#../../../lib/libGraphMol.dll.a(d000975.o):(.text+0x0): first defined here
#collect2.exe: error: ld returned 1 exit status
#Code/GraphMol/Substruct/CMakeFiles/SubstructMatch.dir/build.make:129: recipe for target 'bin/libSubstructMatch.dll' failed
#mingw32-make[2]: *** [bin/libSubstructMatch.dll] Error 1
#CMakeFiles/Makefile2:3123: recipe for target 'Code/GraphMol/Substruct/CMakeFiles/SubstructMatch.dir/all' failed
#mingw32-make[1]: *** [Code/GraphMol/Substruct/CMakeFiles/SubstructMatch.dir/all] Error 2
#Makefile:160: recipe for target 'all' failed
#mingw32-make: *** [all] Error 2

link_RDKIT=https://github.com/rdkit/rdkit/archive/Release_2015_03_1.zip

# Dependency compilation flags.
compile_TBB=0
compile_BOOST=0
compile_RDKIT=0
compile_ZLIB=0

# RDKit is dependent on Boost.
rdkit_dep() {
    if [ $compile_RDKIT -eq 1 ]; then
        if [ ! -d ./boost/stage/lib -a $compile_BOOST -eq 0 ]; then
            echo "RDKit is dependent on Boost libraries, which were not found."
            while [ 1 -eq 1 ]
            do
                echo "Do you want to build Boost? (y/n)"
                read resp
                resp=${resp^^}
                case $resp in
                    Y)
                        compile_BOOST=1
                        break
                    ;;
                    N)
                        echo "RDKit will not be built."
                        compile_RDKIT=0
                        break
                    ;;
                    *)
                        echo "Bad response"
                    ;;
                esac
            done
        fi
    fi
}

print_help() {
    echo "Arguemtns:"
    echo "-cmake {Cygwin style path to CMake}"
    echo "-mingw {Cygwin style path to MinGW}"
    echo "Options:"
    echo " -h               this help"
    echo " --all            build all dependencies"
    echo " --tbb            build TBB"
    echo " --boost          build boost"
    echo " --rdkit          build RDKit"
    echo " --zlib           build zlib"
}

# Print list of dependencies to build.
print_all() {
    echo "The following dependencies will be built:"
    if [ $compile_TBB -eq 1 ]; then
        echo " TBB"
    fi
    if [ $compile_BOOST -eq 1 ]; then
        echo " boost"
    fi
    if [ $compile_RDKIT -eq 1 ]; then
        echo " RDKit"
    fi
    if [ $compile_ZLIB -eq 1 ]; then
        echo " zlib"
    fi    
    echo "Used tools:"
    echo " CMake: $CMAKE_PATH"
    echo " MinGW: $MINGW_PATH"
}

check_env() {
    wget -help >/dev/null 2>&1 || { echo >&2 "'wget' is not installed. Aborting."; exit 1; }
    tar --usage >/dev/null 2>&1 || { echo >&2 "'tar' is not installed. Aborting."; exit 1; }
    unzip >/dev/null 2>&1 || { echo >&2 "'unzip' is not installed. Aborting."; exit 1; }
    
    if [ -z "$CMAKE_PATH" ]; then
        echo "Missing --cmake "
        exit 1;
    fi
    if [ -z "$MINGW_PATH" ]; then
        echo "Missing --mingw "
        exit 1;
    fi
}

build_tbb() {
    if [ $compile_TBB -eq 0 ]; then
        return
    fi
    echo "Building TBB"
    cd download
    wget -nc --output-document=tbb.tgz $link_TBB
    cd ..    
    if [ ! -d ./tbb ]; then
        echo " Extracting"
        tar -xvf download/tbb.tgz -C ./
        mv tbb44_* tbb
    fi
    if [ -d ./tbb/build/windows_ia64_gcc_mingw5.2.0_debug ] && [ -d ./tbb/build/windows_ia64_gcc_mingw5.2.0_release ] ; then
        echo " Already compiled"
        return
    fi
    echo " Compiling"
    PATH_BACKUP=$PATH
    PATH=$MINGW_PATH/bin:$PATH    
    cd tbb
    mingw32-make arch=ia64 compiler=gcc tbb tbbmalloc
    cd ..
    PATH=$PATH_BACKUP
}

build_boost() {
    if [ $compile_BOOST -eq 0 ]; then
        return
    fi
    echo "Building boost"
    cd download
    wget -nc --output-document=boost.zip $link_BOOST
    cd ..
    if [ ! -d ./boost ]; then
        echo " Extracting"
        unzip download/boost.zip -d ./
        mv boost_* boost
    fi
    if [ -d ./boost/stage/lib ]; then
        echo " Already compiled"
        return
    fi
    echo " Compiling"
    PATH_BACKUP=$PATH
    PATH=$MINGW_PATH/bin:$PATH    
    cd boost
    chmod 755 ./bootstrap.bat
    ./bootstrap.bat gcc
    # Use -a to force recompilation.
    # Use -q to quit on first error.
    ./b2 toolset=gcc variant=release link=static runtime-link=shared threading=multi -a -q --with-date_time --with-filesystem --with-graph --with-log --with-program_options --with-regex --with-serialization --with-signals --with-thread --with-test
    
    # TODO : add cxxflags="-fpermissive -Wno-unused-value"
        
    cd ..
    PATH=$PATH_BACKUP
}

build_rdkit() {
    if [ $compile_RDKIT -eq 0 ]; then
        return
    fi
    echo "Building RDKit"
    cd download
    wget -nc --output-document=rdkit.zip $link_RDKIT
    cd ..
    # Extract files.
    if [ ! -d ./rdkit ]; then
        echo " Extracting"
        unzip download/rdkit.zip -d ./
        mv rdkit-Release_* rdkit
    fi
    if [ -d ./rdkit/lib ]; then
        echo " Already compiled"
        return
    fi
    echo " Compiling"
    PATH_BACKUP=$PATH
    PATH=$MINGW_PATH/bin:$CMAKE_PATH/bin:$PATH
    cd rdkit
    # CODE UPDATE RDKit in file rdkit/Code/GraphMol/FMCS/DebugTrace.h there is duplicity definition of timezone -> delete it
    #   Used definition is from MinGw/5.2.0/x86_64-w64-mingw32/include/time.h
    echo " Updating DebugTrace.h"
    if [ ! -e Code/GraphMol/FMCS/DebugTrace.h.backup ]; then
        cp Code/GraphMol/FMCS/DebugTrace.h Code/GraphMol/FMCS/DebugTrace.h.backup    
    fi
    cat Code/GraphMol/FMCS/DebugTrace.h.backup | sed '/struct timezone {/ {N; N; N; d;}' > Code/GraphMol/FMCS/DebugTrace.h
    # CODE UPDATE END
    rm -r build 2>/dev/null
    mkdir build
    cd build
    cmake -G "Unix Makefiles" -D BOOST_ROOT=../boost -D RDK_INSTALL_STATIC_LIBS=ON -D RDK_BUILD_PYTHON_WRAPPERS=OFF -D Boost_USE_STATIC_LIBS=ON -D Boost_NO_SYSTEM_PATHS=ON ..
    # We need to use make from MinGw so we need to get rid of path to CygWin.
    PATH=$MINGW_PATH/bin
    mingw32-make
    mingw32-make install
    # We can try to run ctest here to execute test (for that we need to update path).
    cd ..
    cd ..
    PATH=$PATH_BACKUP
}

build_zlib() {
    if [ $compile_ZLIB -eq 0 ]; then
        return
    fi
    echo "Building zlib"
    cd download
    wget -nc --output-document=zlib.zip $link_ZLIB
    cd ..
    if [ ! -d ./zlib ]; then
        echo " Extracting"
        unzip download/zlib.zip -d ./
        mv zlib-* zlib
    fi
    if [ -e ./zlib/libz.a ]; then
        echo " Already compiled"
        return
    fi
    echo " Compiling"
    PATH_BACKUP=$PATH
    PATH=$MINGW_PATH/bin:$PATH    
    cd zlib
    ./configure --static
    make -fwin32/Makefile.gcc
    make test testdll -fwin32/Makefile.gcc
    cd ..
    PATH=$PATH_BACKUP
}

#
# Main script code.
#

if [ $# -eq 0 ]; then
    echo "To show help use option -h"
    exit
fi

for item in "$@"; do
    case $item in
        --cmake=*)
        CMAKE_PATH="${item#*=}"
        ;;
        --mingw=*)
        MINGW_PATH="${item#*=}"
        ;;
        -h)
            print_help ;
            exit
        ;;
        --all)
            compile_TBB=1;
            compile_BOOST=1;
            compile_RDKIT=1;
            compile_ZLIB=1;
        ;;
        --tbb)
            compile_TBB=1
        ;;
        --boost)
            compile_BOOST=1
        ;;
        --rdkit)
            compile_RDKIT=1
        ;;
        --zlib)
            compile_ZLIB=1
        ;;        
    esac
done

check_env
rdkit_dep
print_all

if [ ! -d download ]; then
    mkdir download
fi

build_tbb
build_boost
build_rdkit
build_zlib
