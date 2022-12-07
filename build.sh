rm -rf build
mkdir build
cd build
cmake ../memalloc
make
cp ./memalloc ../memalloc
