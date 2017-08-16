Read Plist xml centerlines file generated in Schaffer – Nishimura Lab 
(https://snlab.bme.cornell.edu/) and convert it to vtp format so that
it could be compared with centerlines network generated in vmtk tool

Examples how to use it:

# How to build it:
1) create build directory:
mkdir build
cd build 
2) build the project
cmake ..
(optionale:pass your computer coore number to -j parameter to make things faster)
make -j4 
3) run example
convert plist to .vtp or obj file:
./CompareCeterlines inputFilename.xml outputFilename.obj 1
compare centerlines networks:
./CompareCeterlines inputFilename.vtp outputFilename.vtp 2
