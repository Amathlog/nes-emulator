#include <MyTonic.h>
#include <new_exe/audio/nesAudioSystem.h>

using NesEmulatorGL::NesAudioSystem;

NesAudioSystem::NesAudioSystem(NesEmulator::Bus& bus, unsigned nbChannels, unsigned sampleRate, unsigned bufferFrames)
    : AudioSystem(nbChannels, sampleRate, bufferFrames)
    , m_bus(bus)
{
    Tonic::setSampleRate(m_sampleRate);
}

int NesAudioSystem::RenderCallback(void *outputBuffer, void* /*inputBuffer*/, unsigned int nBufferFrames,
                    double /*streamTime*/, RtAudioStreamStatus /*status*/, void* /*userData*/)
{
    m_bus.GetSynth()->fillBufferOfFloats((float*)outputBuffer, nBufferFrames, 1);
    return 0;
}