#ifndef MMAP_ACCESSOR_H_
#define MMAP_ACCESSOR_H_

#include "common.h"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

class MMapAccessor {
 public:
  MMapAccessor() : fd_(-1), file_size_(0), dim_(0), data_(nullptr), size_(0) {}
  ~MMapAccessor() { close(); }
  struct Context {
    void init(int, int) {}
  };

  void open(const std::string& filename, int dim) {
    close();
    fd_ = ::open(filename.c_str(), O_RDONLY);
    if (fd_ == -1) {
      std::cout << "open file failed..." << std::endl;
      exit(-1);
    }
    struct stat sb;
    if (fstat(fd_, &sb)) {
      std::cout << "fstat file failed..." << std::endl;
    }
    file_size_ = sb.st_size;
    data_ = static_cast<float*>(
        mmap(nullptr, file_size_, PROT_READ, MAP_PRIVATE, fd_, 0));
    size_ = file_size_ / sizeof(float);

    dim_ = dim;
  }

  void close() {
    if (data_ != nullptr) {
      munmap(static_cast<void*>(data_), file_size_);
      data_ = nullptr;
    }
    if (fd_ != -1) {
      ::close(fd_);
      fd_ = -1;
    }
    file_size_ = 0;
    dim_ = 0;
    size_ = 0;
  }

  size_t vec_num() const { return size_ / dim_; }

  int dim() const { return dim_; }

  void get(size_t idx, Vector& vec) {
    size_t offset = idx;
    offset *= dim_;
    vec.data = data_ + offset;
  }

  void alloc_context(Context& ctx, int batch_size) {}
  void dealloc_context(Context& ctx) {}

  void batch_get(const std::vector<size_t>& idxs, std::vector<Vector>& vecs,
                 Context& ctx) {
    for (size_t i = 0; i < idxs.size(); ++i) {
      get(idxs[i], vecs[i]);
    }
  }

  void alloc_vec(Vector& vec) { vec.dim = dim_; }

  void dealloc_vec(Vector& vec) { vec.dim = 0; }

 private:
  int fd_;
  size_t file_size_;

  int dim_;

  float* data_;
  size_t size_;
};

#endif  // MMAP_ACCESSOR_H_