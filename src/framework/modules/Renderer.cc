#include "Renderer.h"
#include "../utils/Log.h"

namespace game {

Texture Renderer::EMPTY = {{0, 0}, 0};

Renderer::Renderer() :
    tail(0),
    projectionMatrix(),
    shaderProgram(0),
    aPosition(0),
    aTexture(0),
    uProjection(0),
    uTexture(0),
    vertices(),
    indices(),
    indexCount(0),
    vertexCount(0),
    vertexBuffer(0),
    indexBuffer(0) {

}

void Renderer::add(Renderable renderable) {
    if (tail < renderables.size()) {
        renderables[tail] = renderable;
    } else {
        renderables.push_back(renderable);
    }

    ++tail;
}

void Renderer::startFrame() {
    glClearColor(0.25F, 0.25F, 0.25F, 1);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::endFrame() {
    flush();

    activateTex(EMPTY);
}

void Renderer::activateTex(Texture texture) {
    if (activeTex.id != texture.id) {
        flush();

        activeTex = texture;

        glBindTexture(GL_TEXTURE_2D, activeTex.id);
    }
}

void Renderer::flush() {
    for (uint32_t i = 0; i < tail; i++) {
        GLushort *index = &indices[indexCount];

        index[0] = static_cast<GLushort>(vertexCount + 0);
        index[1] = static_cast<GLushort>(vertexCount + 1);
        index[2] = static_cast<GLushort>(vertexCount + 2);
        index[3] = static_cast<GLushort>(vertexCount + 2);
        index[4] = static_cast<GLushort>(vertexCount + 1);
        index[5] = static_cast<GLushort>(vertexCount + 3);

        GLfloat posX1 = renderables[i].position.x;
        GLfloat posY1 = renderables[i].position.y;
        GLfloat posX2 = posX1 + renderables[i].texSize.width;
        GLfloat posY2 = posY1 + renderables[i].texSize.height;

        GLfloat u1 = renderables[i].texOffset.width / static_cast<float>(activeTex.size.width);
        GLfloat u2 = u1 + renderables[i].texSize.width / static_cast<float>(activeTex.size.width);
        GLfloat v1 = renderables[i].texOffset.height / static_cast<float>(activeTex.size.height);
        GLfloat v2 = v1 + renderables[i].texSize.height / static_cast<float>(activeTex.size.height);

        Vertex *vertex = &vertices[vertexCount];

        vertex[0] = {posX1, posY1, u1, v1};
        vertex[1] = {posX2, posY1, u2, v1};
        vertex[2] = {posX1, posY2, u1, v2};
        vertex[3] = {posX2, posY2, u2, v2};

        indexCount += 6;
        vertexCount += 4;

        if (indexCount >= 65526 || i == tail - 1) {
            glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
            glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex), &vertices[0], GL_DYNAMIC_DRAW);

            glVertexAttribPointer(aPosition, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
            glVertexAttribPointer(aTexture, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(GLfloat) * 2));

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(GLushort), &indices[0], GL_DYNAMIC_DRAW);

            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, (void*)0);

            checkGlErrors();

            indexCount = 0;
            vertexCount = 0;
        }
    }

    tail = 0;
}

