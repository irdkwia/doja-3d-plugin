#pragma once
#include "helper.h"
#include "zlib.h"

struct C3DFile {
    int length;
    void* data;
};
struct C3DData {
    void (*dispose)(struct C3DData* data);
};
struct C3DDTNode {
    int type;
};
struct C3DDTNodeShape {
    int type;
    int flags;
    int* data;
};
struct C3DDTNodeMaterial {
    int type;
    float data[12];
};
struct C3DDTNodeFloat3 {
    int type;
    float data[3];
};
struct C3DDTNodeFloat4 {
    int type;
    float data[4];
};
struct C3DDTNodeCoordsSet {
    int type;
    int coords_no;
};
struct C3DDTNodeTexture {
    int type;
    int tex_no;
    int repeat_t;
    int repeat_s;
};
struct C3DDTCoords {
    int nb;
    struct C3DDTNodeMaterial* mat;
    struct C3DDTNodeTexture* tex;
    float* data;
};
struct C3DDTActPartInfo {
    int id;
    int type;
    float duration;
    int nb_keys;
    float* data;
};
struct C3DDTActInfo {
    float duration;
    int nb_parts;
    struct C3DDTActPartInfo** parts;
};
struct C3DDTTexInfo {
    int width, height;
    int* data;
};
struct C3DDataFig {
    void (*dispose)(struct C3DDataFig* data);
    int tex_id;
    int act_id;
    int action;
    float time;
    int nb_nodes;
    struct C3DDTNode** nodes;
    int nb_coords;
    struct C3DDTCoords** coords;
};
struct C3DDataAct {
    void (*dispose)(struct C3DDataAct* data);
    int nb_actions;
    struct C3DDTActInfo* actions;
};
struct C3DDataTex {
    void (*dispose)(struct C3DDataTex* data);
    int nb_tex;
    struct C3DDTTexInfo* textures;
};

struct C3DFile get_data_compressed(struct ByteArray* ba);
struct C3DData* get_fig(struct C3DFile file);
struct C3DData* get_act(struct C3DFile file);
struct C3DData* get_tex(struct C3DFile file);

void init_anim();
int push_anim(struct ByteArray* ba, int type);
struct C3DData* get_anim(int id);
void pop_anim(int id);
void end_anim();