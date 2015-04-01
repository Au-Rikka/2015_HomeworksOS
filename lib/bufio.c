#include "bufio.h"


struct buf_t *buf_new(size_t capacity)  {
	struct buf_t* bufer = malloc(sizeof(struct buf_t));
	if (bufer == NULL) {
		return NULL;
	}
	bufer->buf = malloc(capacity);
	if (bufer->buf == NULL) {
		free(bufer);
		return NULL;
	}
	bufer->size = 0;
	bufer->capacity = capacity;
	return bufer;
}

void buf_free(struct buf_t* bufer) {
	free(bufer->buf);
	free(bufer);
}

size_t buf_capacity(struct buf_t* bufer) {
	return bufer->capacity;
}

size_t buf_size(struct buf_t* bufer) {
	return bufer->size;
}

ssize_t buf_fill(int fd, struct buf_t* buffer, size_t required) {
	size_t bytes_read = 1;
	while (bytes_read > 0 && buffer->size < required) {
		bytes_read = read(fd, buffer->buf + buffer->size, buffer->capacity - buffer->size);
		if (bytes_read < 0) {
			return -1;
		}
		buffer->size += bytes_read;
	}

	return buffer->size;
}

ssize_t buf_flush(int fd, struct buf_t* buffer, size_t required) {
	size_t bytes_write = 1;
	size_t offset = 0;

	if (buffer->size < required) {
		required = buffer->size;
	}

	while (bytes_write > 0 && offset < required) {
		bytes_write = write(fd, buffer->buf + offset, buffer->size - offset);
		if (bytes_write < 0) {
			return -1;
		}
		offset += bytes_write;
	}

	size_t i;
	size_t j = 0;
	for (i = offset; i < buffer->size; i++) {
		buffer->buf[j] = buffer->buf[i];
		j++;
	}
	
	buffer->size -= offset;
	
	return offset;
}

