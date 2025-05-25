#include "c3d_anim.h"
/*
* Gzip decompress
*/
struct C3DFile get_data_compressed(struct ByteArray* ba) {
    struct C3DFile file = { 0,0 };
    if (((*((int*)(ba->data))) & 0xFFFF) == 0x8B1F) {
        file.data = malloc_s(0x1000);
        z_stream infstream;
        memset(&infstream, 0, sizeof(infstream));

        infstream.avail_in = ba->length;
        infstream.next_in = ba->data;

        inflateInit2(&infstream, 16 + MAX_WBITS);
        int status;
        do {
            infstream.avail_out = 0x1000;
            infstream.next_out = file.data;

            status = inflate(&infstream, Z_NO_FLUSH);

        } while (status == Z_OK);
        file.length = infstream.total_out;
        inflateEnd(&infstream);
        free(file.data);
        file.data = malloc_s(file.length);

        memset(&infstream, 0, sizeof(infstream));

        infstream.avail_in = ba->length;
        infstream.next_in = ba->data;

        inflateInit2(&infstream, 16 + MAX_WBITS);

        infstream.avail_out = file.length;
        infstream.next_out = file.data;

        inflate(&infstream, Z_NO_FLUSH);

        inflateEnd(&infstream);
    }
    else {
        file.data = malloc_s(ba->length);
        file.length = ba->length;
        memcpy(file.data, ba->data, ba->length);
    }
    return file;
}


void dispose_fig(struct C3DDataFig* fig) {
    for (int i = 0; i < fig->nb_nodes; ++i) {
        switch (fig->nodes[i]->type) {
        case 0x10:;
        case 0x11:;
            struct C3DDTNodeShape* node_11 = (struct C3DDTNodeShape*)fig->nodes[i];
            free(node_11->data);
            break;
        }
        free(fig->nodes[i]);
    }
    free(fig->nodes);
    for (int i = 0; i < fig->nb_coords; ++i) {
        if (fig->coords[i]->mat != NULL) {
            free(fig->coords[i]->mat);
        } else if (fig->coords[i]->tex != NULL) {
            free(fig->coords[i]->tex);
        }
        free(fig->coords[i]->data);
        free(fig->coords[i]);
    }
    free(fig->coords);
}
void dispose_act(struct C3DDataAct* act) {
    for (int i = 0; i < act->nb_actions; ++i) {
        for (int j = 0; j < act->actions[i].nb_parts; ++j) {
            free(act->actions[i].parts[j]->data);
            free(act->actions[i].parts[j]);
        }
        free(act->actions[i].parts);
    }
    free(act->actions);
}
void dispose_tex(struct C3DDataTex* tex) {
    for (int i = 0; i < tex->nb_tex; ++i) {
        free(tex->textures[i].data);
    }
    free(tex->textures);
}

void allocate_new(int** area, int* length, int* max) {
    ++(*length);
    if (*length > *max) {
        *max <<= 1;
        int* new_area = malloc_s((*max) * 4);
        for (int i = 0;i < *length - 1;++i) {
            new_area[i] = (*area)[i];
        }
        free(*area);
        *area = new_area;
    }
}

