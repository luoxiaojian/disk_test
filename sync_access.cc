#include "common.h"
#include "mmap_accessor.h"
#include "file_accessor.h"

int main(int argc, char** argv) {
    std::string file_name = argv[1];
    int dim = atoi(argv[2]);
    int iter = atoi(argv[3]);
    int tn = atoi(argv[4]);

#if 0
    MMapAccessor accessor;
    accessor.open(file_name, dim);
#else
    FileAccessor accessor;
    accessor.open(file_name, dim, true);
#endif

    parallel_access(accessor, tn, iter);

    return 0;
}