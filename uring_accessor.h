#ifndef URING_ACCESSOR_H_
#define URING_ACCESSOR_H_

#include "common.h"

#include <string>

#include <liburing.h>

class UringAccessor {
  static constexpr int kQueueDepth = 1024;

 public:
  UringAccessor() {}
  ~UringAccessor() {}

  struct Context {
    Context() : fd(-1) {}

    int fd;
    struct io_uring ring;
    std::vector<struct iovec> iovecs;
  };

  void alloc_context(Context& ctx, int batch_size) {
    dealloc_context(ctx);
    ctx.fd = ::open(filename_.c_str(), O_RDONLY | O_DIRECT);
    // struct io_uring_params params = {
    //     .sq_entries = kQueueDepth,
    //     .flags = IORING_SETUP_SQPOLL | IORING_SETUP_COOP_TASKRUN};
    // auto ret = io_uring_queue_init_params(kQueueDepth, &ctx.ring, &params);
    auto ret = io_uring_queue_init(kQueueDepth, &ctx.ring, 0);
    if (ret) {
      std::cerr << "io_uring init failed: " << ret << std::endl;
      exit(-1);
    }
    // if (io_uring_queue_init_params(kQueueDepth, &ctx.ring, &params)) {
    //   perror("io_uring init");
    //   std::cerr << "io_uring init failed..." << std::endl;
    //   exit(-1);
    // }
    ctx.iovecs.resize(batch_size);
  }

  void dealloc_context(Context& ctx) {
    if (ctx.fd != -1) {
      ::close(ctx.fd);
      ctx.fd = -1;
    }
  }

  void open(const std::string& filename, int dim) {
    filename_ = filename;
    dim_ = dim;

    int fd = ::open(filename.c_str(), O_RDONLY | O_DIRECT);
    struct stat sb;
    if (fstat(fd, &sb)) {
      std::cout << "fstat file failed..." << std::endl;
    }
    file_size_ = sb.st_size;
    block_size_ = sb.st_blksize;
    dim_ = dim;

    ::close(fd);
  }

  void close() {}

  size_t vec_num() const { return file_size_ / sizeof(float) / dim_; }

  int dim() const { return dim_; }

#if 0
  void batch_get(const std::vector<size_t>& idxs, std::vector<Vector>& vecs,
                 Context& ctx) {
    size_t batch_size = idxs.size();
    for (size_t i = 0; i != batch_size; ++i) {
      struct io_uring_sqe* sqe = io_uring_get_sqe(&ctx.ring);
      if (!sqe) {
        std::cerr << "get sqe failed..." << std::endl;
        exit(-1);
      }

      size_t offset = idxs[i];
      offset *= (dim_ * sizeof(float));
      io_uring_prep_read(sqe, ctx.fd, vecs[i].data, dim_ * sizeof(float),
                         offset);
      io_uring_sqe_set_data(sqe, vecs[i].data);
    }
    io_uring_submit(&ctx.ring);

    size_t harvested = 0;
    unsigned head;
    io_uring_for_each_cqe(&ctx.ring, head, ctx.cqes[harvested]) {
      if (++harvested >= batch_size) {
        break;
      }
    }
    float* buf = static_cast<float*>(io_uring_cqe_get_data(ctx.cqes[0]));
    std::cout << "buf[0] = " << buf[0] << ", vecs[0][0] = " << vecs[0].data[0]
              << std::endl;
    io_uring_cq_advance(&ctx.ring, harvested);
  }
#else
  void batch_get(const std::vector<size_t>& idxs, std::vector<Vector>& vecs,
                 Context& ctx) {
    size_t batch_size = idxs.size();
    size_t batch_idx = 0;
    int ret;
    while (batch_idx < batch_size) {
      int pending = 0;
      while (batch_idx < batch_size) {
        struct io_uring_sqe* sqe = io_uring_get_sqe(&ctx.ring);
        if (!sqe) {
          break;
        }

        ctx.iovecs[batch_idx].iov_base = vecs[batch_idx].data;
        ctx.iovecs[batch_idx].iov_len = dim_ * sizeof(float);
        size_t offset = idxs[batch_idx];
        offset *= (dim_ * sizeof(float));
        io_uring_prep_readv(sqe, ctx.fd, &ctx.iovecs[batch_idx], 1, offset);
        ++batch_idx;
        ++pending;
      }
      if (pending) {
        ret = io_uring_submit(&ctx.ring);
        if (ret < 0) {
          std::cerr << "io_uring_submit: " << strerror(-ret) << std::endl;
        }
        while (pending) {
          struct io_uring_cqe* cqe;
          ret = io_uring_wait_cqe(&ctx.ring, &cqe);
          if (ret < 0) {
            std::cerr << "io_uring_wait_cqe: " << strerror(-ret) << std::endl;
          }
          if (cqe->res != (dim_ * sizeof(float))) {
            std::cerr << "cqe->res = " << cqe->res
                      << ", expected: " << dim_ * sizeof(float) << std::endl;
          }
          io_uring_cqe_seen(&ctx.ring, cqe);
          --pending;
        }
      }
    }
  }
#endif

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
  std::string filename_;
  int dim_;
  size_t file_size_;
  size_t block_size_;
};

#endif  // URING_ACCESSOR_H_