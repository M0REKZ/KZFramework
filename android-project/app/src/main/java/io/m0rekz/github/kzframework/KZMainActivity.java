/*
    KZFramework
    Copyright (C) Benjamín Gajardo

    This program is shared under the PLUSKAIZO LICENSE, you should have received
    a copy of the license along with the program.
*/

package io.m0rekz.github.kzframework;

import android.view.View;

import org.libsdl.app.SDLActivity;

public class KZMainActivity extends SDLActivity
{
    @Override
    public void onWindowFocusChanged(boolean hasFocus)
    {
        super.onWindowFocusChanged(hasFocus);

        if (hasFocus)
        {
            getWindow().getDecorView().setSystemUiVisibility(
                    View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY |
                    View.SYSTEM_UI_FLAG_FULLSCREEN |
                    View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
            );
        }
    }
}
