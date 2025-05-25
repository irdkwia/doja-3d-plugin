#include "helper.h"

__declspec(dllexport) void C3DActionTable_nativeConstructor(vm_env* env) {
	env->stack_ptr -= 1;
	struct ByteArray* ba = (struct ByteArray*)env->stack_ptr[1];
	return_func(env, push_anim(ba, ACT));
}
__declspec(dllexport) void C3DActionTable_nativeGetNumAction(vm_env* env) {
	env->stack_ptr -= 1;
	struct C3DDataAct* act = (struct C3DDataAct*)get_anim(env->stack_ptr[1]);
	return_func(env, act->nb_actions);
}
__declspec(dllexport) void C3DActionTable_nativeGetMaxTime(vm_env* env) {
	env->stack_ptr -= 2;
	struct C3DDataAct* act = (struct C3DDataAct*)get_anim(env->stack_ptr[1]);
	int x = env->stack_ptr[2];
	return_func(env, (int)(act->actions[x].duration * 1000));
}
__declspec(dllexport) void C3DActionTable_nativeDispose(vm_env* env) {
	env->stack_ptr -= 1;
	pop_anim(env->stack_ptr[1]);
}