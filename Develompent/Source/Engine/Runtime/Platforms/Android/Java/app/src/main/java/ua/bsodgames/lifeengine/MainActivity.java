package ua.bsodgames.lifeengine;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity
{
    static
    {
        System.loadLibrary("AndroidPlatform" );
    }

    @Override
    protected void onStart()
    {
        super.onStart();
        AndroidMain();
    }

    public native void AndroidMain();
}