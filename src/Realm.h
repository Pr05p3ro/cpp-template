#ifndef CPP_TEMPLATE_REALM_H
#define CPP_TEMPLATE_REALM_H

#include "framework/modules/Renderer.h"
#include "framework/assets/ResourceManager.h"
#include "framework/Types.h"

namespace game {

class Realm {
public:
    Realm();

    void preload(ResourceManager &rm);

    void update(float dt);

    void setup(Size screenSize);

    void terminate();

    Size size;

    Size screenSize;

    float scale;

    Renderer renderer;

    void onTouch(TouchType type, Point point, int pointerId);

private:
    Size imageSize;
    Texture tex;

    Point target;
};

} // namespace game

#endif //CPP_TEMPLATE_REALM_H
