#pragma once

#include <string>
#include <map>
#include <vector>
#include "AL/al.h"
#include "AL/alc.h"

namespace KM
{
    class SoundSource
    {
    public:
        void Init();
        void SetBuffer(ALuint buffer);
        void Play();
        void Stop();
        void SetVolume(float volume);
        void SetPitch(float pitch);
        void Close();
    public:
        ALuint handle;
    };

    class SoundManager
    {
    public:
        void Init();
        void Tick();
        void Play(const std::string& name, float volume, float pitch);
        void Close();

        bool initialized { false };
        ALCdevice* device { nullptr };
        ALCcontext* context { nullptr };

        std::map<std::string, ALuint> buffers;
    };
}