void Renderer::setup(Size size) {
    auto right = GLfloat(size.width);
    auto top = GLfloat(size.height);

    GLfloat left = 0.0F;
    GLfloat bottom = 0.0F;
    GLfloat near = -1.0F;
    GLfloat far = 1.0F;

    projectionMatrix[0][0] = 2.0F / (right - left);
    projectionMatrix[0][1] = 0.0F;
    projectionMatrix[0][2] = 0.0F;
    projectionMatrix[0][3] = 0.0F;
    projectionMatrix[1][0] = 0.0F;
    projectionMatrix[1][1] = 2.0F / (top - bottom);
    projectionMatrix[1][2] = 0.0F;
    projectionMatrix[1][3] = 0.0F;
    projectionMatrix[2][0] = 0.0F;
    projectionMatrix[2][1] = 0.0F;
    projectionMatrix[2][2] = 2.0F / (near - far);
    projectionMatrix[2][3] = 0.0F;
    projectionMatrix[3][0] = (left + right) / (left - right);
    projectionMatrix[3][1] = (top + bottom) / (bottom - top);
    projectionMatrix[3][2] = (near + far) / (near - far);
    projectionMatrix[3][3] = 1.0F;

    const char *VERTEX_SHADER =
        "attribute vec4 aPosition;\n"
        "attribute vec2 aTexture;\n"
        "varying vec2 vTexture;\n"
        "uniform mat4 uProjection;\n"
        "void main() {\n"
        "    vTexture = aTexture;\n"
        "    gl_Position = uProjection * aPosition;\n"
        "}";

    const char *FRAGMENT_SHADER =
        "precision mediump float;\n"
        "varying vec2 vTexture;\n"
        "uniform sampler2D u_texture;\n"
        "void main() {\n"
        "  gl_FragColor = texture2D(u_texture, vTexture);\n"
        "}";

    shaderProgram = createProgram(VERTEX_SHADER, FRAGMENT_SHADER);

    aPosition = static_cast<GLuint>(glGetAttribLocation(shaderProgram, "aPosition"));
    aTexture = static_cast<GLuint>(glGetAttribLocation(shaderProgram, "aTexture"));
    uProjection = static_cast<GLuint>(glGetUniformLocation(shaderProgram, "uProjection"));
    uTexture = static_cast<GLuint>(glGetUniformLocation(shaderProgram, "u_texture"));

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(shaderProgram);

    glUniformMatrix4fv(uProjection, 1, GL_FALSE, &projectionMatrix[0][0]);
    glUniform1i(uTexture, 0);

    glEnableVertexAttribArray(aPosition);
    glEnableVertexAttribArray(aTexture);

    glGenBuffers(1, &vertexBuffer);
    glGenBuffers(1, &indexBuffer);

    glActiveTexture(GL_TEXTURE0);

    checkGlErrors();

    Log::info("pos %d tex %d uproj %d utex %d", aPosition, aTexture, uProjection, uTexture);
}

void Renderer::terminate() {
    glDisableVertexAttribArray(aPosition);
    glDisableVertexAttribArray(aTexture);

    for (auto &it: textures) {
        glDeleteTextures(1, &it.second.id);
    }

    textures.clear();

    glDisable(GL_BLEND);

    glUseProgram(0);
    glDeleteProgram(shaderProgram);
}

GLuint Renderer::createProgram(const char *pVertexShader, const char *pFragmentShader) {
    GLint result;

    char log[256];

    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint shaderProgram;

    // Builds the vertex shader.
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexShader, 1, &pVertexShader, nullptr);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);

    if (result == GL_FALSE) {
        glGetShaderInfoLog(vertexShader, sizeof(log), nullptr, log);
        Log::error("Vertex shader error: %s", log);

        if (vertexShader > 0) glDeleteShader(vertexShader);

        return 0;
    }

    // Builds the fragment shader.
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &pFragmentShader, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);

    if (result == GL_FALSE) {
        glGetShaderInfoLog(fragmentShader, sizeof(log), nullptr, log);
        Log::error("Fragment shader error: %s", log);

        if (vertexShader > 0) glDeleteShader(vertexShader);
        if (fragmentShader > 0) glDeleteShader(fragmentShader);

        return 0;
    }

    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &result);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    if (result == GL_FALSE) {
        glGetProgramInfoLog(shaderProgram, sizeof(log), nullptr, log);
        Log::error("Shader program error: %s", log);

        if (shaderProgram > 0) glDeleteProgram(shaderProgram);

        return 0;
    }

    Log::info("Shader program is created %d", shaderProgram);

    return shaderProgram;
}

void Renderer::checkGlErrors() {
    GLenum errorResult = glGetError();

    if (errorResult != GL_NO_ERROR) {
        Log::error("GL error occurred %d", errorResult);
    };
}

Texture Renderer::genTexture(DecodedImage *img) {
    auto it = textures.find(img->path);

    if (it != textures.end()) {
        return it->second;
    }

    TextureId textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                    GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                    GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->width, img->height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, img->data);

    glBindTexture(GL_TEXTURE_2D, 0);

    checkGlErrors();

    Texture tex = {{img->width, img->height}, textureId};

    textures.insert(std::make_pair(img->path, tex));

    Log::info("Texture %s %dx%d is created with id %d", img->path.c_str(), img->width, img->height,
              textureId);

    return tex;
}

} // namespace game
