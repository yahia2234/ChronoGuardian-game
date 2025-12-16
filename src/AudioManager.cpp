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

  // Wall collision - plays on wall impact
  soundData.clear();
  int collisionSampleRate = sampleRate;
  if (!loadWavFile("assets/sounds/wall_hit.wav", soundData,
                   collisionSampleRate)) {
    generateSineWave(soundData, 150.0f, 0.1f, sampleRate);
    collisionSampleRate = sampleRate;
  }
  buffers[SoundEffect::WALL_COLLISION] =
      createBuffer(soundData, collisionSampleRate);
  sources[SoundEffect::WALL_COLLISION] = createSource();

  // Obstacle hit - Pendulums/Stalactites - Restored to old thud sound
  soundData.clear();
  int obstacleSampleRate = sampleRate;
  if (!loadWavFile("assets/sounds/thud_hit.wav", soundData,
                   obstacleSampleRate)) {
    // Fallback to procedural clang if thud missing
    generateSineWave(soundData, 800.0f, 0.3f, sampleRate);
    obstacleSampleRate = sampleRate;
  }
  buffers[SoundEffect::OBSTACLE_HIT] =
      createBuffer(soundData, obstacleSampleRate);
  sources[SoundEffect::OBSTACLE_HIT] = createSource();

  // Tile crack - Updated per user request to tile_cracker.wav
  soundData.clear();
  int tileCrackSampleRate = sampleRate;
  if (!loadWavFile("assets/sounds/tile_cracker.wav", soundData,
                   tileCrackSampleRate)) {
    std::cout << "Using procedural tile crack sound as fallback" << std::endl;
    generateNoiseWave(soundData, 0.2f, sampleRate);
    tileCrackSampleRate = sampleRate;
  }
  buffers[SoundEffect::TILE_CRACK] =
      createBuffer(soundData, tileCrackSampleRate);
  sources[SoundEffect::TILE_CRACK] = createSource();

  // COIN COLLECT
  soundData.clear();
  int coinSampleRate = sampleRate;
  if (!loadWavFile("assets/sounds/coin_collect.wav", soundData,
                   coinSampleRate)) {
    generateSineWave(soundData, 1200.0f, 0.4f, sampleRate); // Fallback
    coinSampleRate = sampleRate;
  }
  buffers[SoundEffect::COIN_COLLECT] = createBuffer(soundData, coinSampleRate);
  sources[SoundEffect::COIN_COLLECT] = createSource();

  // GEM COLLECT
  soundData.clear();
  int gemSampleRate = sampleRate;
  if (!loadWavFile("assets/sounds/gym_collect.wav", soundData, gemSampleRate)) {
    generateSineWave(soundData, 800.0f, 0.6f, sampleRate); // Fallback
    gemSampleRate = sampleRate;
  }
  buffers[SoundEffect::GEM_COLLECT] = createBuffer(soundData, gemSampleRate);
  sources[SoundEffect::GEM_COLLECT] = createSource();

  // Generic collect fallback (mapped to coin for now)
  buffers[SoundEffect::COLLECTIBLE_PICKUP] = buffers[SoundEffect::COIN_COLLECT];
  sources[SoundEffect::COLLECTIBLE_PICKUP] = createSource();

  // Level complete - success fanfare
  soundData.clear();
  generateSineWave(soundData, 600.0f, 0.8f, sampleRate);
  buffers[SoundEffect::LEVEL_COMPLETE] = createBuffer(soundData, sampleRate);
  sources[SoundEffect::LEVEL_COMPLETE] = createSource();

  // GAME OVER
  soundData.clear();
  int gameOverSampleRate = sampleRate;
  if (!loadWavFile("assets/sounds/game_over.wav", soundData,
                   gameOverSampleRate)) {
    generateSineWave(soundData, 100.0f, 1.0f, sampleRate);
    gameOverSampleRate = sampleRate;
  }
  buffers[SoundEffect::GAME_OVER] = createBuffer(soundData, gameOverSampleRate);
  sources[SoundEffect::GAME_OVER] = createSource();

  // WIN JINGLE
  soundData.clear();
  int winSampleRate = sampleRate;
  if (!loadWavFile("assets/sounds/win_jingle.wav", soundData, winSampleRate)) {
    generateSineWave(soundData, 800.0f, 1.5f, sampleRate);
    winSampleRate = sampleRate;
  }
  buffers[SoundEffect::WIN_JINGLE] = createBuffer(soundData, winSampleRate);
  sources[SoundEffect::WIN_JINGLE] = createSource();

  // DOOR OPEN
  soundData.clear();
  int doorSampleRate = sampleRate;
  if (!loadWavFile("assets/sounds/door_open.wav", soundData, doorSampleRate)) {
    generateNoiseWave(soundData, 1.0f, sampleRate);
    doorSampleRate = sampleRate;
  }
  buffers[SoundEffect::DOOR_OPEN] = createBuffer(soundData, doorSampleRate);
  sources[SoundEffect::DOOR_OPEN] = createSource();

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

  // Read header
  char riff[4];
  fread(riff, 1, 4, file);
  if (strncmp(riff, "RIFF", 4) != 0) {
    std::cerr << "Invalid WAV file (no RIFF): " << filename << std::endl;
    fclose(file);
    return false;
  }

  fseek(file, 4, SEEK_CUR); // Skip file size
  char wave[4];
  fread(wave, 1, 4, file);
  if (strncmp(wave, "WAVE", 4) != 0) {
    std::cerr << "Invalid WAV file (no WAVE): " << filename << std::endl;
    fclose(file);
    return false;
  }

  // Search for format and data chunks
  uint16_t audioFormat = 0;
  uint16_t numChannels = 0;
  uint32_t sampleRateValue = 0;
  uint16_t bitsPerSample = 0;
  bool formatFound = false;
  bool dataFound = false;

  char chunkId[4];
  uint32_t chunkSize;

  while (fread(chunkId, 1, 4, file) == 4) {
    fread(&chunkSize, 4, 1, file);

    // fmt chunk
    if (strncmp(chunkId, "fmt ", 4) == 0) {
      long chunkStart = ftell(file);
      fread(&audioFormat, 2, 1, file);
      fread(&numChannels, 2, 1, file);
      fread(&sampleRateValue, 4, 1, file);
      fseek(file, 6, SEEK_CUR); // Skip byte rate (4) and block align (2)
      fread(&bitsPerSample, 2, 1, file);
      formatFound = true;
      sampleRate = sampleRateValue;
      fseek(file, chunkStart + chunkSize,
            SEEK_SET); // Skip rest of chunk (extensions)
    }
    // data chunk
    else if (strncmp(chunkId, "data", 4) == 0) {
      if (!formatFound) {
        std::cerr << "WAV data chunk before format chunk: " << filename
                  << std::endl;
        break;
      }

      int numSamples = chunkSize / (bitsPerSample / 8);
      buffer.resize(numSamples);

      if (bitsPerSample == 8) {
        // 8-bit is unsigned 0..255, center at 128
        std::vector<uint8_t> tempBuffer(numSamples);
        fread(tempBuffer.data(), 1, numSamples, file);
        for (int i = 0; i < numSamples; ++i) {
          // Convert [0, 255] -> [-32768, 32767]
          buffer[i] = (static_cast<int>(tempBuffer[i]) - 128) * 256;
        }
      } else if (bitsPerSample == 16) {
        // Standard signed 16-bit
        fread(buffer.data(), sizeof(short), numSamples, file);
      } else if (bitsPerSample == 32) {
        // Assuming 32-bit float (type 3) or 32-bit PCM (type 1)
        if (audioFormat == 3) { // IEEE Float
          std::vector<float> tempBuffer(numSamples);
          fread(tempBuffer.data(), sizeof(float), numSamples, file);
          for (int i = 0; i < numSamples; ++i) {
            float val = tempBuffer[i];
            // Clamp -1.0 to 1.0
            if (val > 1.0f)
              val = 1.0f;
            if (val < -1.0f)
              val = -1.0f;
            buffer[i] = static_cast<short>(val * 32767.0f);
          }
        } else if (audioFormat == 1 ||
                   audioFormat == 65534) { // PCM (standard or extensible)
          // 32-bit integer PCM
          std::vector<int32_t> tempBuffer(numSamples);
          fread(tempBuffer.data(), sizeof(int32_t), numSamples, file);
          for (int i = 0; i < numSamples; ++i) {
            // Scale down 32-bit to 16-bit (drop lower 16 bits)
            buffer[i] = static_cast<short>(tempBuffer[i] >> 16);
          }
        } else {
          std::cerr << "Unsupported 32-bit format (" << audioFormat
                    << "): " << filename << std::endl;
          dataFound = false;
        }
      } else if (bitsPerSample == 24) {
        // 24-bit PCM: 3 bytes per sample
        int numSamples24 = chunkSize / 3;
        buffer.resize(numSamples24);
        uint8_t sampleBytes[3];
        for (int i = 0; i < numSamples24; ++i) {
          fread(sampleBytes, 1, 3, file);
          // Combine to 32-bit int, then shift down
          int32_t val =
              (sampleBytes[0]) | (sampleBytes[1] << 8) | (sampleBytes[2] << 16);
          // Sign extend if negative
          if (val & 0x800000)
            val |= 0xFF000000;
          // Convert to 16-bit
          buffer[i] = static_cast<short>(val >> 8);
        }
      } else {
        std::cerr << "Unsupported bit depth: " << bitsPerSample << " in "
                  << filename << std::endl;
        dataFound = false;
      }

      if (!buffer.empty()) {
        dataFound = true;
        std::cout << "Loaded WAV file: " << filename << " (" << bitsPerSample
                  << "-bit, " << numSamples << " samples)" << std::endl;
      }
      break; // Found data, stop searching
    } else {
      // Unknown chunk, skip it
      fseek(file, chunkSize, SEEK_CUR);
    }
  }

  fclose(file);
  return dataFound;
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
