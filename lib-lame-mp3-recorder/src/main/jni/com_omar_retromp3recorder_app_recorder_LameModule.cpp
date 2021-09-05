#include "com_omar_retromp3recorder_app_recorder_LameModule.h"
#include "libmp3lame/lame.h"
#include <new>
#include <iostream>


static lame_global_flags *glf = NULL;

void rethrow_cpp_exception_as_java_exception(JNIEnv *env) {
    try {
        throw; // This allows to determine the type of the exception
    }
    catch (const std::bad_alloc &e) {
        jclass jc = env->FindClass("java/lang/OutOfMemoryError");
        if (jc) env->ThrowNew(jc, e.what());
    }
    catch (const std::ios_base::failure &e) {
        jclass jc = env->FindClass("java/io/IOException");
        if (jc) env->ThrowNew(jc, e.what());
    }
    catch (const std::exception &e) {
        /* unknown exception (may derive from std::exception) */
        jclass jc = env->FindClass("java/lang/Error");
        if (jc) env->ThrowNew(jc, e.what());
    }
    catch (...) {
        /* Oops I missed identifying this exception! */
        jclass jc = env->FindClass("java/lang/Error");
        if (jc)
            env->ThrowNew(jc, "Unidentified exception => "
                              "Improve rethrow_cpp_exception_as_java_exception()");
    }
}

extern "C"
JNIEXPORT void JNICALL Java_com_omar_retromp3recorder_app_recorder_LameModule_init(
        JNIEnv *env, jclass cls, jint inSamplerate, jint outChannel,
        jint outSamplerate, jint outBitrate, jint quality) {
    if (glf != NULL) {
        lame_close(glf);
        glf = NULL;
    }
    glf = lame_init();
    lame_set_in_samplerate(glf, inSamplerate);
    lame_set_num_channels(glf, outChannel);
    lame_set_out_samplerate(glf, outSamplerate);
    lame_set_brate(glf, outBitrate);
    lame_set_quality(glf, quality);
    lame_init_params(glf);
}

extern "C"
JNIEXPORT jint JNICALL Java_com_omar_retromp3recorder_app_recorder_LameModule_encode(
        JNIEnv *env, jclass cls, jshortArray buffer_l, jshortArray buffer_r,
        jint samples, jbyteArray mp3buf) {
    try {
        jshort *j_buffer_l = env->GetShortArrayElements(buffer_l, 0);

        jshort *j_buffer_r = env->GetShortArrayElements(buffer_r, 0);

        const jsize mp3buf_size = env->GetArrayLength(mp3buf);
        jbyte *j_mp3buf = env->GetByteArrayElements(mp3buf, 0);

        int result = lame_encode_buffer(glf, j_buffer_l, j_buffer_r,
                                        samples, (unsigned char *) j_mp3buf, mp3buf_size);

        env->ReleaseShortArrayElements(buffer_l, j_buffer_l, 0);
        env->ReleaseShortArrayElements(buffer_r, j_buffer_r, 0);
        env->ReleaseByteArrayElements(mp3buf, j_mp3buf, 0);

        return result;
    } catch (...) {
        rethrow_cpp_exception_as_java_exception(env);
        return 0;
    }
}

extern "C"
JNIEXPORT jint JNICALL Java_com_omar_retromp3recorder_app_recorder_LameModule_flush(
        JNIEnv *env, jclass cls, jbyteArray mp3buf) {
    try {
        const jsize mp3buf_size = env->GetArrayLength(mp3buf);
        jbyte *j_mp3buf = env->GetByteArrayElements(mp3buf, 0);

        int result = lame_encode_flush(glf, (unsigned char *) j_mp3buf, mp3buf_size);

        env->ReleaseByteArrayElements(mp3buf, j_mp3buf, 0);

        return result;
    } catch (...) {
        rethrow_cpp_exception_as_java_exception(env);
        return 0;
    }
}

extern "C"
JNIEXPORT void JNICALL Java_com_omar_retromp3recorder_app_recorder_LameModule_close(
        JNIEnv *env, jclass cls) {
    try {
        lame_close(glf);
    } catch (...) {
        rethrow_cpp_exception_as_java_exception(env);
    }
    glf = NULL;
}


