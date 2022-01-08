#include "Tonic/Generator.h"
#include "Tonic/RectWave.h"
#include "Tonic/SineWave.h"
#include "Tonic/SquareWave.h"
#include "Tonic/Synth.h"
#include "new_exe/messageService/messageService.h"
#include "new_exe/messageService/messages/screenMessage.h"
#include <MyTonic.h>
#include <new_exe/audio/nesAudioSystem.h>
#include <new_exe/screen.h>
#include <maximilian.h>
#include <chrono>

using NesEmulatorGL::NesAudioSystem;

maxiOsc mySine, mySine2;
Tonic::Synth mySynth;

NesAudioSystem::NesAudioSystem(NesEmulator::Bus& bus, bool syncWithAudio, unsigned nbChannels, unsigned sampleRate, unsigned bufferFrames)
    : AudioSystem(nbChannels, sampleRate, bufferFrames)
    , m_bus(bus)
    , m_syncWithAudio(syncWithAudio)
{
    Tonic::setSampleRate((float)m_sampleRate);
    auto f = mySynth.addParameter("123", 440.0f);
    mySynth.setOutputGen(Tonic::RectWave().freq(f));// * Tonic::SineWave().freq(1));
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
            {
                count++;
                double value = m_bus.GetAudioSample();
                ((double*)outputBuffer)[2 * count] = value;
                ((double*)outputBuffer)[2 * count + 1] = value;
            }

            if (m_bus.GetPPU().IsFrameComplete())
            {
                DispatchMessageServiceSingleton::GetInstance().Push(RenderMessage(m_bus.GetPPU().GetScreen(), m_bus.GetPPU().GetHeight() * m_bus.GetPPU().GetWidth()));
            }
        }
    }

    // m_bus.GetSynth()->fillBufferOfFloats((float*)outputBuffer, nBufferFrames, m_nbChannels);

    // static bool useTonic = false;
    // static int count = 0;
    // static double freq = 440.0;
    // static int threshhold = 4410;
    // static auto start = std::chrono::high_resolution_clock::now();

    // start = std::chrono::high_resolution_clock::now();

    // if (!useTonic)
    // {
    //     for (auto i = 0; i < nBufferFrames; ++i)
    //     {
    //         if (++count % threshhold == 0)
    //         {
    //             freq = freq == 440.0 ? 550.0 : 440.0;
    //         }

    //         float value = mySine.square(freq);// * mySine2.sinewave(1);
    //         ((float*)outputBuffer)[2*i] = value;
    //         ((float*)outputBuffer)[2*i + 1] = value;
    //     }
    // }
    // else
    // {
    //     count += nBufferFrames;
    //     if (count >= threshhold)
    //     {
    //         count -= threshhold;
    //         freq = freq == 440.0 ? 550.0 : 440.0;
    //         mySynth.setParameter("123", freq);
    //     }

    //     mySynth.fillBufferOfFloats((float*)outputBuffer, nBufferFrames, m_nbChannels);
    // }

    // auto end = std::chrono::high_resolution_clock::now();
    // std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;

    return 0;
}