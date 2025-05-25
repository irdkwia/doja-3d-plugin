#include "helper.h"

static int buffer_gl[] = {
    0,
    GL_COLOR_BUFFER_BIT,
    GL_DEPTH_BUFFER_BIT,
    0,
    GL_STENCIL_BUFFER_BIT,
};
static int c3dl_gl[] = {GL_NEVER,
GL_ALWAYS,
GL_LESS,
GL_LEQUAL,
GL_EQUAL,
GL_GEQUAL,
GL_GREATER,
GL_NOTEQUAL,
GL_POINTS,
GL_LINES,
GL_LINE_STRIP,
GL_LINE_LOOP,
GL_TRIANGLES,
GL_TRIANGLE_STRIP,
GL_TRIANGLE_FAN,
GL_QUADS,
GL_QUAD_STRIP,
GL_POLYGON,
GL_ZERO,
GL_ONE,
GL_SRC_COLOR,
GL_ONE_MINUS_SRC_COLOR,
GL_DST_COLOR,
GL_ONE_MINUS_DST_COLOR,
GL_SRC_ALPHA,
GL_ONE_MINUS_SRC_ALPHA,
GL_COLOR_BUFFER_BIT,
GL_DEPTH_BUFFER_BIT,
GL_STENCIL_BUFFER_BIT,
GL_TRUE,
GL_FALSE,
GL_FRONT_AND_BACK,
GL_AMBIENT,
GL_DIFFUSE,
GL_AMBIENT_AND_DIFFUSE,
GL_SPECULAR,
GL_EMISSION,
GL_ALPHA_TEST,
GL_BLEND,
GL_COLOR_MATERIAL,
GL_CULL_FACE,
GL_DEPTH_TEST,
GL_DITHER,
GL_FOG,
GL_LIGHT0,
GL_LIGHT1,
GL_LIGHT2,
GL_LIGHT3,
GL_LIGHT4,
GL_LIGHT5,
GL_LIGHT6,
GL_LIGHT7,
GL_LIGHTING,
GL_LINE_SMOOTH,
GL_SCISSOR_TEST,
GL_STENCIL_TEST,
GL_TEXTURE_2D,
GL_TEXTURE_GEN_S,
GL_TEXTURE_GEN_T,
GL_FOG_START,
GL_FOG_END,
GL_FOG_COLOR,
GL_CW,
GL_CCW,
GL_VENDOR,
GL_VERSION,
GL_POSITION,
GL_SPOT_CUTOFF,
GL_SPOT_DIRECTION,
GL_SPOT_EXPONENT,
GL_CONSTANT_ATTENUATION,
GL_LINEAR_ATTENUATION,
GL_SHININESS,
GL_MODELVIEW,
GL_PROJECTION,
GL_TEXTURE,
GL_COMPILE,
0,
0,
GL_KEEP,
GL_REPLACE,
GL_INCR,
GL_DECR,
GL_INVERT,
GL_TEXTURE_ENV,
GL_TEXTURE_ENV_COLOR,
GL_TEXTURE_ENV_MODE,
GL_MODULATE,
GL_DECAL,
GL_OBJECT_PLANE,
GL_EYE_PLANE,
GL_TEXTURE_GEN_MODE,
GL_OBJECT_LINEAR,
GL_EYE_LINEAR,
GL_S,
GL_T,
GL_RGB,
GL_RGBA,
GL_RGB4,
GL_RGB5,
GL_RGB8,
GL_RGBA4,
GL_RGBA8,
GL_RGB5_A1,
GL_UNSIGNED_BYTE,
GL_TEXTURE_MIN_FILTER,
GL_TEXTURE_MAG_FILTER,
GL_TEXTURE_WRAP_S,
GL_TEXTURE_WRAP_T,
GL_NEAREST,
GL_LINEAR,
GL_CLAMP,
GL_REPEAT,
GL_NO_ERROR,
GL_INVALID_VALUE,
GL_INVALID_OPERATION,
GL_OUT_OF_MEMORY,
GL_SPHERE_MAP,
GL_LIGHT_MODEL_COLOR_CONTROL,
GL_SINGLE_COLOR,
GL_SEPARATE_SPECULAR_COLOR,
GL_FOG_MODE,
GL_EXP,
GL_EXP2,
GL_FOG_DENSITY
};

#define TEX_SIZE 512
int texture_names[TEX_SIZE];

int get_gl_enum(int c3dl_val) {
    if (c3dl_val >= 125) return 0;
    return c3dl_gl[c3dl_val];
}

static const EGLint configAttribs[] = {
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_DEPTH_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
        EGL_NONE
};

