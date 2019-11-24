#include "AndroidAppState.h"

std::unique_ptr<AndroidAppState> AndroidAppState::state = nullptr;

AndroidAppState::AndroidAppState(android_app *androidApp) :
    androidApp(androidApp),
    resourceManager(game::ResourceManager(androidApp)),
    app(game::App()),
    frame_time(0),
    last_frame_time(0),
    redraw_needed(false) {

}

AndroidAppState *AndroidAppState::createInstance(android_app *androidApp) {
    state = std::make_unique<AndroidAppState>(androidApp);

    return state.get();
}

AndroidAppState *AndroidAppState::getInstance() {
    return state ? state.get() : nullptr;
}
