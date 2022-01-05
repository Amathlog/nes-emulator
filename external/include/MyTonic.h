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

    public:
        MyNoise_() { std::memset(buffer, 0, sizeof(buffer)); }
        void reset() { std::memset(buffer, 0, sizeof(buffer)); lastCall = 0; ptr = 0; }
        float buffer[2048];
        size_t ptr = 0;
        size_t lastCall = 0;
};

inline void MyNoise_::computeSynthesisBlock( const Tonic::Tonic_::SynthesisContext_ & context ){
    TonicFloat* fdata = &outputFrames_[0];
    if (lastCall + outputFrames_.size() > 2048)
    {
        std::memcpy(fdata, buffer + lastCall, 2048 - lastCall);
        lastCall = outputFrames_.size() - (2048 - lastCall);
        std::memcpy(fdata, buffer, lastCall);
    }
    else
    {
        std::memcpy(fdata, buffer + lastCall, outputFrames_.size());
        lastCall += outputFrames_.size();
    }
}

class MyNoise : public Tonic::TemplatedGenerator<MyNoise_>
{
    public:
        void setOutput(float value)
        {
            gen()->buffer[gen()->ptr] = value;
            gen()->ptr = (gen()->ptr + 1) % 2048;
        }

        void reset() { gen()->reset(); }
};

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__) || defined(__GNUG__)
#pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#pragma warning( pop )
#endif
