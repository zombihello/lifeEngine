#include <jni.h>
#include "Core.h"
#include "OutputDevices.h"

JNI_METHOD void JNICALL Java_ua_bsodgames_lifeengine_MainActivity_AndroidMain( JNIEnv* InEnv, jobject /* this */ )
{
    debugf( TEXT( "Hello world!" ) );
}