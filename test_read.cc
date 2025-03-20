#include "common.h"
#include "file_accessor.h"
#include "mmap_accessor.h"

int main() {
  // FileAccessor accessor;
  MMapAccessor accessor;
  // accessor.open("/data/vec_128G.dat", 1024, true);
  // accessor.open("./sample", 1024, true);
  accessor.open("./sample", 1024);
  Vector vec;
  accessor.alloc_vec(vec);
  for (int i = 0; i < 1; ++i) {
    accessor.get(i, vec);
    printf("%f\n", vec.data[0]);
  }
  return 0;
}