typedef struct {
    int x, y, width, height;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    unsigned char* data;
    unsigned char* buffer;
} context;
static context ctx;
void copy3DContext() {
    unsigned char* data = ctx.buffer;
    glReadPixels(ctx.x, ctx.y, ctx.width, ctx.height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, data);
    for (int i = 0;i < (ctx.width - ctx.x) * (ctx.height - ctx.y);++i) {
        ctx.data[i * 3] = ((ctx.data[i * 3] * (255 - data[i * 4 + 3])) + (data[i * 4] * data[i * 4 + 3])) / 255;
        ctx.data[i * 3 + 1] = ((ctx.data[i * 3 + 1] * (255 - data[i * 4 + 3])) + (data[i * 4 + 1] * data[i * 4 + 3])) / 255;
        ctx.data[i * 3 + 2] = ((ctx.data[i * 3 + 2] * (255 - data[i * 4 + 3])) + (data[i * 4 + 2] * data[i * 4 + 3])) / 255;
    }
}
__declspec(dllexport) void C3DGraphics_nativeCreateContext(vm_env* env) {
    env->stack_ptr -= 8;
    int x = env->stack_ptr[1];
    int y = env->stack_ptr[2];
    int width = env->stack_ptr[3];
    int height = env->stack_ptr[4];
    struct Graphics* graphics = (struct Graphics*)env->stack_ptr[5];
    struct Frame* frame = (struct Frame*)env->stack_ptr[6];
    int frame_not_exists = env->stack_ptr[7];
    struct Font* font = (struct Font*)env->stack_ptr[8];
    ctx.x = x;
    ctx.y = y;
    ctx.width = width;
    ctx.height = height;
    if (!frame_not_exists) {
        struct Graphics* g2 = frame->impl->graphics;
        frame->impl->graphics = graphics;
        graphics->id = g2->id;
        g2->id = 0;
        g2->finalizer = 0;
        graphics->pictoColor = g2->pictoColor;
        graphics->owner = frame;
        graphics->finalizer = g2->finalizer;
    }
    graphics->initFont = font;
    ctx.data = (void*)graphics->id[1][2];
    ctx.buffer = malloc_s((ctx.width - ctx.x) * (ctx.height - ctx.y) * 4);

    // 1. Initialize EGL
    ctx.display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    EGLint major, minor;

    eglInitialize(ctx.display, &major, &minor);

    // 2. Select an appropriate configuration
    EGLint numConfigs;
    EGLConfig eglCfg;

    eglChooseConfig(ctx.display, configAttribs, &eglCfg, 1, &numConfigs);

    EGLint pbufferAttribs[] = {
      EGL_WIDTH, width - x,
      EGL_HEIGHT, height - y,
      EGL_NONE,
    };

    // 3. Create a surface
    ctx.surface = eglCreatePbufferSurface(ctx.display, eglCfg, pbufferAttribs);

    // 4. Bind the API
    eglBindAPI(EGL_OPENGL_API);

    EGLint pContextAttribs[] = {
          EGL_CONTEXT_MAJOR_VERSION, 2,
          EGL_CONTEXT_MINOR_VERSION, 0,
          EGL_NONE,
    };
    // 5. Create a context and make it current
    ctx.context = eglCreateContext(ctx.display, eglCfg, EGL_NO_CONTEXT, pContextAttribs);

    eglMakeCurrent(ctx.display, ctx.surface, ctx.surface, ctx.context);
    print_flush("VERSION %s\n", glGetString(GL_VERSION));
    int ver;
    glGetIntegerv(GL_MAJOR_VERSION, &ver);
    print_flush("MAJOR %d\n", ver);
    glGetIntegerv(GL_MINOR_VERSION, &ver);
    print_flush("MINOR %d\n", ver);
    glClearDepth(1.0f);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    for (int i = 0; i < TEX_SIZE; ++i) {
        texture_names[i] = -1;
    }
}
__declspec(dllexport) void C3DGraphics_nativeDeleteContext(vm_env* env) {
    eglTerminate(ctx.display);
}

__declspec(dllexport) void C3DGraphics_nativeAlphaFunc(vm_env* env) {
    env->stack_ptr -= 2;
    int x = get_gl_enum(env->stack_ptr[1]);
    float y = ntof(env->stack_ptr[2]);
    glAlphaFunc(x, y);
}

