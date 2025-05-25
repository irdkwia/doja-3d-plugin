#include "helper.h"

__declspec(dllexport) void melcoC3d_SystemInitialize() {
	print_flush("melcoC3d System Initialize\n");
	init_anim();
}
__declspec(dllexport) void melcoC3d_ApplicationInitialize() {
	print_flush("melcoC3d Application Initialize\n");
}
__declspec(dllexport) void melcoC3d_SystemEnd() {
	print_flush("melcoC3d System End\n");
	end_anim();
}
__declspec(dllexport) void melcoC3d_mKvmFinalizeC3D() {
	print_flush("melcoC3d mKvm Finalize C3D\n");
}
__declspec(dllexport) void melcoC3d_mKvmSweepInstance3D() {
	print_flush("melcoC3d mKvm Sweep Instance 3D\n");
}

__declspec(dllexport) void c3dCanvasInstSweepResource() {
}
__declspec(dllexport) void c3dgraphicsInstSweepResource() {
}
__declspec(dllexport) void c3danimationInstSweepResource(struct AnimImpl* x) {
}