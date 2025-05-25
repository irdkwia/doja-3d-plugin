#include <math.h>

#include "helper.h"

__declspec(dllexport) void C3DMath_nativeacos(vm_env* env) {
    env->stack_ptr -= 1;
    return_func(env, rton(acos(ntod(env->stack_ptr[1]))));
}
__declspec(dllexport) void C3DMath_nativeasin(vm_env* env) {
    env->stack_ptr -= 1;
    return_func(env, rton(asin(ntod(env->stack_ptr[1]))));
}
__declspec(dllexport) void C3DMath_nativeatan(vm_env* env) {
    env->stack_ptr -= 1;
    return_func(env, rton(atan(ntod(env->stack_ptr[1]))));
}
__declspec(dllexport) void C3DMath_nativeatan2(vm_env* env) {
    env->stack_ptr -= 2;
    return_func(env, rton(atan2(ntod(env->stack_ptr[1]), ntod(env->stack_ptr[2]))));
}
__declspec(dllexport) void C3DMath_nativecos(vm_env* env) {
    env->stack_ptr -= 1;
    return_func(env, dton(cos(ntor(env->stack_ptr[1]))));
}
__declspec(dllexport) void C3DMath_nativelog(vm_env* env) {
    env->stack_ptr -= 1;
    return_func(env, dton(log(ntod(env->stack_ptr[1]))));
}
__declspec(dllexport) void C3DMath_nativepow(vm_env* env) {
    env->stack_ptr -= 2;
    return_func(env, dton(pow(ntod(env->stack_ptr[1]), ntod(env->stack_ptr[2]))));
}
__declspec(dllexport) void C3DMath_nativesin(vm_env* env) {
    env->stack_ptr -= 1;
    return_func(env, dton(sin(ntor(env->stack_ptr[1]))));
}
__declspec(dllexport) void C3DMath_nativesqrt(vm_env* env) {
    env->stack_ptr -= 1;
    return_func(env, dton(sqrt(ntod(env->stack_ptr[1]))));
}
__declspec(dllexport) void C3DMath_nativetan(vm_env* env) {
    env->stack_ptr -= 1;
    return_func(env, dton(tan(ntor(env->stack_ptr[1]))));
}