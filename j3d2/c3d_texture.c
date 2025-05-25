#include "helper.h"

__declspec(dllexport) void C3DTexture_nativeConstructor(vm_env* env) {
    env->stack_ptr -= 1;
    struct ByteArray* ba = (struct ByteArray*)env->stack_ptr[1];
	return_func(env, push_anim(ba, TEX));
}
__declspec(dllexport) void C3DTexture_nativeDispose(vm_env* env) {
	env->stack_ptr -= 1;
	pop_anim(env->stack_ptr[1]);
}