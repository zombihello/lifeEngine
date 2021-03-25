#include <jni.h>
#include <string>
#include <stdio.h>
#include "Core.h"


extern "C" JNIEXPORT jstring JNICALL
Java_ua_bsodgames_lifeengine_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {

    std::string hello = "Hello from C++ lifeEngine";
    return env->NewStringUTF(hello.c_str());
}