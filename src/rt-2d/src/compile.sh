# Use the line below to compile your code *without* multithreading for development/testing

#g++ -g -O3 light2D.c -lm -o light2D
# Use the line below to compile your code *wuth* multithreading for fast rendering of high-quality images
#/usr/lib/aomp/bin/clang -O3 -v -fopenmp -fopenmp-targets=amdgcn-amd-amdhsa -Xopenmp-target=amdgcn-amd-amdhsa -march=gfx900 light2D.c -lm -o light2D 
g++ -O3 -fopenmp -march=native light2D.c -lm -o light2D