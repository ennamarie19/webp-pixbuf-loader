#include <stdint.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_BUFF_SIZE 5000000

uint8_t *buff[MAX_BUFF_SIZE];
int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    GError *error = NULL;
    if (size > MAX_BUFF_SIZE) return 0;
    FILE *f = fmemopen(buff, MAX_BUFF_SIZE, "w+");
    GdkPixbuf *pix = gdk_pixbuf__webp_image_load(f, &error);
    if (pix) {
        gdk_pixbuf_get_width(pix);
        g_object_unref(pix);
    }

    //Cleanup
    fclose(f);
    if(error) g_error_free(error);
    return 0;
}
