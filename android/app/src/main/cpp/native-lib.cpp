#include <jni.h>
#include <string>
#include <core/bus.h>

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_nesemulator_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    NesEmulator::Bus bus;
    return env->NewStringUTF((hello + std::to_string(bus.GetCPURAM().size())).c_str());
}