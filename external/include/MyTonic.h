#pragma once

#include <cstring>

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wimplicit-int-float-conversion"
#elif defined(__GNUC__) || defined(__GNUG__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-int-float-conversion"
#elif defined(_MSC_VER)
#pragma warning( push )
#pragma warning ( disable : 4068 4267 4244 4305 4018 )
#endif

#include <Tonic.h>

// Also add the noise generator
#include <Tonic/TonicCore.h>

class MyNoise_ : public Tonic::Tonic_::Generator_
{
    protected:
        void computeSynthesisBlock( const Tonic::Tonic_::SynthesisContext_ & context );
        unsigned nbSamplesPerRandomValue = 1;
        float volume = 0.0f;

    public:
        void setFreq(float value)
        {
            nbSamplesPerRandomValue = 0;
            if (value != 0.0f)
            {
                double sampleDuration = 1.0 / Tonic::sampleRate();
                double signalPeriod = 1.0 / value;
                nbSamplesPerRandomValue = (unsigned)(floor(signalPeriod / sampleDuration));
            }

            if (nbSamplesPerRandomValue == 0)
            {
                nbSamplesPerRandomValue = 1;
                volume = 0.0f;
            }
        }

        void setVolume(float value)
        {
            volume = value;
        }
};

inline void MyNoise_::computeSynthesisBlock( const Tonic::Tonic_::SynthesisContext_ & context )
{
    TonicFloat* fdata = &outputFrames_[0];
    float value = 0.0f;
    for (unsigned int i=0; i<outputFrames_.size(); i++)
    {
        if (i % nbSamplesPerRandomValue == 0)
            value = Tonic::randomSample() > 0.0f ? volume : -volume;
        *fdata++ = value;
    }
}

class MyNoise : public Tonic::TemplatedGenerator<MyNoise_>
{
public:
    void setFreq(float value) { gen()->setFreq(value); }
    void setVolume(float value) { gen()->setVolume(value); }
};

class MyFilter_ : public Tonic::Tonic_::Effect_
{
protected:
    void computeSynthesisBlock( const Tonic::Tonic_::SynthesisContext_ & context );
    float LP_Out = 0.0f;
    float HPA_Out = 0.0f, HPA_Prev = 0.0f;
    float HPB_Out = 0.0f, HPB_Prev = 0.0f;
};

inline void MyFilter_::computeSynthesisBlock( const Tonic::Tonic_::SynthesisContext_ & context )
{
    TonicFloat* fdata = &outputFrames_[0];
    TonicFloat* idata = &dryFrames_[0];
    for (unsigned int i=0; i<outputFrames_.size(); i++)
    {
        float LP_in = *idata++;
        LP_Out = (LP_in - LP_Out) * 0.815686f;

        HPA_Out = HPA_Out * 0.996039f + LP_Out - HPA_Prev;
        HPA_Prev = LP_Out;

        HPB_Out = HPB_Out * 0.999835f + HPA_Out - HPB_Prev;
        HPB_Prev = HPA_Out;

        *fdata++ = HPB_Out;
    }
}

class MyFilter : public Tonic::TemplatedEffect<MyFilter, MyFilter_>
{};

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__) || defined(__GNUG__)
#pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#pragma warning( pop )
#endif
