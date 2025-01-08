#!/bin/bash

WINDOWS=1
RELEASE=0
TEST=0
OSX=1
VALIDATION=0
VERBOSE=0
VK_SDK="include/vendored/VulkanSDK"
BENCHMARK=0
EXAMPLES=0
CLEAN=1
NO_WARN=0
SANITISE=0

while [[ $# -gt 0 ]]; do
  case $1 in
    -w|--windows)
      WINDOWS=0
      shift # past argument
      ;;
    -e|--examples)
      EXAMPLES=1
      shift
      ;;
    -v|--validation)
      VALIDATION=1
      shift
      ;;
    -vv|--verbose-validation)
      VALIDATION=1
      VERBOSE=1
      shift
      ;;
    --vk)
      VK_SDK=$2
      shift
      shift
      ;;
    -o|--osx)
      OSX=0
      shift
      ;;
    -r|--release)
      RELEASE=1
      shift
      ;;
    -b|--benchmark)
      BENCHMARK=1
      shift
      ;;
    -t|--test)
      TEST=1
      shift
      ;;
    --sanitise)
      SANITISE=1
      shift
      ;;
    -d|--development)
      NO_WARN=1
      shift
      ;;
    -c|--continue)
      CLEAN=0
      shift
      ;;
    -*|--*)
      echo "Unknown option $1"
      exit 1
      ;;
    *)
      POSITIONAL_ARGS+=("$1") # save positional arg
      shift # past argument
      ;;
  esac
done

export VULKAN_SDK=$VK_SDK
export VULKAN_LIBRARY="$VK_SDK/Linux/Lib"
export VULKAN_INCLUDE_DIR="$VK_SDK/Include"

if [[ $CLEAN -eq 1 ]]; 
then
  for file in build CMakeFiles cmake_install.cmake CMakeCache.txt Makefile Particles
  do
    if [ -d $file ];
    then
      rm -rf $file
    fi
    if [ -f $file ];
    then
      rm $file
    fi
  done
  cmake -E make_directory build
fi

if [[ $WINDOWS -eq 0 ]];
then 
  export VULKAN_SDK=$VK_SDK/Windows
  export VULKAN_LIBRARY="$VK_SDK/Windows/Lib"
  export VULKAN_INCLUDE_DIR="$VK_SDK/Include" 
  dir=$(pwd)
  ln -s "$dir/$VK_SDK/Windows/Lib" "$dir/$VK_SDK/Windows/lib"
  ln -s "$dir/$VK_SDK/Include" "$dir/$VK_SDK/Windows/Include"
  ln -s "$dir/$VK_SDK/Windows/Include" "$dir/$VK_SDK/Windows/include"
  cd build
  cmake .. -D WINDOWS=ON -D SANITISE=$SANITISE -D VERBOSE=$VERBOSE -D EXAMPLES=$EXAMPLES -D VALIDATION=$VALIDATION -D RELEASE=$RELEASE -D TEST_SUITE=$TEST -D NO_WARN=$NO_WARN -D CMAKE_TOOLCHAIN_FILE=./windows.cmake 
  make -j 4
  export STATUS=$?
  cd ..
  # now copy dlls
  PREFIX="x86_64-w64-mingw32"

  paths=("/usr/local/mingw64/bin"
    "/usr/local/mingw64/bin/x64"
     "/usr/$PREFIX/bin"
    "/usr/$PREFIX/lib"
  )

  for p in /usr/lib/gcc/$PREFIX/*
  do 
    paths+=($p)
  done

  echo -e "\n###############\nChecking Paths: \n"
  for p in "${paths[@]}"
  do
    echo -e "$p\n"
  done 
  echo -e "###############\n"

  dll=()

  for j in "${dll[@]}"
  do
    findAndCopyDLL $j || echo "Could not find $j"
  done
elif [[ $OSX -eq 0 ]];
then
  cd build
  cmake .. -D OSX=ON -D RELEASE=$RELEASE -D SANITISE=$SANITISE -D TEST_SUITE=$TEST -D EXAMPLES=$EXAMPLES -D NO_WARN=$NO_WARN -D CMAKE_TOOLCHAIN_FILE=./osx.cmake
  make -j 4
  export STATUS=$?
  cd ..
else
  cd build
  cmake -D BENCHMARK=$BENCHMARK -D SANITISE=$SANITISE -D VERBOSE=$VERBOSE -D VALIDATION=$VALIDATION -D RELEASE=$RELEASE -D TEST_SUITE=$TEST -D NO_WARN=$NO_WARN -D EXAMPLES=$EXAMPLES ..
  make -j 4
  export STATUS=$?
  cd ..
fi

exit $STATUS
