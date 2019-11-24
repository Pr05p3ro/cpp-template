#include <initializer_list>
#include <memory>
#include <cstdlib>
#include <cstring>
#include <jni.h>
#include <cerrno>
#include <cassert>

#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include <android_native_app_glue.h>
#include "../App.h"
#include "../Types.h"
#include "../utils/Log.h"
#include "AndroidAppState.h"
#include "../../Realm.h"

/**
 * Initialize an EGL context for the current display.
 */
static int app_init_display(AndroidAppState *state) {
    const EGLint attribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_NONE
    };

    const EGLint CONTEXT_ATTRIBS[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE
    };

    EGLint w;
    EGLint h;
    EGLint numConfigs;
    EGLConfig config;
    EGLSurface surface;
    EGLContext context;

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    eglInitialize(display, nullptr, nullptr);

    eglChooseConfig(display, attribs, nullptr, 0, &numConfigs);

    std::unique_ptr<EGLConfig[]> supportedConfigs(new EGLConfig[numConfigs]);

    assert(supportedConfigs);

    eglChooseConfig(display, attribs, supportedConfigs.get(), numConfigs, &numConfigs);

    assert(numConfigs);

    config = supportedConfigs[0];

    for (auto i = 0; i < numConfigs; i++) {
        auto &cfg = supportedConfigs[i];
        EGLint r, g, b, d;

        if (eglGetConfigAttrib(display, cfg, EGL_RED_SIZE, &r) &&
            eglGetConfigAttrib(display, cfg, EGL_GREEN_SIZE, &g) &&
            eglGetConfigAttrib(display, cfg, EGL_BLUE_SIZE, &b) &&
            eglGetConfigAttrib(display, cfg, EGL_DEPTH_SIZE, &d) &&
            r == 8 && g == 8 && b == 8 && d == 0) {

            config = supportedConfigs[i];
            break;
        }
    }

    surface = eglCreateWindowSurface(display, config, state->androidApp->window, nullptr);
    context = eglCreateContext(display, config, nullptr, CONTEXT_ATTRIBS);

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        game::Log::warn("Unable to eglMakeCurrent");
        return -1;
    }

    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);

    state->display = display;
    state->context = context;
    state->surface = surface;

    std::initializer_list<GLenum> opengl_info = {GL_VENDOR, GL_RENDERER, GL_VERSION, GL_EXTENSIONS};

    for (auto name : opengl_info) {
        auto info = glGetString(name);
        game::Log::info("OpenGL Info: %s", info);
    }

    glEnable(GL_CULL_FACE);
    glViewport(0, 0, w, h);
    glDisable(GL_DEPTH_TEST);

    state->app.onInitWindow({w, h}, state->resourceManager);

    return 0;
}

static void app_draw_frame(AndroidAppState *state) {
    if (state->display == nullptr) {
        return;
    }

    float dt = state->last_frame_time > 0
               ? (state->frame_time - state->last_frame_time) / 1000000000.0f
               : 0;

    state->last_frame_time = state->frame_time;
    state->redraw_needed = false;

    state->app.update(dt);

    eglSwapBuffers(state->display, state->surface);
}

