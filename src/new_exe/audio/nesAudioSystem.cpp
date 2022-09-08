#include "new_exe/messageService/messageService.h"
#include "new_exe/messageService/messages/screenMessage.h"
#include <MyTonic.h>
#include <new_exe/audio/nesAudioSystem.h>
#include <new_exe/screen.h>
#include <core/constants.h>

using NesEmulatorGL::NesAudioSystem;

NesAudioSystem::NesAudioSystem(NesEmulator::Bus& bus, bool syncWithAudio, unsigned nbChannels, unsigned bufferFrames)
    : AudioSystem(nbChannels, bufferFrames)
    , m_bus(bus)
    , m_syncWithAudio(syncWithAudio)
{
    Tonic::setSampleRate((float)NesEmulator::Cst::SAMPLE_RATE);
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

            if (m_bus.GetPPU().IsFrameComplete())
            {
                DispatchMessageServiceSingleton::GetInstance().Push(RenderMessage(m_bus.GetPPU().GetScreen(), m_bus.GetPPU().GetHeight() * m_bus.GetPPU().GetWidth()));
            }
        }
    }

    m_bus.GetAPU().FillSamples((float*)outputBuffer, nBufferFrames, m_nbChannels);

    return 0;
}