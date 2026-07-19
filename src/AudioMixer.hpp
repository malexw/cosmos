#ifndef COSMOS_AUDIOMIXER_H_
#define COSMOS_AUDIOMIXER_H_

#include <mutex>
#include <vector>

#include <SDL3/SDL.h>
#include <phonon.h>

#include "Vector3f.hpp"
#include "Quaternion.hpp"

class FileBlob;

// Owns audio output: decoded clips, playing voices, the SDL audio stream, and
// the Steam Audio context used for spatialization. Mixing runs on SDL's audio
// thread in sdl_callback(); the main thread only sets voice/listener params.
class AudioMixer {
 public:
  static constexpr int kSampleRate = 48000;
  static constexpr int kFrameSize = 1024;

  AudioMixer() = default;
  ~AudioMixer() { shutdown(); }

  // Brings up SDL audio, Steam Audio context/HRTF and the output stream.
  // On failure the mixer stays disabled: clips and voices still work, but
  // nothing is ever rendered (engine runs fine without an audio device).
  bool init();

  // Idempotent. Destroys the SDL stream first so the callback can't touch
  // Steam Audio objects being freed after it.
  void shutdown();

  // Decodes a WAV blob to mono float at kSampleRate. Returns clip id, or -1.
  int load_clip(FileBlob& blob);

  // Creates a voice bound to a clip, with its own Steam Audio effects.
  // Voices start paused at gain 0. Returns voice id, or -1.
  int create_voice(int clip_id);

  void play(int voice_id);
  void pause(int voice_id);
  void set_gain(int voice_id, float gain);
  void set_looping(int voice_id, bool looping);
  void set_position(int voice_id, const Vector3f& pos);
  void set_rolloff(int voice_id, float rolloff);

  void set_listener(const Vector3f& pos, const Quaternion& rot);

 private:
  struct Clip {
    std::vector<float> samples;  // mono, kSampleRate
  };

  struct Voice {
    int clip = -1;
    unsigned int playhead = 0;
    bool playing = false;
    bool looping = false;
    float gain = 0.0f;
    float rolloff = 1.0f;
    Vector3f position;
    IPLDirectEffect direct = nullptr;
    IPLBinauralEffect binaural = nullptr;
  };

  // Per-voice snapshot taken under the lock each block, consumed lock-free.
  struct VoiceWork {
    std::vector<float> samples;  // kFrameSize mono, preallocated
    float gain = 0.0f;
    float rolloff = 1.0f;
    Vector3f position;
    IPLDirectEffect direct = nullptr;
    IPLBinauralEffect binaural = nullptr;
    bool active = false;
  };

  static void SDLCALL sdl_callback(void* userdata, SDL_AudioStream* stream,
                                   int additional_amount, int total_amount);
  void mix_block();

  bool enabled_ = false;
  SDL_AudioStream* stream_ = nullptr;
  IPLContext context_ = nullptr;
  IPLHRTF hrtf_ = nullptr;
  IPLAudioBuffer mono_direct_{};  // 1ch: after direct effect
  IPLAudioBuffer stereo_out_{};   // 2ch: binaural output
  std::vector<float> mix_left_;
  std::vector<float> mix_right_;
  std::vector<float> interleaved_;  // kFrameSize * 2 staging for SDL

  std::vector<Clip> clips_;
  std::vector<Voice> voices_;
  std::vector<VoiceWork> work_;
  Vector3f listener_pos_;
  Vector3f listener_ahead_ = Vector3f::NEGATIVE_Z;
  Vector3f listener_up_ = Vector3f::UNIT_Y;
  std::mutex mutex_;

  AudioMixer(const AudioMixer&) = delete;
  AudioMixer& operator=(const AudioMixer&) = delete;
};

#endif
