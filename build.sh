#!/bin/bash

set -e
ARGS=$(getopt --options 'h,m:t:j:' --longoptions 'help,module:,type:,j:,' -- "$@")
eval set -- "${ARGS}"

build_module="pancake-store"
build_type="release"
cpu_cores=""
project_path=$PWD

echo "start to build pancake-store"

while true;
do
    case $1 in
        -h | --help)
            echo "Build Usage:"
            echo "    -h, --help"
            echo "    -m, --module  build module : pancake-store(default) / deps"
            echo "    -t, --type    build type   : release / debug / debug-asan"
            echo "    -j, --j       build cores  : cpu-cores(default)"
            exit
            ;;
        -m | --module)
            build_module=$2
            shift 2
            ;;
        -t | --type)
            build_type=$2
            shift 2
            ;;
        -j | --j)
            cpu_cores=$2
            shift 2
            ;;
        --)
            shift
            break
            ;;
    esac
done

if [[ -z $cpu_cores ]]; then
    cpu_cores=`bash -c "nproc --all"`
fi

echo "build_module:    " $build_module
echo "build_type:      " $build_type
echo "build_cores:     " $cpu_cores
echo ""

gcc_version=`g++ --version`
echo $gcc_version
echo ""

# init and update submodule
git submodule update --init --recursive

CMAKE_FLAGS=""
CMAKE_FLAGS="${CMAKE_FLAGS} -DCMAKE_BUILD_MODULE=${build_module}"
CMAKE_FLAGS="${CMAKE_FLAGS} -DCMAKE_BUILD_TYPE=${build_type}"

# gen proto (TODO)
work_place=$project_path"/build"
if [ ! -d $work_place ]; then
    echo "mkdir "$work_place
    mkdir $work_place
fi
cd $work_place

# build start
echo "cmake "$CMAKE_FLAGS" .."
cmake $CMAKE_FLAGS ..
make -j$cpu_cores