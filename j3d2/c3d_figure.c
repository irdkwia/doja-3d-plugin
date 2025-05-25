#include "helper.h"

__declspec(dllexport) void C3DFigure_nativeConstructor(vm_env* env) {
	env->stack_ptr -= 1;
	struct ByteArray* ba = (struct ByteArray*)env->stack_ptr[1];
	return_func(env, push_anim(ba, FIG));
}
__declspec(dllexport) void C3DFigure_nativeSetTexture(vm_env* env) {
	env->stack_ptr -= 2;
	struct C3DDataFig* fig = (struct C3DDataFig*)get_anim(env->stack_ptr[1]);
	struct Texture* tex = (struct Texture*) env->stack_ptr[2];
	if (tex == NULL) {
		fig->tex_id = -1;
	}
	else {
		fig->tex_id = tex->impl->id;
	}
}
__declspec(dllexport) void C3DFigure_nativeSetPostureByTime(vm_env* env) {
	env->stack_ptr -= 4;
	struct C3DDataFig* fig = (struct C3DDataFig*)get_anim(env->stack_ptr[1]);
	struct ActionTable* act = (struct ActionTable*)env->stack_ptr[2];
	if (act == NULL) {
		fig->act_id = -1;
	}
	else {
		fig->act_id = act->impl->id;
	}
	fig->action = env->stack_ptr[3];
	fig->time = env->stack_ptr[4]/1000.f;
}
__declspec(dllexport) void C3DFigure_nativeGetAnimationTime(vm_env* env) {
	env->stack_ptr -= 1;
	struct C3DDataFig* fig = (struct C3DDataFig*)get_anim(env->stack_ptr[1]);
	return_func(env, (int) (fig->time*1000));
}
__declspec(dllexport) void C3DFigure_nativeDispose(vm_env* env) {
	env->stack_ptr -= 1;
	fflush(stdout);
	pop_anim(env->stack_ptr[1]);
}