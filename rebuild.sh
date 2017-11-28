rm -rf build_epuck
mkdir build_epuck
cd build_epuck
cmake -DCMAKE_TOOLCHAIN_FILE=/home/arena/repos/robots-thesis/TargetEPuck.cmake ../src
make
