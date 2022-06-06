#include <stdint.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>


#include "io-webp.h"

#define MAX_BUFF_SIZE 5000000

int delete_file(const char*);
char *buf_to_file(const uint8_t*, size_t);

int delete_file(const char *pathname) {
  int ret = unlink(pathname);
  free((void *)pathname);
  return ret;
}

char *buf_to_file(const uint8_t *buf, size_t size) {
  char *pathname = strdup("/dev/shm/fuzz-XXXXXX");
  if (pathname == NULL) {
    return NULL;
  }

  int fd = mkstemp(pathname);
  if (fd == -1) {
    free(pathname);
    return NULL;
  }

  size_t pos = 0;
  while (pos < size) {
    int nbytes = write(fd, &buf[pos], size - pos);
    if (nbytes <= 0) {
      if (nbytes == -1 && errno == EINTR) {
        continue;
      }
      goto err;
    }
    pos += nbytes;
  }

  if (close(fd) == -1) {
    goto err;
  }

  return pathname;

err:
  delete_file(pathname);
  return NULL;
}
int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    GError *error = NULL;
    if (size > MAX_BUFF_SIZE) return 0;

    const char *filename = buf_to_file(data, size);
    if (!filename)
        exit(EXIT_FAILURE);
    GdkPixbuf *pix = gdk_pixbuf_new_from_file(filename, &error);
    if (pix) {
        gdk_pixbuf_get_width(pix);
        g_object_unref(pix);
    }

    //Cleanup
    delete_file(filename);
    if(error) g_error_free(error);
    return 0;
}
