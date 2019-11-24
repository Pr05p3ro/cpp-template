package com.fornity.template;

import android.app.NativeActivity;
import android.view.Choreographer;

public class MainActivity extends NativeActivity
        implements Choreographer.FrameCallback {

    static {
        System.loadLibrary("native-lib");
    }

    @Override
    public void doFrame(long frameTimeNanos) {
        Choreographer.getInstance().postFrameCallback(this);
        choreographerCallback(frameTimeNanos);
    }

    protected void onResume() {
        super.onResume();
        Choreographer.getInstance().postFrameCallback(this);
    }

    protected void onPause() {
        super.onPause();
        Choreographer.getInstance().removeFrameCallback(this);
    }

    protected native void choreographerCallback(long frameTimeNanos);
}