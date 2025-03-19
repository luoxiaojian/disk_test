#ifndef FILE_ACCESSOR_H_
#define FILE_ACCESSOR_H_

class FileAccessor {
 public:
  FileAccessor() : fd_(-1), file_size_(0), dim_(0) {}
  ~FileAccessor() { close(); }

  void open(const std::string& filename, int dim, bool direct) {
    close();
    if (direct) {
      fd_ = ::open(filename.c_str(), O_RDONLY | O_DIRECT);
    } else {
      fd_ = ::open(filename.c_str(), O_RDONLY);
    }
    struct stat sb;
    if (fstat(fd_, &sb)) {
      std::cout << "fstat file failed..." << std::endl;
    }
    file_size_ = sb.st_size;
    block_size_ = sb.st_blksize;
    dim_ = dim;
  }

  void close() {
    if (fd_ != -1) {
      ::close(fd_);
      fd_ = -1;
    }
    file_size_ = 0;
    dim_ = 0;
  }

  size_t vec_num() const { return file_size_ / sizeof(float) / dim_; }

  int dim() const { return dim_; }

  void get(size_t idx, Vector& vec) {
    size_t offset = idx;
    offset *= (dim_ * sizeof(float));
    ssize_t ret = pread(fd_, vec.data, dim_ * sizeof(float), offset);
    if (ret == -1) {
      perror("read failed...");
    }
  }

  void batch_get(const std::vector<size_t>& idxs, std::vector<Vector>& vecs) {
    for (size_t i = 0; i < idxs.size(); ++i) {
      get(idxs[i], vecs[i]);
    }
  }

  void alloc_vec(Vector& vec) {
#if 1
    posix_memalign((void**) &vec.data, block_size_, dim_ * sizeof(float));
#else
    vec.data = (float*) malloc(dim_ * sizeof(float));
#endif
    vec.dim = dim_;
  }

  void dealloc_vec(Vector& vec) {
    free(vec.data);
    vec.data = nullptr;
    vec.dim = 0;
  }

 private:
  int fd_;
  size_t file_size_;
  size_t block_size_;

  int dim_;
};

#endif  // FILE_ACCESSOR_H_