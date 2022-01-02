#include "new_exe/messageService/messageService.h"
#include "new_exe/messageService/messages/screenMessage.h"
#include <MyTonic.h>
#include <new_exe/audio/nesAudioSystem.h>

using NesEmulatorGL::NesAudioSystem;

NesAudioSystem::NesAudioSystem(NesEmulator::Bus& bus, bool syncWithAudio, unsigned nbChannels, unsigned sampleRate, unsigned bufferFrames)
    : AudioSystem(nbChannels, sampleRate, bufferFrames)
    , m_bus(bus)
    , m_syncWithAudio(syncWithAudio)
{
    Tonic::setSampleRate(m_sampleRate);
}

int NesAudioSystem::RenderCallback(void *outputBuffer, void* /*inputBuffer*/, unsigned int nBufferFrames,
                    double /*streamTime*/, RtAudioStreamStatus /*status*/, void* /*userData*/)
{
    if (m_syncWithAudio)
    {
        // Clock as much as samples are needed
        unsigned int count = 0;
        while (count < nBufferFrames)
        {
            if (m_bus.Clock())
                count++;

            // if (m_bus.GetPPU().IsFrameComplete())
            //     DispatchMessageServiceSingleton::GetInstance().Push(RenderMessage());
        }
    }
    m_bus.GetSynth()->fillBufferOfFloats((float*)outputBuffer, nBufferFrames, 1);
    return 0;
}