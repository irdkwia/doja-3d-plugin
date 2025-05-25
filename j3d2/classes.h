#pragma once
struct String;
struct AnimImpl;
struct FrameImpl;
struct Object {
    int reserved_1;
    int reserved_2;
};
struct ByteArray {
    int reserved_1;
    int reserved_2;
    int length;
    char data[1];
};
struct IntArray {
    int reserved_1;
    int reserved_2;
    int length;
    int data[1];
};
struct ScreenUpdater {
    int reserved_1;
    int reserved_2;
};
struct Frame {
    int reserved_1;
    int reserved_2;
    int bgcolor;
    int color;
    int x;
    int y;
    int width;
    int height;
    int left;
    int top;
    int right;
    int bottom;
    int border_width;
    int border;
    struct Frame* parent;
    struct String* softLabel1;
    struct String* softLabel2;
    int softLabelShown;
    struct FrameImpl* impl;
    struct ScreenUpdater* screenUpdater;
};
struct Font {
    int reserved_1;
    int reserved_2;
    int face;
    int size;
    int style;
};
struct Graphics {
    int reserved_1;
    int reserved_2;
    struct Frame* owner;
    int** id;
    int pictoColor;
    struct Object* finalizer;
    struct Font* initFont;
    int disposed3d;
    struct Frame* child_owner;
    int isGraphics;
    struct Object* child_finalizer;
    int change_inst_flg;
};
struct FrameImpl {
    int reserved_1;
    int reserved_2;
    struct Frame* frame;
    struct Graphics* graphics;
};
struct AnimImpl {
    int reserved_1;
    int reserved_2;
    struct Object* finalizer;
    int id;
    int model_type;
};
struct Texture {
    int reserved_1;
    int reserved_2;
    int hwnd;
    int disposed;
    struct Object* mutex;
    struct AnimImpl* impl;
};
struct ActionTable {
    int reserved_1;
    int reserved_2;
    int hwnd;
    int naction;
    int disposed;
    struct Object* mutex;
    struct AnimImpl* impl;
};
struct Figure {
    int reserved_1;
    int reserved_2;
    int hwnd;
    int disposed;
    struct Object* mutex;
    struct AnimImpl* impl;
};

typedef struct {
    int* stack_ptr;
} vm_env;