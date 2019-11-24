#include "Realm.h"
#include "Config.h"
#include "framework/utils/Log.h"

namespace game {

Realm::Realm() :
    scale(0),
    imageSize({324, 300}) {

}

void Realm::preload(ResourceManager &rm) {
    tex = renderer.genTexture(rm.getImage("example.png"));
}

void Realm::setup(Size screenSize) {
    this->screenSize = screenSize;

    scale = float(DESIGN_WIDTH) / float(screenSize.width);
    size = {DESIGN_WIDTH, static_cast<int>(round(screenSize.height * scale))};

    if (target.x == 0 && target.y == 0) {
        target = {size.width / 2.0F, size.height / 2.0F};
    }

    renderer.setup(size);
}

void Realm::terminate() {
    renderer.terminate();
}

void Realm::update(float dt) {
    renderer.startFrame();

    renderer.activateTex(tex);

    renderer.add({
        {target.x - imageSize.width / 2, target.y - imageSize.height / 2},
        {imageSize.width, imageSize.height}
    });

    target.y = fmax(target.y - dt * 100.0F, imageSize.height / 2.0F);

    renderer.endFrame();
}

void Realm::onTouch(TouchType type, Point point, int pointerId) {
    if (type == TouchType::DOWN) {
        target = point;
    }
}

} // namespace game