struct C3DData* get_fig(struct C3DFile file) {
    struct C3DDataFig* fig = malloc_s(sizeof(struct C3DDataFig));
    fig->dispose = dispose_fig;
    fig->tex_id = -1;
    fig->act_id = -1;
    fig->action = 0;
    fig->time = 0.f;
    int* data = file.data;
    int length = bigint(data[3]) + 5;
    int off = 5;
    fig->nb_coords = 0;
    int limit_coords = 2;
    fig->coords = malloc_s(limit_coords * 4);
    fig->nb_nodes = 0;
    int limit_nodes = 2;
    fig->nodes = malloc_s(limit_nodes * 4);
    FILE* fp;
    int bindtex = -1;
    while (off<length) {
        int store;
        switch (bigint(data[off]) & 0xFF) {
        case 0x29:
            off += 1;
            struct C3DDTNodeMaterial* node_29 = malloc_s(sizeof(struct C3DDTNodeMaterial));
            node_29->type = 0x29;
            for (int i = 0; i < 12; ++i) {
                *((int*)(node_29->data + i)) = bigint(data[off]);
                off += 1;
            }
            if (bindtex >= 0) {
                struct C3DDTCoords* coords = NULL;
                for (int i = 0; i < fig->nb_coords; ++i) {
                    if (fig->coords[i]->nb == bindtex) {
                        coords = fig->coords[i];
                    }
                }
                if (coords == NULL) {
                    allocate_new((int**)&fig->coords, & fig->nb_coords, & limit_coords);
                    coords = malloc_s(sizeof(struct C3DDTCoords));
                    coords->nb = bindtex;
                    coords->data = NULL;
                    coords->mat = NULL;
                    coords->tex = NULL;
                    fig->coords[fig->nb_coords - 1] = coords;
                }
                coords->mat = node_29;
            }
            else {
                allocate_new((int**)&fig->nodes, &fig->nb_nodes, &limit_nodes);
                fig->nodes[fig->nb_nodes - 1] = (struct C3DDTNode*)node_29;
            }
            break;
        case 0x44:
            store = bigint(data[off + 1]);
            struct C3DDTNodeTexture* node_44 = malloc_s(sizeof(struct C3DDTNodeTexture));
            node_44->type = 0x44;
            node_44->tex_no = store & 0xFF;
            node_44->repeat_s = (store >> 16) & 0xFF;
            node_44->repeat_t = store >> 24;
            if (bindtex >= 0) {
                struct C3DDTCoords* coords = NULL;
                for (int i = 0; i < fig->nb_coords; ++i) {
                    if (fig->coords[i]->nb == bindtex) {
                        coords = fig->coords[i];
                    }
                }
                if (coords == NULL) {
                    allocate_new((int**)&fig->coords, &fig->nb_coords, &limit_coords);
                    coords = malloc_s(sizeof(struct C3DDTCoords));
                    coords->nb = bindtex;
                    coords->data = NULL;
                    coords->mat = NULL;
                    coords->tex = NULL;
                    fig->coords[fig->nb_coords - 1] = coords;
                }
                coords->tex = node_44;
            }
            else {
                allocate_new((int**)&fig->nodes, &fig->nb_nodes, &limit_nodes);
                fig->nodes[fig->nb_nodes - 1] = (struct C3DDTNode*)node_44;
            }
            off += 2;
            break;
        case 0x10:
        case 0x11:
            off += 1;
            store = off;
            while (bigint(data[off]) > 0) {
                off += bigint(data[off]) + 1;
            }
            off += 1;
            allocate_new((int**)&fig->nodes, &fig->nb_nodes, &limit_nodes);
            struct C3DDTNodeShape* node_11 = malloc_s(sizeof(struct C3DDTNodeShape));
            node_11->type = bigint(data[store - 1]) & 0xFF;
            node_11->flags = bigint(data[store - 1]) >> 8;
            node_11->data = malloc_s(4 * (off-store));
            for (int i = 0; i < off-store; ++i) {
                node_11->data[i] = bigint(data[store + i]);
            }
            fig->nodes[fig->nb_nodes - 1] = (struct C3DDTNode*)node_11;
            break;
        case 0x36:
        case 0x38:
            store = bigint(data[off]);
            off += 1;
            allocate_new((int**)&fig->nodes, &fig->nb_nodes, &limit_nodes);
            struct C3DDTNodeFloat3* node_36 = malloc_s(sizeof(struct C3DDTNodeFloat3));
            node_36->type = store & 0xFF;
            if (store & 0x80000000) {
                node_36->type |= 0x100;
            }
            for (int i = 0; i < 3; ++i) {
                *((int*)(node_36->data + i)) = bigint(data[off]);
                off += 1;
            }
            fig->nodes[fig->nb_nodes - 1] = (struct C3DDTNode*)node_36;
            break;
        case 0x37:
            off += 1;
            allocate_new((int**)&fig->nodes, &fig->nb_nodes, &limit_nodes);
            struct C3DDTNodeFloat4* node_37 = malloc_s(sizeof(struct C3DDTNodeFloat4));
            node_37->type = bigint(data[off - 1]) & 0xFF;
            for (int i = 0; i < 4; ++i) {
                *((int*)(node_37->data + i)) = bigint(data[off]);
                off += 1;
            }
            fig->nodes[fig->nb_nodes - 1] = (struct C3DDTNode*)node_37;
            break;
        case 0x56:
            off += 1;
            store = bigint(data[off]);
            struct C3DDTCoords* coords = NULL;
            for (int i = 0; i < fig->nb_coords; ++i) {
                if (fig->coords[i]->nb == bigint(data[off - 1]) >> 24) {
                    coords = fig->coords[i];
                }
            }
            if (coords == NULL) {
                allocate_new((int**)&fig->coords, &fig->nb_coords, &limit_coords);
                coords = malloc_s(sizeof(struct C3DDTCoords));
                coords->nb = bigint(data[off - 1]) >> 24;
                coords->data = NULL;
                coords->mat = NULL;
                coords->tex = NULL;
                fig->coords[fig->nb_coords - 1] = coords;
            }
            coords->data = malloc_s(4 * store);
            off += 1;
            for (int i = 0; i < store; ++i) {
                *((int*)(coords->data + i)) = bigint(data[off]);
                off += 1;
            }
            break;
        case 0x51:
        case 0x53:
            store = bigint(data[off]);
            allocate_new((int**)&fig->nodes, &fig->nb_nodes, &limit_nodes);
            struct C3DDTNodeCoordsSet* node_53 = malloc_s(sizeof(struct C3DDTNodeCoordsSet));
            node_53->type = store & 0xFF;
            node_53->coords_no = store >> 24;
            fig->nodes[fig->nb_nodes - 1] = (struct C3DDTNode*)node_53;
            off += 1;
            break;
        case 0x54:
        case 0x47:
        case 0x42:
        case 0x43:
            store = bigint(data[off]);
            allocate_new((int**)&fig->nodes, &fig->nb_nodes, &limit_nodes);
            struct C3DDTNodeCoordsSet* node_54 = malloc_s(sizeof(struct C3DDTNodeCoordsSet));
            node_54->type = store & 0xFF;
            node_54->coords_no = store >> 16;
            fig->nodes[fig->nb_nodes - 1] = (struct C3DDTNode*)node_54;
            off += 1;
            break;
        case 0x34:
        case 0x35:
            allocate_new((int**)&fig->nodes, &fig->nb_nodes, &limit_nodes);
            struct C3DDTNode* node_34 = malloc_s(sizeof(struct C3DDTNode));
            node_34->type = bigint(data[off]) & 0xFF;
            fig->nodes[fig->nb_nodes - 1] = (struct C3DDTNode*)node_34;
            off += 1;
            break;
        case 0x50: // Start Default Texture Binding
            bindtex = bigint(data[off]) >> 24;
            off += 1;
            break;
        case 0x52: // End Texture Binding
            bindtex = -1;
            off += 1;
            break;
        case 0x39:
            store = bigint(data[off]);
            allocate_new((int**)&fig->nodes, &fig->nb_nodes, &limit_nodes);
            struct C3DDTNodeCoordsSet* node_39 = malloc_s(sizeof(struct C3DDTNodeCoordsSet));
            node_39->type = store & 0xFF;
            node_39->coords_no = store >> 8;
            fig->nodes[fig->nb_nodes - 1] = (struct C3DDTNode*)node_39;
            off += 1;
            break;
        case 0x3A:
            off += 1;
            break;
        case 0x00:
        case 0x40:
        case 0x41:
        case 0x57: // Separate Coord Figure
            off += 1;
            break;
        default:
            fp = fopen("error_model.bin", "wb");
            if (fp != NULL)
            {
                fwrite(file.data, 1, file.length, fp);
                fclose(fp);
            }
            print_flush(">>> ERROR: FIGURE Unknown mode %04X, %08X\n", bigint(data[off]) & 0xFF, off<<2);
            exit(1);
        }
    }
    free(data);
    return (struct C3DData*)fig;
}
struct C3DData* get_act(struct C3DFile file) {
    struct C3DDataAct* act = malloc_s(sizeof(struct C3DDataAct));
    act->dispose = dispose_act;
    int* data = file.data;
    FILE* fp;
    act->nb_actions = bigint(data[5]);
    if (act->nb_actions != 1) {
        fp = fopen("error_model.bin", "wb");
        if (fp != NULL)
        {
            fwrite(file.data, 1, file.length, fp);
            fclose(fp);
        }
        print_flush(">>> ERROR: ACTION Number of actions not resolved %d\n", act->nb_actions);
        exit(1);
    }
    act->actions = malloc_s(act->nb_actions * sizeof(struct C3DDTActInfo));
    int off = 6 + (act->nb_actions << 1);
    int ioff = off;
    for (int i = 0; i < act->nb_actions; ++i) {
        int loff = ioff + bigint(data[6 + (i << 1)]);
        *((int*)& (act->actions[0].duration)) = bigint(data[7 + (i << 1)]);
        act->actions[i].nb_parts = 0;
        int limit_parts = 2;
        act->actions[i].parts = malloc_s(limit_parts * 4);
        struct C3DDTActPartInfo* part = NULL;
        int store;
        int part_id;
        while (off < loff) {
            switch (bigint(data[off]) & 0xFF) {
            case 0x32:
            case 0x61:
            case 0x62:
                allocate_new((int**)&act->actions[i].parts, &act->actions[i].nb_parts, &limit_parts);
                part = malloc_s(sizeof(struct C3DDTActPartInfo));
                act->actions[i].parts[act->actions[i].nb_parts - 1] = part;
                part->id = part_id;
                part->type = bigint(data[off]) & 0xFF;
                off += 1;
                if (part->type == 0x32) {
                    part->nb_keys = 0;
                    part->duration = 0;
                    store = 16;
                }
                else {
                    if (part->type == 0x61) {
                        store = 5;
                    }
                    else {
                        store = 4;
                        if (((bigint(data[off - 1])>>16)&0xFF) != 0x1) {
                            part->type |= 0x100;
                        }
                    }
                    *((int*)&part->duration) = bigint(data[off]);
                    part->nb_keys = bigint(data[off + 1]);
                    store *= part->nb_keys;
                    off += 2;
                }
                part->data = malloc_s(4 * store);
                for (int j = 0; j < store; ++j) {
                    *((int*)(part->data + j)) = bigint(data[off]);
                    off += 1;
                }
                break;
            case 0x64:
                part_id = bigint(data[off]) >> 16;
                off += 1;
                break;
            case 0x65:
                part = NULL;
                off += 1;
                break;
            default:
                fp = fopen("error_model.bin", "wb");
                if (fp != NULL)
                {
                    fwrite(file.data, 1, file.length, fp);
                    fclose(fp);
                }
                print_flush(">>> ERROR: ACTION Unknown mode %04X, %08X\n", bigint(data[off]) & 0xFF, off << 2);
                exit(1);
            }
        }
    }
    free(data);
    return (struct C3DData*)act;
}

