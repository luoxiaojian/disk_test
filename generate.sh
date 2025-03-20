#!/bin/bash

# ./build/generate_blob /data/vec_64G.dat 16777216 1024
./build/generate_blob /data/vec_128G.dat 33554432 1024
./build/generate_blob /data/vec_256G.dat 67108864 1024
./build/generate_blob /data/vec_512G.dat 134217728 1024
echo "finished..."
