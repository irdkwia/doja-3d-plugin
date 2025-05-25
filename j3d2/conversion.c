#include "conversion.h"

float ntof(int i) {
    return i / 65536.0f;
}
int fton(float i) {
    return (int)(i * 65536.0f);
}
double ntod(int i) {
    return i / 65536.0f;
}
int dton(double i) {
    return (int)(i * 65536.0f);
}
double ntor(int x) {
    return ntod(x) * PI / 180.f;
}
int rton(double x) {
    return dton(x / PI * 180.f);
}

short bigsh(short i) {
    return ((i << 8) & 0xFF00) | ((i >> 8) & 0xFF);
}
int bigint(int i) {
    return ((i << 24) & 0xFF000000) | ((i << 8) & 0xFF0000) | ((i >> 8) & 0xFF00) | ((i >> 24) & 0xFF);
}