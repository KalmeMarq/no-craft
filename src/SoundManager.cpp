#include "SoundManager.hpp"

namespace KM
{
    void SoundManager::Init()
    {
        this->device = alcOpenDevice(nullptr);
        this->context = alcCreateContext(this->device, nullptr);
        alcMakeContextCurrent(this->context);
        this->initialized = true;
    }

    void SoundManager::Tick()
    {
    }

    void SoundManager::Play(const std::string &name, float volume, float pitch)
    {
    }

    void SoundManager::Close()
    {
        if (this->initialized)
        {
            alcMakeContextCurrent(nullptr);
            alcDestroyContext(this->context);
            alcCloseDevice(this->device);
        }
    }
    
    void SoundSource::Init()
    {
        alGenSources(1, &this->handle);
    }

    void SoundSource::SetBuffer(ALuint buffer)
    {
        alSourcei(this->handle, AL_BUFFER, buffer);
    }

    void SoundSource::Play()
    {
        alSourcePlay(this->handle);
    }

    void SoundSource::Stop()
    {
        alSourceStop(this->handle);
    }

    void SoundSource::SetVolume(float volume)
    {
        alSourcef(this->handle, AL_GAIN, volume);
    }

    void SoundSource::SetPitch(float pitch)
    {
        alSourcef(this->handle, AL_PITCH, pitch);
    }

    void SoundSource::Close()
    {
        alDeleteSources(1, &this->handle);
    }
}