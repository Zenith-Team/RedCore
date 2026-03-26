#include <cmath>

extern "C" double floor(double x) {
    return floorf(x);
}

extern "C" double ceil(double x) {
    return ceilf(x);
}

extern "C" double sin(double x) {
    return sinf(x);
}

extern "C" double pow(double x, double y) {
    return powf(x, y);
}

extern "C" double log(double x) {
    return logf(x);
}

extern "C" double cos(double x) {
    return cosf(x);
}

extern "C" double atan2(double y, double x) {
    return atan2f(y, x);
}
