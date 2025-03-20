#!/bin/bash

./generate.sh

for t in 1 2 4 8 16 32 64
do
    for x in 64 128 256 512
    do
        for y in 1000000
        do
            echo "size - ${x} GB, iter - ${y}, thread - ${t}"
            cmda="./build/sync_access /data/vec_${x}G.dat mmap 1024 ${y} ${t}"
            echo $cmda
            eval $cmda
            cmdb="./build/sync_access /data/vec_${x}G.dat direct 1024 ${y} ${t}"
            echo $cmdb
            eval $cmdb
        done
    done
done

# for t in 1 2 4 8 16 32 64
# do
#     for x in 16 24 32 48 64 128
#     do
#         y=$((x * 262144))
#         echo "size - ${x} GB, iter - ${y}, thread - ${t}"
#         cmd="./build/sync_access /data/vec_${x}G.dat 1024 ${y} ${t}"
#         echo $cmd
#         eval $cmd
#     done
# done

for t in 1 2 4 8 16 32 64
do
    for x in 64 128 256 512
    do
        for y in 10000
        do
            echo "size - ${x} GB, iter - ${y}, thread - ${t}"
            cmda="./build/batch_access /data/vec_${x}G.dat mmap 1024 ${y} 100 ${t}"
            echo $cmda
            eval $cmda
            cmdb="./build/batch_access /data/vec_${x}G.dat direct 1024 ${y} 100 ${t}"
            echo $cmdb
            eval $cmdb
            cmdc="./build/batch_access /data/vec_${x}G.dat uring 1024 ${y} 100 ${t}"
            echo $cmdc
            eval $cmdc
        done
    done
done