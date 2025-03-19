#!/bin/bash

for t in 1 2 4 8
do
    for x in 16 24 32 48 64 128
    do
        for y in 100 1000 10000 100000 1000000
        do
            echo "size - ${x} GB, iter - ${y}, thread - ${t}"
            cmd="./build/sync_access /data/vec_${x}G.dat 1024 ${y} ${t}"
            echo $cmd
            eval $cmd
        done
    done
done

# for t in 1 2 4 8
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