static void app_term_display(AndroidAppState *state) {
    if (state->display != EGL_NO_DISPLAY) {
        state->app.terminateWindow();

        eglMakeCurrent(state->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

        if (state->context != EGL_NO_CONTEXT) {
            eglDestroyContext(state->display, state->context);
        }

        if (state->surface != EGL_NO_SURFACE) {
            eglDestroySurface(state->display, state->surface);
        }

        eglTerminate(state->display);
    }

    state->display = EGL_NO_DISPLAY;
    state->context = EGL_NO_CONTEXT;
    state->surface = EGL_NO_SURFACE;
}

static game::Point getEventPoint(game::App &app, AInputEvent *event, size_t index) {
    auto height = app.realm.screenSize.height;

    return {
        AMotionEvent_getX(event, index),
        height - AMotionEvent_getY(event, index)
    };
}

static int32_t app_handle_input(struct android_app *app, AInputEvent *event) {
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY) {
        if (AKeyEvent_getKeyCode(event) == AKEYCODE_BACK) {
            // actions on back key
            return 1; // <-- prevent default handler
        };
    } else if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        if (AInputEvent_getSource(event) == AINPUT_SOURCE_TOUCHSCREEN) {
            auto &app = AndroidAppState::getInstance()->app;

            int32_t action = AMotionEvent_getAction(event);
            size_t pointerCount = AMotionEvent_getPointerCount(event);

            int32_t flags = action & AMOTION_EVENT_ACTION_MASK;
            int32_t pointerIndex = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK)
                >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;

            switch (flags) {
                case AMOTION_EVENT_ACTION_DOWN:
                case AMOTION_EVENT_ACTION_POINTER_DOWN:
                    app.onTouch(
                        game::TouchType::DOWN,
                        getEventPoint(app, event, pointerIndex),
                        AMotionEvent_getPointerId(event, pointerIndex),
                        pointerCount
                    );
                    break;

                case AMOTION_EVENT_ACTION_MOVE:
                    for (uint32_t i = 0; i < pointerCount; i++) {
                        app.onTouch(
                            game::TouchType::MOVE,
                            getEventPoint(app, event, i),
                            AMotionEvent_getPointerId(event, i),
                            pointerCount
                        );
                    }
                    break;

                case AMOTION_EVENT_ACTION_UP:
                case AMOTION_EVENT_ACTION_POINTER_UP:
                    app.onTouch(
                        game::TouchType::UP,
                        getEventPoint(app, event, pointerIndex),
                        AMotionEvent_getPointerId(event, pointerIndex),
                        pointerCount
                    );
                    break;

                default:
                    break;
            }
        }

        return 1;
    }

    return 0;
}

static void app_handle_cmd(struct android_app *app, int32_t cmd) {
    auto state = (AndroidAppState *) app->userData;

    switch (cmd) {
        // TODO APP_CMD_CONFIG_CHANGED, APP_CMD_LOW_MEMORY, APP_CMD_WINDOW_RESIZED?

        case APP_CMD_INIT_WINDOW:
            if (state->androidApp->window != nullptr) {
                app_init_display(state);
            }
            break;

        case APP_CMD_TERM_WINDOW:
            app_term_display(state);
            break;

        case APP_CMD_RESUME:
            state->app.onPauseStateChanged(false);
            state->last_frame_time = 0;
            break;

        case APP_CMD_PAUSE:
            state->app.onPauseStateChanged(true);
            break;

        case APP_CMD_START:
            game::Log::info("App activity started");
            break;

        case APP_CMD_DESTROY:
            game::Log::info("App activity is going to be destroyed");
            break;

        case APP_CMD_SAVE_STATE:
            game::Log::info("State should be saved");
            break;

        default:
            break;
    }
}

bool app_draw_required(AndroidAppState *state) {
    return !state->app.isPaused() && state->redraw_needed;
}

void android_main(struct android_app *app) {
    auto appState = AndroidAppState::createInstance(app);

    app->userData = appState;
    app->onAppCmd = app_handle_cmd;
    app->onInputEvent = app_handle_input;

    while (true) {
        int events;
        struct android_poll_source *source;

        while (ALooper_pollAll(app_draw_required(appState) ? 0 : -1, nullptr, &events,
                               (void **) &source) >= 0) {
            if (source != nullptr) {
                source->process(app, source);
            }

            if (app->destroyRequested != 0) {
                app_term_display(appState);
                return;
            }
        }

        if (app_draw_required(appState)) {
            app_draw_frame(appState);
        }

    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_fornity_template_MainActivity_choreographerCallback(JNIEnv *env,
                                                            jobject instance,
                                                            jlong frameTimeNanos) {
    auto appState = AndroidAppState::getInstance();

    if (appState) {
        appState->redraw_needed = true;
        appState->frame_time = frameTimeNanos;

        ALooper_wake(appState->androidApp->looper);
    }
}