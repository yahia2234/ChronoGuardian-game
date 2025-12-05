#include "AudioManager.h"
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>

AudioManager::AudioManager()
    : device(nullptr), context(nullptr), masterVolume(0.5f) {
  srand(time(nullptr));
}

AudioManager::~AudioManager() { cleanup(); }

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
  for (auto &pair : sources) {
    alDeleteSources(1, &pair.second);
  }
  sources.clear();

  // Delete buffers
  for (auto &pair : buffers) {
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

  // Movement sound - try to load from file, fallback to procedural
  soundData.clear();
  int movementSampleRate = sampleRate;
  if (!loadWavFile("assets/sounds/movement.wav", soundData,
                   movementSampleRate)) {
    // Fallback to procedural if file not found
    std::cout << "Using procedural movement sound" << std::endl;
    generateSineWave(soundData, 80.0f, 0.5f, sampleRate);
    movementSampleRate = sampleRate;
  }
  buffers[SoundEffect::MOVEMENT] = createBuffer(soundData, movementSampleRate);
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

  // Tile crack - try to load from file, fallback to procedural
  soundData.clear();
  int tileCrackSampleRate = sampleRate;
  if (!loadWavFile("assets/sounds/tile_crack.wav", soundData,
                   tileCrackSampleRate)) {
    // Fallback to procedural if file not found
    std::cout << "Using procedural tile crack sound" << std::endl;
    generateNoiseWave(soundData, 0.2f, sampleRate);
    tileCrackSampleRate = sampleRate;
  }
  buffers[SoundEffect::TILE_CRACK] =
      createBuffer(soundData, tileCrackSampleRate);
  sources[SoundEffect::TILE_CRACK] = createSource();

  // Collectible pickup - pleasant chime
  soundData.clear();
  generateSineWave(soundData, 1200.0f, 0.4f, sampleRate);
  buffers[SoundEffect::COLLECTIBLE_PICKUP] =
      createBuffer(soundData, sampleRate);
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

void AudioManager::generateSineWave(std::vector<short> &buffer, float frequency,
                                    float duration, int sampleRate) {
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

void AudioManager::generateNoiseWave(std::vector<short> &buffer, float duration,
                                     int sampleRate) {
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

bool AudioManager::loadWavFile(const std::string &filename,
                               std::vector<short> &buffer, int &sampleRate) {
  FILE *file = fopen(filename.c_str(), "rb");
  if (!file) {
    std::cerr << "Failed to open WAV file: " << filename << std::endl;
    return false;
  }

  // Read WAV header
  char chunkId[4];
  uint32_t chunkSize;
  char format[4];

  fread(chunkId, 1, 4, file);
  fread(&chunkSize, 4, 1, file);
  fread(format, 1, 4, file);

  if (strncmp(chunkId, "RIFF", 4) != 0 || strncmp(format, "WAVE", 4) != 0) {
    std::cerr << "Invalid WAV file format" << std::endl;
    fclose(file);
    return false;
  }

  // Read fmt subchunk
  char subchunk1Id[4];
  uint32_t subchunk1Size;
  uint16_t audioFormat;
  uint16_t numChannels;
  uint32_t sampleRateValue;
  uint32_t byteRate;
  uint16_t blockAlign;
  uint16_t bitsPerSample;

  fread(subchunk1Id, 1, 4, file);
  fread(&subchunk1Size, 4, 1, file);
  fread(&audioFormat, 2, 1, file);
  fread(&numChannels, 2, 1, file);
  fread(&sampleRateValue, 4, 1, file);
  fread(&byteRate, 4, 1, file);
  fread(&blockAlign, 2, 1, file);
  fread(&bitsPerSample, 2, 1, file);

  sampleRate = sampleRateValue;

  // Read data subchunk
  char subchunk2Id[4];
  uint32_t subchunk2Size;
  fread(subchunk2Id, 1, 4, file);
  fread(&subchunk2Size, 4, 1, file);

  // Read audio data
  int numSamples = subchunk2Size / (bitsPerSample / 8);
  buffer.resize(numSamples);

  if (bitsPerSample == 16) {
    fread(buffer.data(), sizeof(short), numSamples, file);
  } else {
    std::cerr << "Unsupported bit depth: " << bitsPerSample << std::endl;
    fclose(file);
    return false;
  }

  fclose(file);
  std::cout << "Loaded WAV file: " << filename << " (" << numSamples
            << " samples)" << std::endl;
  return true;
}

ALuint AudioManager::createBuffer(const std::vector<short> &data,
                                  int sampleRate) {
  ALuint buffer;
  alGenBuffers(1, &buffer);
  alBufferData(buffer, AL_FORMAT_MONO16, data.data(),
               data.size() * sizeof(short), sampleRate);
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
  for (auto &pair : sources) {
    alSourceStop(pair.second);
  }
}
// hello
void AudioManager::setMasterVolume(float volume) {
  masterVolume = std::max(0.0f, std::min(1.0f, volume));
  for (auto &pair : sources) {
    alSourcef(pair.second, AL_GAIN, masterVolume);
  }
}
