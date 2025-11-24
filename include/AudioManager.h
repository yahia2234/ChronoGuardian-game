#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif

#include <string>
#include <map>
#include <vector>

enum class SoundEffect {
    MOVEMENT,
    WALL_COLLISION,
    OBSTACLE_HIT,
    TILE_CRACK,
    COLLECTIBLE_PICKUP,
    LEVEL_COMPLETE,
    AMBIENT_HUM
};

class AudioManager {
public:
    static AudioManager& getInstance() {
        static AudioManager instance;
        return instance;
    }

    bool init();
    void cleanup();
    
    void playSound(SoundEffect effect, float volume = 1.0f, bool loop = false);
    void stopSound(SoundEffect effect);
    void stopAllSounds();
    
    void setMasterVolume(float volume);
    void generateProceduralSounds();

private:
    AudioManager();
    ~AudioManager();
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    ALCdevice* device;
    ALCcontext* context;
    
    std::map<SoundEffect, ALuint> buffers;
    std::map<SoundEffect, ALuint> sources;
    
    float masterVolume;
    
    void generateSineWave(std::vector<short>& buffer, float frequency, float duration, int sampleRate);
    void generateNoiseWave(std::vector<short>& buffer, float duration, int sampleRate);
    ALuint createBuffer(const std::vector<short>& data, int sampleRate);
    ALuint createSource();
};

#endif

