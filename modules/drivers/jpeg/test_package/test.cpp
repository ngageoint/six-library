#include <stddef.h>
#include <stdio.h>
#include <jpeglib.h>

int main() {
    void *p = (void*)(jpeg_destroy_decompress);
    return 0;
}
