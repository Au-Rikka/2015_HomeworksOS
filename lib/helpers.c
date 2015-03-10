#include "helpers.h"

ssize_t read_(int fd, void* buf, size_t count) {
    ssize_t bytes_read = 0;
    ssize_t cur = 1;

    while (cur > 0) {
        cur = read(fd, buf + bytes_read, count);
        if (cur < 0) {
            return cur;
        }
        bytes_read += cur;
        count -= cur;
    }

    return bytes_read;
}


ssize_t write_(int fd, void* buf, size_t count) {
    ssize_t bytes_write = 0;
    ssize_t cur = 1;

    while (cur != 0) {
        cur = write(fd, buf + bytes_write, count);
        if (cur < 0) {
            return cur;
        }
        bytes_write += cur;
        count -= cur;
    }

    return bytes_write;
}
