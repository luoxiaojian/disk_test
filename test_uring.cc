#include <fcntl.h>
#include <liburing.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define QD 4

int main() {
  struct io_uring ring;
  int ret = io_uring_queue_init(QD, &ring, 0);
  if (ret < 0) {
    fprintf(stderr, "queue init: %s\n", strerror(-ret));
    return 1;
  }

  int fd = open("/data/vec_64G.dat", O_RDONLY | O_DIRECT);
  if (fd < 0) {
    perror("open");
    exit(1);
  }

  struct iovec* iovecs =
      static_cast<struct iovec*>(calloc(QD, sizeof(struct iovec)));
  for (int i = 0; i < QD; ++i) {
    void* buf;
    if (posix_memalign(&buf, 4096, 4096)) {
      return 1;
    }
    memset(buf, 0, 4096);
    iovecs[i].iov_base = buf;
    iovecs[i].iov_len = 4096;
  }

  for (int i = 0; i < QD; ++i) {
    struct io_uring_sqe* sqe = io_uring_get_sqe(&ring);
    if (!sqe) {
      fprintf(stderr, "get %d-th sqe failed\n", i + 1);
      break;
    }
    io_uring_prep_readv(sqe, fd, &iovecs[i], 1, i * 4096);
  }

  ret = io_uring_submit(&ring);

  for (int i = 0; i < QD; ++i) {
    struct io_uring_cqe* cqe;
    int ret = io_uring_wait_cqe(&ring, &cqe);
    if (ret < 0) {
      fprintf(stderr, "wait cqe: %s\n", strerror(-ret));
      return 1;
    }

    if (cqe->res != 4096) {
      fprintf(stderr, "cqe->res = %d\n", cqe->res);
      return 1;
    }
    printf("cqe->res = %d\n", cqe->res);
    for (int k = 0; k < QD; ++k) {
      printf("%f\n", static_cast<float*>(iovecs[k].iov_base)[0]);
    }
    io_uring_cqe_seen(&ring, cqe);
  }

  close(fd);
  io_uring_queue_exit(&ring);
  for (int i = 0; i < QD; ++i) {
    free(iovecs[i].iov_base);
  }
  free(iovecs);
  return 0;
}