__declspec(dllexport) void C3DGraphics_nativeBegin(vm_env* env) {
    env->stack_ptr -= 1;
    int x = get_gl_enum(env->stack_ptr[1]);
    glBegin(x);
}
__declspec(dllexport) void C3DGraphics_nativeBindTexture(vm_env* env) {
    env->stack_ptr -= 2;
    int x = get_gl_enum(env->stack_ptr[1]);
    int y = env->stack_ptr[2];
    if (texture_names[y] == -1) {
        glGenTextures(1, texture_names + y);
        glBindTexture(x, texture_names[y]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
    else {
        glBindTexture(x, texture_names[y]);
    }
}
__declspec(dllexport) void C3DGraphics_nativeBlendFunc(vm_env* env) {
    env->stack_ptr -= 2;
    int x = get_gl_enum(env->stack_ptr[1]);
    int y = get_gl_enum(env->stack_ptr[2]);
    glBlendFunc(x, y);
}
__declspec(dllexport) void C3DGraphics_nativeCallList(vm_env* env) {
    env->stack_ptr -= 1;
    int x = env->stack_ptr[1];
    glCallList(x);
}
int find_act_seq(float* data, int nb_keys, int epk, float time) {
    for (int i = 0; i < nb_keys; ++i) {
        if (time < data[epk * i]) {
            return i;
        }
    }
    return nb_keys;
}
float interpolate(float istart, float iend, float icurrent, float vstart, float vend) {
    float perc = (icurrent - istart) / (iend - istart);
    return vstart + (vend - vstart) * perc;
}
void call_action_table(struct C3DDataAct* act, int action, float time, int point) {
    struct C3DDTActPartInfo* part = NULL;
    for (int i = 0; i < act->actions[action].nb_parts; ++i) {
        part = act->actions[action].parts[i];
        if (part->id == point) {
            int intern = 0;
            switch (part->type) {
            case 0x32:;
                glMultMatrixf(part->data);
                break;
            case 0x61:
                intern = find_act_seq(part->data, part->nb_keys, 5, time);
                if (intern == part->nb_keys) {
                    glRotatef(part->data[(intern - 1) * 5 + 4], part->data[(intern - 1) * 5 + 1], part->data[(intern - 1) * 5 + 2], part->data[(intern - 1) * 5 + 3]);
                }
                else {
                    float rot[4];
                    rot[0] = interpolate(part->data[(intern - 1) * 5], part->data[intern * 5], time, part->data[(intern - 1) * 5 + 1], part->data[intern * 5 + 1]);
                    rot[1] = interpolate(part->data[(intern - 1) * 5], part->data[intern * 5], time, part->data[(intern - 1) * 5 + 2], part->data[intern * 5 + 2]);
                    rot[2] = interpolate(part->data[(intern - 1) * 5], part->data[intern * 5], time, part->data[(intern - 1) * 5 + 3], part->data[intern * 5 + 3]);
                    rot[3] = interpolate(part->data[(intern - 1) * 5], part->data[intern * 5], time, part->data[(intern - 1) * 5 + 4], part->data[intern * 5 + 4]);
                    glRotatef(rot[3], rot[0], rot[1], rot[2]);
                }
                break;
            case 0x62:
            case 0x162:
                intern = find_act_seq(part->data, part->nb_keys, 4, time);
                if (intern == part->nb_keys) {
                    if (part->type == 0x62) {
                        glTranslatef(part->data[(intern - 1) * 4 + 1], part->data[(intern - 1) * 4 + 2], part->data[(intern - 1) * 4 + 3]);
                    } else {
                        glScalef(part->data[(intern - 1) * 4 + 1], part->data[(intern - 1) * 4 + 2], part->data[(intern - 1) * 4 + 3]);
                    }
                }
                else {
                    float trs[3];
                    trs[0] = interpolate(part->data[(intern - 1) * 4], part->data[intern * 4], time, part->data[(intern - 1) * 4 + 1], part->data[intern * 4 + 1]);
                    trs[1] = interpolate(part->data[(intern - 1) * 4], part->data[intern * 4], time, part->data[(intern - 1) * 4 + 2], part->data[intern * 4 + 2]);
                    trs[2] = interpolate(part->data[(intern - 1) * 4], part->data[intern * 4], time, part->data[(intern - 1) * 4 + 3], part->data[intern * 4 + 3]);
                    if (part->type == 0x62) {
                        glTranslatef(trs[0], trs[1], trs[2]);
                    } else {
                        glScalef(trs[0], trs[1], trs[2]);
                    }
                }
                break;
            }
        }
    }
}
void use_material(struct C3DDTNodeMaterial* node_29) {
    int enabled_color = glIsEnabled(GL_COLOR_MATERIAL);
    glDisable(GL_COLOR_MATERIAL);
    float amb[] = { node_29->data[0] * node_29->data[2], node_29->data[0] * node_29->data[3], node_29->data[0] * node_29->data[4], 1 };
    float diff[] = { node_29->data[2], node_29->data[3], node_29->data[4], node_29->data[1] };
    float spec[] = { node_29->data[5], node_29->data[6], node_29->data[7], 1 };
    float emss[] = { node_29->data[8], node_29->data[9], node_29->data[10], 1 };
    float shns = node_29->data[11];
    glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT, GL_SHININESS, shns);
    glMaterialfv(GL_FRONT, GL_EMISSION, emss);
    if (enabled_color) {
        glEnable(GL_COLOR_MATERIAL);
    }
}
void use_texture(struct C3DDTNodeTexture* node_44, struct C3DDataTex* tex) {
    if (tex != NULL) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, node_44->repeat_s ? GL_REPEAT : GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, node_44->repeat_t ? GL_REPEAT : GL_CLAMP);
        glTexImage2D(GL_TEXTURE_2D,
            0,
            GL_RGBA8,
            tex->textures[node_44->tex_no].width,
            tex->textures[node_44->tex_no].height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            tex->textures[node_44->tex_no].data);
    }
}
__declspec(dllexport) void C3DGraphics_nativeCallFigure(vm_env* env) {
    int enabled_tex = glIsEnabled(GL_TEXTURE_2D);
    int enabled_ccw;
    float texture_stack[32][16];
    int ts_number = 0;
    for (int i = 0; i < 16; ++i) texture_stack[ts_number][i] = i % 4 == i / 4 ? 1.0f : 0.0f;

    glGetIntegerv(GL_FRONT_FACE, &enabled_ccw);
    glDisable(GL_TEXTURE_2D);
    glFrontFace(GL_CW);
    glPushMatrix();
    env->stack_ptr -= 1;
    struct Figure* fig_data = (struct Figure*)env->stack_ptr[1];
    struct C3DDataFig* fig = (struct C3DDataFig*)get_anim(fig_data->impl->id);
    struct C3DDataTex* tex = (struct C3DDataTex*)get_anim(fig->tex_id);
    struct C3DDataAct* act = (struct C3DDataAct*)get_anim(fig->act_id);
    float* coordset = NULL;
    int tex_no;
    glGenTextures(1, &tex_no);
    glBindTexture(GL_TEXTURE_2D, tex_no);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    int no_material = 1;
    for (int i = 0; i < fig->nb_nodes; ++i) {

        switch (fig->nodes[i]->type) {
        case 0x29:;
            no_material = 0;
            use_material((struct C3DDTNodeMaterial*)fig->nodes[i]);
            break;
        case 0x44:;
            use_texture((struct C3DDTNodeTexture*)fig->nodes[i], tex);
            break;
        case 0x34:
            glPushMatrix();
            ts_number += 1;
            for (int i = 0; i < 16; ++i) texture_stack[ts_number][i] = texture_stack[ts_number - 1][i];
            break;
        case 0x35:
            ts_number -= 1;
            glPopMatrix();
            break;
        case 0x39:;
            struct C3DDTNodeCoordsSet* node_39 = (struct C3DDTNodeCoordsSet*)fig->nodes[i];
            if (node_39->coords_no&1) {
                glFrontFace(GL_CCW);
            }
            else {
                glFrontFace(GL_CW);
            }
            break;
        case 0x36:
        case 0x136:;
            struct C3DDTNodeFloat3* node_36 = (struct C3DDTNodeFloat3*)fig->nodes[i];
            if (node_36->type==0x136) {
                glTranslatef(node_36->data[0], node_36->data[1], node_36->data[2]);
            }
            else {
                texture_stack[ts_number][12] += node_36->data[0];
                texture_stack[ts_number][13] += node_36->data[1];
                texture_stack[ts_number][14] += node_36->data[2];
            }
            break;
        case 0x38:
        case 0x138:;
            struct C3DDTNodeFloat3* node_38 = (struct C3DDTNodeFloat3*)fig->nodes[i];
            if (node_38->type == 0x138) {
                glScalef(node_38->data[0], node_38->data[1], node_38->data[2]);
            } else {
                texture_stack[ts_number][0] *= node_38->data[0];
                texture_stack[ts_number][5] *= node_38->data[1];
                texture_stack[ts_number][10] *= node_38->data[2];
            }
            break;
        case 0x37:;
            struct C3DDTNodeFloat4* node_37 = (struct C3DDTNodeFloat4*)fig->nodes[i];
            glRotatef(node_37->data[3], node_37->data[0], node_37->data[1], node_37->data[2]);
            break;
        case 0x10:;
        case 0x11:;
            struct C3DDTNodeShape* node_11 = (struct C3DDTNodeShape*)fig->nodes[i];
            int off = 0;
            while (node_11->data[off] != 0) {
                int len = node_11->data[off];
                off += 1;

                if (node_11->flags & 0x4) {
                    glEnable(GL_TEXTURE_2D);
                }
                else {
                    glDisable(GL_TEXTURE_2D);
                }
                if (no_material) {
                    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
                }
                else {
                    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                }
                if (node_11->flags & 0x1000) {
                    glBegin(GL_POLYGON);
                } else {
                    glBegin(GL_TRIANGLE_STRIP);
                }
                for (int i = 0;i < len;++i) {
                    int v = (node_11->data[off]) & 0xFFFF;
                    if (coordset != NULL) {
                        if (node_11->flags & 0x4) {
                            float x = coordset[(v << 3)] * texture_stack[ts_number][0] + coordset[(v << 3) + 1] * texture_stack[ts_number][4] + texture_stack[ts_number][12];
                            float y = coordset[(v << 3)] * texture_stack[ts_number][1] + coordset[(v << 3) + 1] * texture_stack[ts_number][5] + texture_stack[ts_number][13];
                            glTexCoord2f(x, y);
                            glNormal3f(coordset[(v << 3) + 2], coordset[(v << 3) + 3], coordset[(v << 3) + 4]);
                            glVertex3f(coordset[(v << 3) + 5], coordset[(v << 3) + 6], coordset[(v << 3) + 7]);
                        }
                        else {
                            glNormal3f(coordset[v * 6], coordset[v * 6 + 1], coordset[v * 6 + 2]);
                            glVertex3f(coordset[v * 6 + 3], coordset[v * 6 + 4], coordset[v * 6 + 5]);
                        }
                    }
                    off += 1;
                }
                glEnd();
            }
            break;
        case 0x43:
            no_material = 1;
            break;
        case 0x47:;
            struct C3DDTNodeCoordsSet* node_47 = (struct C3DDTNodeCoordsSet*)fig->nodes[i];
            break;
        case 0x51:
        case 0x53:;
            struct C3DDTNodeCoordsSet* node_53 = (struct C3DDTNodeCoordsSet*)fig->nodes[i];
            for (int i = 0; i < fig->nb_coords; ++i) {
                if (fig->coords[i]->nb == node_53->coords_no) {
                    if (node_53->type == 0x53) {
                        coordset = fig->coords[i]->data;
                    }
                    if (node_53->type == 0x51) {
                        if (fig->coords[i]->mat != NULL) {
                            no_material = 0;
                            use_material(fig->coords[i]->mat);
                        }
                        else {
                            no_material = 1;
                        }
                        if (fig->coords[i]->tex != NULL) {
                            use_texture(fig->coords[i]->tex, tex);
                        }
                    }
                }
            }
            break;
        case 0x54:;
            struct C3DDTNodeCoordsSet* node_54 = (struct C3DDTNodeCoordsSet*)fig->nodes[i];
            call_action_table(act, fig->action, fig->time, node_54->coords_no);
            break;
        }
    }
    glDeleteTextures(1, &tex_no);
    glPopMatrix();
    glFrontFace(enabled_ccw);
    if (enabled_tex) {
        glEnable(GL_TEXTURE_2D);
    } else {
        glDisable(GL_TEXTURE_2D);
    }
}
__declspec(dllexport) void C3DGraphics_nativeClear(vm_env* env) {
    env->stack_ptr -= 2;
    int x = buffer_gl[env->stack_ptr[1]];
    int y = buffer_gl[env->stack_ptr[2]]; // UNUSED
    glClear(x);
    glClear(y);
}
__declspec(dllexport) void C3DGraphics_nativeClearColor(vm_env* env) {
    env->stack_ptr -= 4;
    float x = ntof(env->stack_ptr[1]);
    float y = ntof(env->stack_ptr[2]);
    float z = ntof(env->stack_ptr[3]);
    float a = ntof(env->stack_ptr[4]);
    glClearColor(x, y, z, a);
}
__declspec(dllexport) void C3DGraphics_nativeClearDepth(vm_env* env) {
    env->stack_ptr -= 1;
    double x = ntod(env->stack_ptr[1]);
    glClearDepth(x);
}
__declspec(dllexport) void C3DGraphics_nativeClearStencil(vm_env* env) {
    env->stack_ptr -= 1;
    int x = env->stack_ptr[1];
    glClearStencil(x);
}
__declspec(dllexport) void C3DGraphics_nativeColor(vm_env* env) { 
    print_flush("C3DGraphics_nativeColor\n");
    env->stack_ptr -= 4;
    float x = ntof(env->stack_ptr[1]);
    float y = ntof(env->stack_ptr[2]);
    float z = ntof(env->stack_ptr[3]);
    float a = ntof(env->stack_ptr[4]);
    glColor4f(x, y, z, a);
}
__declspec(dllexport) void C3DGraphics_nativeColorv(vm_env* env) {
    print_flush("C3DGraphics_nativeColorv\n");
    env->stack_ptr -= 1;
    struct IntArray* x = (struct IntArray*)env->stack_ptr[1];
    float* xa = malloc_s(sizeof(float) * x->length);
    for (int i = 0;i < x->length;++i) {
        xa[i] = ntof(x->data[i]);
    }
    glColor4fv(xa);
    free(xa);
}
__declspec(dllexport) void C3DGraphics_nativeColorMask(vm_env* env) {
    print_flush("C3DGraphics_nativeColorMask\n");
    // TODO: Boolean
    env->stack_ptr -= 4;
    int x = get_gl_enum(env->stack_ptr[1]);
    int y = get_gl_enum(env->stack_ptr[2]);
    int z = get_gl_enum(env->stack_ptr[3]);
    int a = get_gl_enum(env->stack_ptr[4]);
    glColorMask(x, y, z, a);
}
__declspec(dllexport) void C3DGraphics_nativeColorMaterial(vm_env* env) {
    print_flush("C3DGraphics_nativeColorMaterial\n");
    env->stack_ptr -= 2;
    int x = get_gl_enum(env->stack_ptr[1]);
    int y = get_gl_enum(env->stack_ptr[2]);
    glColorMaterial(x, y);
}
__declspec(dllexport) void C3DGraphics_nativeEnd(vm_env* env) {
    glEnd();
}
__declspec(dllexport) void C3DGraphics_nativeDeleteLists(vm_env* env) {
    env->stack_ptr -= 2;
    int x = env->stack_ptr[1];
    int y = env->stack_ptr[2];
    glDeleteLists(x, y);
}
__declspec(dllexport) void C3DGraphics_nativeDeleteTextures(vm_env* env) {
    env->stack_ptr -= 2;
    int x = env->stack_ptr[1];
    struct IntArray* y = (struct IntArray*)env->stack_ptr[2];
    int* ya = malloc_s(sizeof(int) * y->length);
    for (int i = 0;i < y->length;++i) {
        ya[i] = texture_names[y->data[i]];
        texture_names[y->data[i]] = -1;
    }
    glDeleteTextures(x, ya);
    free(ya);
}
__declspec(dllexport) void C3DGraphics_nativeDepthFunc(vm_env* env) {
    print_flush("C3DGraphics_nativeDepthFunc\n");
    env->stack_ptr -= 1;
    int x = env->stack_ptr[1];
    glDepthFunc(x);
}
__declspec(dllexport) void C3DGraphics_nativeDepthMask(vm_env* env) {
    print_flush("C3DGraphics_nativeDepthMask\n");
    // TODO: Boolean
    env->stack_ptr -= 1;
    int x = get_gl_enum(env->stack_ptr[1]);
    glDepthMask(x);
}
__declspec(dllexport) void C3DGraphics_nativeEnable(vm_env* env) {
    env->stack_ptr -= 1;
    int x = get_gl_enum(env->stack_ptr[1]);
    glEnable(x);
}
__declspec(dllexport) void C3DGraphics_nativeDisable(vm_env* env) {
    env->stack_ptr -= 1;
    int x = get_gl_enum(env->stack_ptr[1]);
    glDisable(x);
}
__declspec(dllexport) void C3DGraphics_nativeFinish(vm_env* env) {
    env->stack_ptr -= 1;
    int x = env->stack_ptr[1]; // UNUSED
    glFinish();
    copy3DContext();
}
__declspec(dllexport) void C3DGraphics_nativeFlush(vm_env* env) {
    env->stack_ptr -= 1;
    int x = env->stack_ptr[1]; // UNUSED
    glFlush();
    copy3DContext();
}
__declspec(dllexport) void C3DGraphics_nativeFog(vm_env* env) {
    print_flush("C3DGraphics_nativeFog\n");
    env->stack_ptr -= 2;
    int x = get_gl_enum(env->stack_ptr[1]);
    float y = ntof(env->stack_ptr[2]);
    glFogf(x, y);
}
__declspec(dllexport) void C3DGraphics_nativeFogv(vm_env* env) {
    print_flush("C3DGraphics_nativeFogv\n");
    env->stack_ptr -= 2;
    int x = get_gl_enum(env->stack_ptr[1]);
    struct IntArray* y = (struct IntArray*)env->stack_ptr[2];
    float* ya = malloc_s(sizeof(float) * y->length);
    for (int i = 0;i < y->length;++i) {
        ya[i] = ntof(y->data[i]);
    }
    glFogfv(x, ya);
    free(ya);
}
__declspec(dllexport) void C3DGraphics_nativeFrontFace(vm_env* env) {
    print_flush("C3DGraphics_nativeFrontFace\n");
    env->stack_ptr -= 1;
    int x = get_gl_enum(env->stack_ptr[1]);
    glFrontFace(x);
}
__declspec(dllexport) void C3DGraphics_nativeFrustum(vm_env* env) {
    print_flush("C3DGraphics_nativeFrustum\n");
    env->stack_ptr -= 6;
    double x = ntod(env->stack_ptr[1]);
    double y = ntod(env->stack_ptr[2]);
    double z = ntod(env->stack_ptr[3]);
    double a = ntod(env->stack_ptr[4]);
    double b = ntod(env->stack_ptr[5]);
    double c = ntod(env->stack_ptr[6]);
    glFrustum(x, y, z, a, b, c);
}
__declspec(dllexport) void C3DGraphics_nativeGetError(vm_env* env) {
    print_flush("C3DGraphics_nativeGetError\n");
    return_func(env, glGetError());
}
__declspec(dllexport) void C3DGraphics_nativeGetString(vm_env* env) {
    // TODO
    print_flush("C3DGraphics_nativeGetString\n");
    //glGetString(NULL);
    return_func(env, 0);
}
__declspec(dllexport) void C3DGraphics_nativeLight(vm_env* env) {
    env->stack_ptr -= 3;
    int x = get_gl_enum(env->stack_ptr[1]);
    int y = get_gl_enum(env->stack_ptr[2]);
    struct IntArray* z = (struct IntArray*)env->stack_ptr[3];
    float* za = malloc_s(sizeof(float)*z->length);
    for (int i = 0;i < z->length;++i) {
        za[i] = ntof(z->data[i]);
    }
    glLightfv(x, y, za);
    free(za);
}
__declspec(dllexport) void C3DGraphics_nativeLightModeli(vm_env* env) {
    print_flush("C3DGraphics_nativeLightModeli\n");
    env->stack_ptr -= 2;
    int x = get_gl_enum(env->stack_ptr[1]);
    int y = get_gl_enum(env->stack_ptr[2]);
    glLightModeli(x, y);
}
__declspec(dllexport) void C3DGraphics_nativeLoadIdentity(vm_env* env) {
    glLoadIdentity();
}
__declspec(dllexport) void C3DGraphics_nativeLoadMatrix(vm_env* env) {
    print_flush("C3DGraphics_nativeLoadMatrix\n");
    env->stack_ptr -= 1;
    struct IntArray* x = (struct IntArray*)env->stack_ptr[1];
    float* xa = malloc_s(sizeof(float) * x->length);
    for (int i = 0;i < x->length;++i) {
        xa[i] = ntof(x->data[i]);
    }
    glLoadMatrixf(xa);
    free(xa);
}

