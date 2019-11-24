#ifndef CPP_TEMPLATE_APP_H
#define CPP_TEMPLATE_APP_H

#include <memory>
#include <vector>
#include <android_native_app_glue.h>
#include "../Realm.h"
#include "assets/ResourceManager.h"

namespace game {

class App {
public:
    App();

    void update(float dt);

    void onInitWindow(Size size, ResourceManager &rm);

    void terminateWindow();

    void onPauseStateChanged(bool paused);

    bool isPaused();

    void setPaused(bool paused);

    void onTouch(TouchType type, Point point, int pointerId, int pointersCount);

    Realm realm;

private:
    bool paused;
};

} // namespace game

#endif //CPP_TEMPLATE_APP_H
