#include <iostream>
#include <random>

#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char** argv) {
    if (argc < 4) {
        std::cout << "./generate_blob <output-file> <vec_num> <vec_dim>" << std::endl;
        exit(0);
    }
    std::string target_file = argv[1];
    int vec_num = atoi(argv[2]);
    int vec_dim = atoi(argv[3]);

    size_t blob_size = vec_num;
    blob_size *= vec_dim;

    size_t file_size = blob_size * sizeof(float);

    std::random_device rd;
    std::mt19937 gen(rd());

    int fd = open(target_file.c_str(), O_RDWR | O_CREAT | O_EXCL | O_TRUNC, 0666);
    if (fd == -1) {
        std::cout << "failed to open file " << target_file << std::endl;
        exit(0);
    }

    if (posix_fallocate(fd, 0, file_size) != 0) {
        std::cout << "failed to truncate file" << std::endl;
        exit(0);
    }

    void* addr = mmap(nullptr, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    char* data = static_cast<char*>(addr);

    std::uniform_real_distribution<float> dis(0.0, 100.0);
    for (size_t i = 0; i < blob_size; ++i) {
        data[i] = dis(gen);
    }

    if (msync(addr, file_size, MS_SYNC) == -1) {
        std::cout << "failed to sync" << std::endl;
        exit(0);
    }

    munmap(addr, file_size);
    close(fd);

    return 0;
}