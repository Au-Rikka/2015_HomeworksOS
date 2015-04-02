#include <stdio.h>
#include <stdlib.h>
#include <bufio.h>

size_t const BUF_SIZE = 4096;

int main() {
    struct buf_t* buffer = buf_new(BUF_SIZE);
    size_t write_bytes;

    size_t read_bytes = buf_fill(STDIN_FILENO, buffer, BUF_SIZE);
    while (read_bytes > 0) {
        write_bytes = buf_flush(STDOUT_FILENO, buffer, read_bytes);
        if (write_bytes < 0) {
            buf_free(buffer);
            return EXIT_FAILURE;
        }

        read_bytes = buf_fill(STDIN_FILENO, buffer, BUF_SIZE);
    } 

    write_bytes = buf_flush(STDOUT_FILENO, buffer, read_bytes);
    
    buf_free(buffer);

    if (write_bytes < 0 || read_bytes < 0) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
