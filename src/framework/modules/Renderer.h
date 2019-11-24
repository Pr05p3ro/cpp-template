#ifndef CPP_TEMPLATE_RENDERER_H
#define CPP_TEMPLATE_RENDERER_H

#include <vector>
#include <string>
#include <GLES2/gl2.h>
#include <unordered_map>
#include "../Types.h"

namespace game {

struct Renderable {
    Point position = {0, 0};
    Size texSize = {0, 0};
    Size texOffset = {0, 0};
};

struct Vertex {
    GLfloat x;
    GLfloat y;
    GLfloat u;
    GLfloat v;
};

class Renderer {
public:
    Renderer();

    void startFrame();

    void endFrame();

    void add(Renderable renderable);

    void setup(Size size);

    void terminate();

    Texture genTexture(DecodedImage *img);

    void activateTex(Texture tex);

private:
    GLuint createProgram(const char *pVertexShader, const char *pFragmentShader);

    void checkGlErrors();

    void flush();

    std::vector<Renderable> renderables;

    uint32_t tail;

    GLfloat projectionMatrix[4][4];

    GLuint shaderProgram;

    GLuint aPosition;

    GLuint aTexture;

    GLuint uProjection;

    GLuint uTexture;

    Vertex vertices[43688];

    GLushort indices[65532];

    GLushort indexCount;

    GLushort vertexCount;

    std::unordered_map<std::string, Texture> textures;

    Texture activeTex;

    GLuint vertexBuffer;

    GLuint indexBuffer;

    static Texture EMPTY;
};

} // namespace game

#endif //CPP_TEMPLATE_RENDERER_H
