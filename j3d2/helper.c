#include "helper.h"

void return_func(vm_env* env, int val) {
    env->stack_ptr[0] = val;
}
void* malloc_s(int size) {
    void* data = malloc(size);
    if (data == NULL) {
        print_flush("ALERT: memory allocation (0x%08X bytes) failed!\n", size);
        exit(1);
    }
    return data;
}