int color16to32(short v) {
    int c = 0;
    if (v & 0x8000) {
        c |= 0xFF000000;
    }
    c |= (v & 0x1F) << 19;
    c |= (v & 0x3E0) << 6;
    c |= (v & 0x7C00) >> 7;
    return c;
}

struct C3DData* get_tex(struct C3DFile file) {
    struct C3DDataTex* tex = malloc_s(sizeof(struct C3DDataTex));
    tex->dispose = dispose_tex;
    int* data = file.data;
    tex->nb_tex = bigint(data[5]);
    tex->textures = malloc_s((tex->nb_tex) * sizeof(struct C3DDTTexInfo));
    for (int i = 0; i < tex->nb_tex; ++i) {
        int off = bigint(data[6 + i]);
        tex->textures[i].width = bigsh(data[off + 1] & 0xFFFF);
        tex->textures[i].height = bigsh(data[off + 1] >> 16);
        tex->textures[i].data = malloc_s(tex->textures[i].width * tex->textures[i].height * 4);
        if (data[off + 2] & 0x2000) {
            short* pal_data = (short*)(data + off + 4 + ((tex->textures[i].width * tex->textures[i].height) >> ((data[off + 2] & 0x1000) ? 2 : 3)));
            for (int j = 0; j < ((tex->textures[i].width * tex->textures[i].height) >> ((data[off + 2] & 0x1000) ? 0 : 1)); ++j) {
                unsigned char v = ((char*)(data + off + 4))[j];
                if (data[off + 2] & 0x1000) {
                    tex->textures[i].data[j] = color16to32(bigsh(pal_data[v]));
                }
                else {
                    tex->textures[i].data[j * 2] = color16to32(bigsh(pal_data[v >> 4]));
                    tex->textures[i].data[j * 2 + 1] = color16to32(bigsh(pal_data[v & 0xF]));
                }
            }
        }
        else {
            short* s = (short*) (data + off + 4);
            for (int j = 0; j < tex->textures[i].width * tex->textures[i].height; ++j) {
                tex->textures[i].data[j] = color16to32(bigsh(s[j]));
            }
        }
    }
    free(data);
    return (struct C3DData*)tex;
}



