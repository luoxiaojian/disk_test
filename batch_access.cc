#include "common.h"
#include "file_accessor.h"
#include "mmap_accessor.h"

int main(int argc, char** argv) {
  std::string file_name = argv[1];
  std::string strategy = argv[2];
  int dim = atoi(argv[3]);
  int iter = atoi(argv[4]);
  int batch_size = atoi(argv[5]);
  int tn = atoi(argv[6]);

  auto func = [](const std::vector<Vector>& vecs) {
    float sum = 0.0;
    for (auto& vec : vecs) {
      for (int i = 0; i < vec.dim; ++i) {
        sum += vec.data[i];
      }
    }
    return sum;
  };

  if (strategy == "mmap") {
    MMapAccessor accessor;
    accessor.open(file_name, dim);
    parallel_batch_access(accessor, func, tn, iter, batch_size);
    return 0;
  } else if (strategy == "direct") {
    FileAccessor accessor;
    accessor.open(file_name, dim, true);
    parallel_batch_access(accessor, func, tn, iter, batch_size);
    return 0;
  } else if (strategy == "file") {
    FileAccessor accessor;
    accessor.open(file_name, dim, false);
    parallel_batch_access(accessor, func, tn, iter, batch_size);
    return 0;
  }

  return 0;
}