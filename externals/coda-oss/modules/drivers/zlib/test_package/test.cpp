#include <zlib.h>
#include <zip.h>

int main() {
    auto f_zlib = &inflate;
    auto f = &zipOpen;
    return 0;
}
