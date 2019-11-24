#ifndef CPP_TEMPLATE_TYPES_H
#define CPP_TEMPLATE_TYPES_H

#include <string>
#include <GLES2/gl2.h>

namespace game {

using TextureId = GLuint;

using Entity = uint32_t;

struct Point {
    float x = 0;
    float y = 0;
};

struct Size {
    int width = 0;
    int height = 0;
};

enum TouchType {
    DOWN,
    MOVE,
    UP
};

struct DecodedImage {
    int width = 0;
    int height = 0;
    int channels = 0;
    std::string path = "";
    unsigned char *data = nullptr;
};

struct Texture {
    Size size;
    TextureId id;
};

} // namespace game

#endif //CPP_TEMPLATE_TYPES_H
