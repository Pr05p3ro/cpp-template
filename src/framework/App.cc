#include <GLES2/gl2.h>
#include "App.h"
#include "utils/Log.h"
#include "assets/ResourceManager.h"
#include "../Realm.h"

namespace game {

App::App() : paused(true) {
}

void App::update(float dt) {
    realm.update(dt);
}

void App::onInitWindow(Size size, ResourceManager &rm) {
    realm.setup(size);
    setPaused(false);

    Log::info("App window is initialized %dx%d", size.width, size.height);

    realm.preload(rm);
}

void App::terminateWindow() {
    setPaused(true);
    realm.terminate();

    Log::info("App window is terminated");
}

void App::onPauseStateChanged(bool paused) {
    setPaused(paused);
}

bool App::isPaused() {
    return paused;
}

void App::setPaused(bool paused) {
    this->paused = paused;

    Log::info(paused ? "App is paused" : "App is resumed");
}

void App::onTouch(TouchType type, Point point, int pointerId, int pointerCount) {
    realm.onTouch(
        type,
        {point.x * realm.scale, point.y * realm.scale},
        pointerId
    );
}

} // namespace game