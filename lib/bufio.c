#include "bufio.h"
#include "helpers.h"
#include <string.h>


struct buf_t* buf_new(size_t capacity)  {
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
	#ifdef DEBUG
		if (bufer == NULL) {
			abort();
		}
	#endif

	free(bufer->buf);
	free(bufer);
}

size_t buf_capacity(struct buf_t* bufer) {
	#ifdef DEBUG
		if (bufer == NULL) {
			abort();
		}
	#endif

	return bufer->capacity;
}

size_t buf_size(struct buf_t* bufer) {
	#ifdef DEBUG
		if (buffer == NULL) {
			abort();
		}
	#endif

	return bufer->size;
}

ssize_t buf_fill(int fd, struct buf_t* buffer, size_t required) {
	#ifdef DEBUG
		if (buffer == NULL) {
			abort();
		}
		if (required > buffer->capacity) {
			abort();
		}
	#endif 

	int bytes_read = 1;
	if (required > buffer->capacity) {
		required = buffer->capacity;
	}

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
	#ifdef DEBUG
		if (buffer == NULL) {
			abort();
		}
	#endif

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



ssize_t buf_getline(int fd, struct buf_t* buffer, char* dest) {
	#ifdef DEBUG
		if (buffer == NULL) {
			abort();
		}
	#endif

	size_t i;
	size_t bytes_write = 0;
	int is_done = 0;
	size_t k = 0;

	do {
		for (i = 0; i < buf_size(buffer) && buffer->buf[i] != '\n'; i++) {
			dest[bytes_write] = buffer->buf[i];
			bytes_write++;
		}

		if (i < buf_size(buffer)) {
			is_done = 1;
			i++;
			memmove(buffer->buf, buffer->buf + i, buffer->size - i);
			buffer->size -= i;
		} else {
			buffer->size = 0;
			k = buf_fill(fd, buffer, buffer->capacity);
		}
	} while (is_done == 0 && k > 0);

	if (k < 0) {
		return k;
	}

	return bytes_write;
}




ssize_t buf_write(int fd, struct buf_t* buffer, char* src, size_t len) {
	size_t all = buf_size(buffer) + len;
	size_t bytes_write = 0;
	size_t offset = 0;
	size_t i;

	while (bytes_write < all) {
		for (i = 0; (offset + i < len) && (buffer->size + i < buffer->capacity); i++) {
			buffer->buf[buffer->size + i] = src[offset + i]; 
		}
		offset += i;
		buffer->size += i;

		size_t k = buf_flush(fd, buffer, buf_size(buffer));
		if (k >= 0) {
			bytes_write += k;
		} else {
			return k;
		}
	}	

	return bytes_write;
}