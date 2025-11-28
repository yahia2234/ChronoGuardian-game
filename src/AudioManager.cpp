#include "AudioManager.h"
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>

AudioManager::AudioManager() 
    : device(nullptr), context(nullptr), masterVolume(0.5f) {
    srand(time(nullptr));
}

AudioManager::~AudioManager() {
    cleanup();
}

bool AudioManager::init() {
    // Open default audio device
    device = alcOpenDevice(nullptr);
    if (!device) {
        std::cerr << "Failed to open audio device" << std::endl;
        return false;
    }

    // Create audio context
    context = alcCreateContext(device, nullptr);
    if (!context) {
        std::cerr << "Failed to create audio context" << std::endl;
        alcCloseDevice(device);
        return false;
    }

    alcMakeContextCurrent(context);

    // Check for errors
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        std::cerr << "OpenAL error: " << error << std::endl;
        return false;
    }

    // Generate procedural sounds
    generateProceduralSounds();

    std::cout << "Audio system initialized successfully" << std::endl;
    return true;
}

void AudioManager::cleanup() {
    // Stop all sounds
    stopAllSounds();

    // Delete sources
    for (auto& pair : sources) {
        alDeleteSources(1, &pair.second);
    }
    sources.clear();

    // Delete buffers
    for (auto& pair : buffers) {
        alDeleteBuffers(1, &pair.second);
    }
    buffers.clear();

    // Cleanup context and device
    if (context) {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(context);
        context = nullptr;
    }

    if (device) {
        alcCloseDevice(device);
        device = nullptr;
    }
}

void AudioManager::generateProceduralSounds() {
    int sampleRate = 22050;
    std::vector<short> soundData;

    // Movement hover sound - low frequency hum
    soundData.clear();
    generateSineWave(soundData, 80.0f, 0.5f, sampleRate);
    buffers[SoundEffect::MOVEMENT] = createBuffer(soundData, sampleRate);
    sources[SoundEffect::MOVEMENT] = createSource();

    // Wall collision - short thud
    soundData.clear();
    generateSineWave(soundData, 150.0f, 0.1f, sampleRate);
    buffers[SoundEffect::WALL_COLLISION] = createBuffer(soundData, sampleRate);
    sources[SoundEffect::WALL_COLLISION] = createSource();

    // Obstacle hit - metallic clang
    soundData.clear();
    generateSineWave(soundData, 800.0f, 0.3f, sampleRate);
    buffers[SoundEffect::OBSTACLE_HIT] = createBuffer(soundData, sampleRate);
    sources[SoundEffect::OBSTACLE_HIT] = createSource();

    // Tile crack - noise burst
    soundData.clear();
    generateNoiseWave(soundData, 0.2f, sampleRate);
    buffers[SoundEffect::TILE_CRACK] = createBuffer(soundData, sampleRate);
    sources[SoundEffect::TILE_CRACK] = createSource();

    // Collectible pickup - pleasant chime
    soundData.clear();
    generateSineWave(soundData, 1200.0f, 0.4f, sampleRate);
    buffers[SoundEffect::COLLECTIBLE_PICKUP] = createBuffer(soundData, sampleRate);
    sources[SoundEffect::COLLECTIBLE_PICKUP] = createSource();

    // Level complete - success fanfare
    soundData.clear();
    generateSineWave(soundData, 600.0f, 0.8f, sampleRate);
    buffers[SoundEffect::LEVEL_COMPLETE] = createBuffer(soundData, sampleRate);
    sources[SoundEffect::LEVEL_COMPLETE] = createSource();

    // Ambient hum
    soundData.clear();
    generateSineWave(soundData, 60.0f, 1.0f, sampleRate);
    buffers[SoundEffect::AMBIENT_HUM] = createBuffer(soundData, sampleRate);
    sources[SoundEffect::AMBIENT_HUM] = createSource();
}

void AudioManager::generateSineWave(std::vector<short>& buffer, float frequency, float duration, int sampleRate) {
    int numSamples = static_cast<int>(duration * sampleRate);
    buffer.resize(numSamples);

    for (int i = 0; i < numSamples; i++) {
        float t = static_cast<float>(i) / sampleRate;
        float value = sin(2.0f * M_PI * frequency * t);
        
        // Apply envelope (fade in/out)
        float envelope = 1.0f;
        float fadeTime = 0.05f;
        if (t < fadeTime) {
            envelope = t / fadeTime;
        } else if (t > duration - fadeTime) {
            envelope = (duration - t) / fadeTime;
        }
        
        buffer[i] = static_cast<short>(value * envelope * 32767.0f * 0.3f);
    }
}

void AudioManager::generateNoiseWave(std::vector<short>& buffer, float duration, int sampleRate) {
    int numSamples = static_cast<int>(duration * sampleRate);
    buffer.resize(numSamples);

    for (int i = 0; i < numSamples; i++) {
        float t = static_cast<float>(i) / sampleRate;
        float value = (rand() / static_cast<float>(RAND_MAX)) * 2.0f - 1.0f;
        
        // Apply envelope
        float envelope = 1.0f - (t / duration);
        buffer[i] = static_cast<short>(value * envelope * 32767.0f * 0.2f);
    }
}

ALuint AudioManager::createBuffer(const std::vector<short>& data, int sampleRate) {
    ALuint buffer;
    alGenBuffers(1, &buffer);
    alBufferData(buffer, AL_FORMAT_MONO16, data.data(), data.size() * sizeof(short), sampleRate);
    return buffer;
}

ALuint AudioManager::createSource() {
    ALuint source;
    alGenSources(1, &source);
    alSourcef(source, AL_PITCH, 1.0f);
    alSourcef(source, AL_GAIN, masterVolume);
    alSource3f(source, AL_POSITION, 0.0f, 0.0f, 0.0f);
    alSource3f(source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    alSourcei(source, AL_LOOPING, AL_FALSE);
    return source;
}

void AudioManager::playSound(SoundEffect effect, float volume, bool loop) {
    auto sourceIt = sources.find(effect);
    auto bufferIt = buffers.find(effect);
    
    if (sourceIt == sources.end() || bufferIt == buffers.end()) {
        return;
    }

    ALuint source = sourceIt->second;
    ALuint buffer = bufferIt->second;

    // Stop if already playing
    alSourceStop(source);

    // Set buffer and properties
    alSourcei(source, AL_BUFFER, buffer);
    alSourcef(source, AL_GAIN, volume * masterVolume);
    alSourcei(source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);

    // Play
    alSourcePlay(source);
}

void AudioManager::stopSound(SoundEffect effect) {
    auto it = sources.find(effect);
    if (it != sources.end()) {
        alSourceStop(it->second);
    }
}

void AudioManager::stopAllSounds() {
    for (auto& pair : sources) {
        alSourceStop(pair.second);
    }
}
//hello
void AudioManager::setMasterVolume(float volume) {
    masterVolume = std::max(0.0f, std::min(1.0f, volume));
    for (auto& pair : sources) {
        alSourcef(pair.second, AL_GAIN, masterVolume);
    }
}

