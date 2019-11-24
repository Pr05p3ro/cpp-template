#ifndef CPP_TEMPLATE_ANDROID_APP_STATE_H
#define CPP_TEMPLATE_ANDROID_APP_STATE_H

#include <android_native_app_glue.h>
#include <EGL/egl.h>
#include "../App.h"

class AndroidAppState {
public:
    explicit AndroidAppState(android_app *androidApp);

    static AndroidAppState *createInstance(android_app *androidApp);

    static AndroidAppState *getInstance();

    struct android_app *androidApp;
    game::ResourceManager resourceManager;
    game::App app;
    EGLDisplay display{};
    EGLSurface surface{};
    EGLContext context{};
    int64_t frame_time;
    int64_t last_frame_time;
    bool redraw_needed;

private:
    static std::unique_ptr<AndroidAppState> state;
};

#endif //CPP_TEMPLATE_ANDROID_APP_STATE_H
