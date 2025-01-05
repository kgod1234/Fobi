mkdir build
cd build
cmake ..
make
cd bin
sudo chmod a+rw /dev/ttyUSB0
./MyProject
cd ..
cd ..