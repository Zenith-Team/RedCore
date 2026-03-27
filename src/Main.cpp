#include <telkin/Print.h>

void main() {
    tk::print("Welcome to REDCORE\n");
}

extern "C" void free(void* ptr) {
    return MEMFreeToDefaultHeap(ptr);
}