__declspec(dllexport) void C3DGraphics_nativeLookAt(vm_env* env) {
    env->stack_ptr -= 9;
    gluLookAt(ntod(env->stack_ptr[1]), ntod(env->stack_ptr[2]), ntod(env->stack_ptr[3]), 
        ntod(env->stack_ptr[4]), ntod(env->stack_ptr[5]), ntod(env->stack_ptr[6]), 
        ntod(env->stack_ptr[7]), ntod(env->stack_ptr[8]), ntod(env->stack_ptr[9]));
}
__declspec(dllexport) void C3DGraphics_nativeMaterial(vm_env* env) {
    print_flush("C3DGraphics_nativeMaterial\n");
    env->stack_ptr -= 3;
    int x = get_gl_enum(env->stack_ptr[1]);
    int y = get_gl_enum(env->stack_ptr[2]);
    struct IntArray* z = (struct IntArray*)env->stack_ptr[3];
    float* za = malloc_s(sizeof(float) * z->length);
    for (int i = 0;i < z->length;++i) {
        za[i] = ntof(z->data[i]);
    }
    glMaterialfv(x, y, za);
    free(za);
}
__declspec(dllexport) void C3DGraphics_nativeMatrixMode(vm_env* env) {
    env->stack_ptr -= 1;
    int x = get_gl_enum(env->stack_ptr[1]);
    glMatrixMode(x);
}
__declspec(dllexport) void C3DGraphics_nativeMultiMatrix(vm_env* env) {
    print_flush("C3DGraphics_nativeMultiMatrix\n");
    env->stack_ptr -= 1;
    struct IntArray* x = (struct IntArray*)env->stack_ptr[1];
    float* xa = malloc_s(sizeof(float) * x->length);
    for (int i = 0;i < x->length;++i) {
        xa[i] = ntof(x->data[i]);
    }
    glMultMatrixf(xa);
    free(xa);
}
__declspec(dllexport) void C3DGraphics_nativeNewList(vm_env* env) {
    env->stack_ptr -= 2;
    int x = env->stack_ptr[1];
    int y = get_gl_enum(env->stack_ptr[2]);
    glNewList(x, y);
}
__declspec(dllexport) void C3DGraphics_nativeEndList(vm_env* env) {
    glEndList();
}
__declspec(dllexport) void C3DGraphics_nativeNormal0(vm_env* env) {
    print_flush("C3DGraphics_nativeNormal0\n");
    env->stack_ptr -= 3;
    float x = ntof(env->stack_ptr[1]);
    float y = ntof(env->stack_ptr[2]);
    float z = ntof(env->stack_ptr[3]);
    glNormal3f(x, y, z);
}
__declspec(dllexport) void C3DGraphics_nativeNormalv(vm_env* env) {
    print_flush("C3DGraphics_nativeNormalv\n");
    env->stack_ptr -= 1;
    struct IntArray* x = (struct IntArray*)env->stack_ptr[1];
    float* xa = malloc_s(sizeof(float) * x->length);
    for (int i = 0;i < x->length;++i) {
        xa[i] = ntof(x->data[i]);
    }
    glNormal3fv(xa);
    free(xa);
}
__declspec(dllexport) void C3DGraphics_nativePushMatrix0(vm_env* env) {
    glPushMatrix();
}
__declspec(dllexport) void C3DGraphics_nativePopMatrix(vm_env* env) {
    glPopMatrix();
}
__declspec(dllexport) void C3DGraphics_nativePerspective(vm_env* env) {
    env->stack_ptr -= 4;
    double x = ntod(env->stack_ptr[1]);
    double y = ntod(env->stack_ptr[2]);
    double z = ntod(env->stack_ptr[3]);
    double a = ntod(env->stack_ptr[4]);
    gluPerspective(x, y, z, a);
}
__declspec(dllexport) void C3DGraphics_nativeRotate(vm_env* env) {
    env->stack_ptr -= 4;
    float x = ntof(env->stack_ptr[1]);
    float y = ntof(env->stack_ptr[2]);
    float z = ntof(env->stack_ptr[3]);
    float a = ntof(env->stack_ptr[4]);
    glRotatef(x, y, z, a);
}
__declspec(dllexport) void C3DGraphics_nativeScale(vm_env* env) {
    env->stack_ptr -= 3;
    float x = ntof(env->stack_ptr[1]);
    float y = ntof(env->stack_ptr[2]);
    float z = ntof(env->stack_ptr[3]);
    glScalef(x, y, z);
}
__declspec(dllexport) void C3DGraphics_nativeScissorMethod(vm_env* env) {
    print_flush("C3DGraphics_nativeScissorMethod\n");
    env->stack_ptr -= 4;
    int x = env->stack_ptr[1];
    int y = env->stack_ptr[2];
    int z = env->stack_ptr[3];
    int a = env->stack_ptr[4];
    glScissor(x, y, z, a);
}
__declspec(dllexport) void C3DGraphics_nativeSphere(vm_env* env) {
    print_flush("C3DGraphics_nativeSphere\n");
    GLUquadric* quad;
    quad = gluNewQuadric();
    gluSphere(quad, 1, 20, 20);
    gluDeleteQuadric(quad);
}
__declspec(dllexport) void C3DGraphics_nativeStencilFunc(vm_env* env) {
    print_flush("C3DGraphics_nativeStencilFunc\n");
    env->stack_ptr -= 3;
    int x = get_gl_enum(env->stack_ptr[1]);
    int y = env->stack_ptr[2];
    int z = env->stack_ptr[3];
    glStencilFunc(x, y, z);
}
__declspec(dllexport) void C3DGraphics_nativeStencilMask(vm_env* env) {
    print_flush("C3DGraphics_nativeStencilMask\n");
    env->stack_ptr -= 1;
    int x = env->stack_ptr[1];
    glStencilMask(x);
}
__declspec(dllexport) void C3DGraphics_nativeStencilOp(vm_env* env) {
    print_flush("C3DGraphics_nativeStencilOp\n");
    env->stack_ptr -= 3;
    int x = get_gl_enum(env->stack_ptr[1]);
    int y = get_gl_enum(env->stack_ptr[2]);
    int z = get_gl_enum(env->stack_ptr[3]);
    glStencilOp(x, y, z);
}
__declspec(dllexport) void C3DGraphics_nativeTexCoord(vm_env* env) {
    env->stack_ptr -= 2;
    float x = ntof(env->stack_ptr[1]);
    float y = ntof(env->stack_ptr[2]);
    glTexCoord2f(x, y);
}
__declspec(dllexport) void C3DGraphics_nativeTexCoordv(vm_env* env) {
    print_flush("C3DGraphics_nativeTexCoordv\n");
    env->stack_ptr -= 1;
    struct IntArray* x = (struct IntArray*)env->stack_ptr[1];
    float* xa = malloc_s(sizeof(float) * x->length);
    for (int i = 0;i < x->length;++i) {
        xa[i] = ntof(x->data[i]);
    }
    glTexCoord2fv(xa);
    free(xa);
}
__declspec(dllexport) void C3DGraphics_nativeTexEnvf(vm_env* env) {
    env->stack_ptr -= 3;
    int x = get_gl_enum(env->stack_ptr[1]);
    int y = get_gl_enum(env->stack_ptr[2]);
    struct IntArray* z = (struct IntArray*)env->stack_ptr[3];
    glTexEnviv(x, y, z->data);
}
__declspec(dllexport) void C3DGraphics_nativeTexEnvi(vm_env* env) {
    env->stack_ptr -= 3;
    int x = get_gl_enum(env->stack_ptr[1]);
    int y = get_gl_enum(env->stack_ptr[2]);
    int z = get_gl_enum(env->stack_ptr[3]);
    glTexEnvi(x, y, z);
}
__declspec(dllexport) void C3DGraphics_nativeTexGenf(vm_env* env) {
    print_flush("C3DGraphics_nativeTexGenf\n");
    env->stack_ptr -= 3;
    int x = get_gl_enum(env->stack_ptr[1]);
    int y = get_gl_enum(env->stack_ptr[2]);
    struct IntArray* z = (struct IntArray*)env->stack_ptr[3];
    float* za = malloc_s(sizeof(float) * z->length);
    for (int i = 0;i < z->length;++i) {
        za[i] = ntof(z->data[i]);
    }
    glTexGenfv(x, y, za);
    free(za);
}
__declspec(dllexport) void C3DGraphics_nativeTexGeni0(vm_env* env) {
    print_flush("C3DGraphics_nativeTexGeni0\n");
    env->stack_ptr -= 3;
    int x = get_gl_enum(env->stack_ptr[1]);
    int y = get_gl_enum(env->stack_ptr[2]);
    int z = get_gl_enum(env->stack_ptr[3]);
    glTexGeni(x, y, z);
}
__declspec(dllexport) void C3DGraphics_nativeTexImage2D(vm_env* env) {
    env->stack_ptr -= 9;
    int target = get_gl_enum(env->stack_ptr[1]);
    int internalFormat = get_gl_enum(env->stack_ptr[3]);
    int format = get_gl_enum(env->stack_ptr[7]);
    int type = get_gl_enum(env->stack_ptr[8]);
    struct ByteArray* pixels = (struct ByteArray*)env->stack_ptr[9];
    glTexImage2D(target, env->stack_ptr[2], internalFormat, env->stack_ptr[4], env->stack_ptr[5], env->stack_ptr[6], format, type, pixels->data);
}
__declspec(dllexport) void C3DGraphics_nativeTexParameter(vm_env* env) {
    print_flush("C3DGraphics_nativeTexParameter\n");
    env->stack_ptr -= 3;
    int x = get_gl_enum(env->stack_ptr[1]);
    int y = get_gl_enum(env->stack_ptr[2]);
    int z = get_gl_enum(env->stack_ptr[3]);
    glTexParameteri(x, y, z);
}
__declspec(dllexport) void C3DGraphics_nativeTexSubImage2D0(vm_env* env) {
    print_flush("C3DGraphics_nativeTexSubImage2D0\n");
    env->stack_ptr -= 9;
    int target = get_gl_enum(env->stack_ptr[1]);
    int format = get_gl_enum(env->stack_ptr[7]);
    int type = get_gl_enum(env->stack_ptr[8]);
    struct ByteArray* pixels = (struct ByteArray*)env->stack_ptr[9];
    glTexSubImage2D(target, env->stack_ptr[2], env->stack_ptr[3], env->stack_ptr[4], env->stack_ptr[5], env->stack_ptr[6], format, type, pixels->data);
}
__declspec(dllexport) void C3DGraphics_nativeTranslate0(vm_env* env) {
    env->stack_ptr -= 3;
    float x = ntof(env->stack_ptr[1]);
    float y = ntof(env->stack_ptr[2]);
    float z = ntof(env->stack_ptr[3]);
    glTranslatef(x, y, z);
}
__declspec(dllexport) void C3DGraphics_nativeVertex(vm_env* env) {
    env->stack_ptr -= 3;
    float x = ntof(env->stack_ptr[1]);
    float y = ntof(env->stack_ptr[2]);
    float z = ntof(env->stack_ptr[3]);
    glVertex3f(x, y, z);
}
__declspec(dllexport) void C3DGraphics_nativeVertexv(vm_env* env) {
    print_flush("C3DGraphics_nativeVertexv\n");
    env->stack_ptr -= 1;
    struct IntArray* x = (struct IntArray*)env->stack_ptr[1];
    float* xa = malloc_s(sizeof(float) * x->length);
    for (int i = 0;i < x->length;++i) {
        xa[i] = ntof(x->data[i]);
    }
    glVertex3fv(xa);
    free(xa);
}
__declspec(dllexport) void C3DGraphics_nativeViewport(vm_env* env) {
    env->stack_ptr -= 4;
    int x = env->stack_ptr[1];
    int y = env->stack_ptr[2];
    int z = env->stack_ptr[3];
    int a = env->stack_ptr[4];
    glViewport(x, y, z, a);
}