/*
* Anim Handler
*/
#define ANIM_SIZE 512
static int current_id;
struct C3DData* buffer[ANIM_SIZE];

void advance_id() {
    ++current_id;
    if (current_id >= ANIM_SIZE) current_id -= ANIM_SIZE;
}

void init_anim() {
    current_id = 0;
    for (int i = 0; i < ANIM_SIZE; ++i) {
        buffer[i] = NULL;
    }
}

int push_anim(struct ByteArray* ba, int type) {
    struct C3DFile file = get_data_compressed(ba);
    while (buffer[current_id] != NULL) {
        advance_id();
    }
    struct C3DData* ele = NULL;
    switch (type) {
    case FIG:
        ele = get_fig(file);
        break;
    case ACT:
        ele = get_act(file);
        break;
    case TEX:
        ele = get_tex(file);
        break;
    }
    buffer[current_id] = ele;
    int return_id = current_id;
    advance_id();
    return return_id;
}

struct C3DData* get_anim(int id) {
    if (id < 0) {
        return NULL;
    }
    return buffer[id];
}

void pop_anim(int id) {
    buffer[id]->dispose(buffer[id]);
    free(buffer[id]);
    buffer[id] = NULL;
}

void end_anim() {
    current_id = 0;
    for (int i = 0; i < ANIM_SIZE; ++i) {
        if (buffer[i] != NULL) {
            buffer[i]->dispose(buffer[i]);
            free(buffer[i]);
            buffer[i] = NULL;
        }
    }
}