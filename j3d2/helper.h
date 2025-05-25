#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>
#pragma comment(lib, "lib/opengl32.lib")
#include "GL/gl.h"
#pragma comment(lib, "glu32.lib")
#include <GL/glu.h>
#pragma comment(lib, "lib/libEGL.lib")
#include "EGL/egl.h"
#include "c3d_anim.h"
#include "classes.h"
#include "conversion.h"

#define print_flush(...) printf(__VA_ARGS__); fflush(stdout)

#define ACT 0
#define FIG 1
#define TEX 2

void return_func(vm_env* env, int val);
void* malloc_s(int size);