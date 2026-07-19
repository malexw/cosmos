#include <algorithm>
#include <cmath>
#include <iostream>

#include "AudioMixer.hpp"

#include "FileBlob.hpp"

bool AudioMixer::init() {
  if (!SDL_InitSubSystem(SDL_INIT_AUDIO)) {
    std::cout << "AudioMixer: SDL audio init failed: " << SDL_GetError() << std::endl;
    return false;
  }

  IPLContextSettings context_settings{};
  context_settings.version = STEAMAUDIO_VERSION;
  if (iplContextCreate(&context_settings, &context_) != IPL_STATUS_SUCCESS) {
    std::cout << "AudioMixer: failed to create Steam Audio context" << std::endl;
    shutdown();
    return false;
  }

  IPLAudioSettings audio_settings{kSampleRate, kFrameSize};
  IPLHRTFSettings hrtf_settings{};
  hrtf_settings.type = IPL_HRTFTYPE_DEFAULT;
  hrtf_settings.volume = 1.0f;
  if (iplHRTFCreate(context_, &audio_settings, &hrtf_settings, &hrtf_) != IPL_STATUS_SUCCESS) {
    std::cout << "AudioMixer: failed to create HRTF" << std::endl;
    shutdown();
    return false;
  }

  if (iplAudioBufferAllocate(context_, 1, kFrameSize, &mono_direct_) != IPL_STATUS_SUCCESS ||
      iplAudioBufferAllocate(context_, 2, kFrameSize, &stereo_out_) != IPL_STATUS_SUCCESS) {
    std::cout << "AudioMixer: failed to allocate audio buffers" << std::endl;
    shutdown();
    return false;
  }

  mix_left_.resize(kFrameSize);
  mix_right_.resize(kFrameSize);
  interleaved_.resize(kFrameSize * 2);

  SDL_AudioSpec spec{SDL_AUDIO_F32, 2, kSampleRate};
  stream_ = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec,
                                      &AudioMixer::sdl_callback, this);
  if (stream_ == nullptr) {
    std::cout << "AudioMixer: failed to open audio device: " << SDL_GetError() << std::endl;
    shutdown();
    return false;
  }
  SDL_ResumeAudioStreamDevice(stream_);

  enabled_ = true;
  return true;
}

void AudioMixer::shutdown() {
  // Destroying the stream stops the callback; everything after is single-threaded
  if (stream_ != nullptr) {
    SDL_DestroyAudioStream(stream_);
    stream_ = nullptr;
  }
  for (Voice& voice : voices_) {
    if (voice.direct != nullptr) iplDirectEffectRelease(&voice.direct);
    if (voice.binaural != nullptr) iplBinauralEffectRelease(&voice.binaural);
  }
  if (mono_direct_.data != nullptr) iplAudioBufferFree(context_, &mono_direct_);
  if (stereo_out_.data != nullptr) iplAudioBufferFree(context_, &stereo_out_);
  if (hrtf_ != nullptr) iplHRTFRelease(&hrtf_);
  if (context_ != nullptr) iplContextRelease(&context_);
  if (enabled_) {
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    enabled_ = false;
  }
}

int AudioMixer::load_clip(FileBlob& blob) {
  if (blob.size() <= 0) {
    return -1;
  }

  SDL_IOStream* io = SDL_IOFromConstMem(blob.get_bytes(), blob.size());
  SDL_AudioSpec wav_spec{};
  Uint8* wav_data = nullptr;
  Uint32 wav_len = 0;
  if (!SDL_LoadWAV_IO(io, true, &wav_spec, &wav_data, &wav_len)) {
    std::cout << "AudioMixer: WAV decode failed: " << SDL_GetError() << std::endl;
    return -1;
  }

  SDL_AudioSpec clip_spec{SDL_AUDIO_F32, 1, kSampleRate};
  Uint8* converted = nullptr;
  int converted_len = 0;
  bool ok = SDL_ConvertAudioSamples(&wav_spec, wav_data, static_cast<int>(wav_len),
                                    &clip_spec, &converted, &converted_len);
  SDL_free(wav_data);
  if (!ok) {
    std::cout << "AudioMixer: sample conversion failed: " << SDL_GetError() << std::endl;
    return -1;
  }

  Clip clip;
  const float* samples = reinterpret_cast<const float*>(converted);
  clip.samples.assign(samples, samples + converted_len / sizeof(float));
  SDL_free(converted);

  std::lock_guard<std::mutex> guard(mutex_);
  clips_.push_back(std::move(clip));
  return static_cast<int>(clips_.size()) - 1;
}

int AudioMixer::create_voice(int clip_id) {
  std::lock_guard<std::mutex> guard(mutex_);
  if (clip_id < 0 || clip_id >= static_cast<int>(clips_.size())) {
    return -1;
  }

  Voice voice;
  voice.clip = clip_id;
  if (context_ != nullptr) {
    IPLAudioSettings audio_settings{kSampleRate, kFrameSize};
    IPLDirectEffectSettings direct_settings{};
    direct_settings.numChannels = 1;
    iplDirectEffectCreate(context_, &audio_settings, &direct_settings, &voice.direct);
    IPLBinauralEffectSettings binaural_settings{};
    binaural_settings.hrtf = hrtf_;
    iplBinauralEffectCreate(context_, &audio_settings, &binaural_settings, &voice.binaural);
  }
  voices_.push_back(voice);

  VoiceWork work;
  work.samples.resize(kFrameSize);
  work_.push_back(std::move(work));
  return static_cast<int>(voices_.size()) - 1;
}

void AudioMixer::play(int voice_id) {
  std::lock_guard<std::mutex> guard(mutex_);
  if (voice_id < 0 || voice_id >= static_cast<int>(voices_.size())) return;
  voices_[voice_id].playing = true;
}

void AudioMixer::pause(int voice_id) {
  std::lock_guard<std::mutex> guard(mutex_);
  if (voice_id < 0 || voice_id >= static_cast<int>(voices_.size())) return;
  voices_[voice_id].playing = false;
}

void AudioMixer::set_gain(int voice_id, float gain) {
  std::lock_guard<std::mutex> guard(mutex_);
  if (voice_id < 0 || voice_id >= static_cast<int>(voices_.size())) return;
  voices_[voice_id].gain = gain;
}

void AudioMixer::set_looping(int voice_id, bool looping) {
  std::lock_guard<std::mutex> guard(mutex_);
  if (voice_id < 0 || voice_id >= static_cast<int>(voices_.size())) return;
  voices_[voice_id].looping = looping;
}

void AudioMixer::set_position(int voice_id, const Vector3f& pos) {
  std::lock_guard<std::mutex> guard(mutex_);
  if (voice_id < 0 || voice_id >= static_cast<int>(voices_.size())) return;
  voices_[voice_id].position = pos;
}

void AudioMixer::set_rolloff(int voice_id, float rolloff) {
  std::lock_guard<std::mutex> guard(mutex_);
  if (voice_id < 0 || voice_id >= static_cast<int>(voices_.size())) return;
  voices_[voice_id].rolloff = rolloff;
}

void AudioMixer::set_listener(const Vector3f& pos, const Quaternion& rot) {
  std::lock_guard<std::mutex> guard(mutex_);
  listener_pos_ = pos;
  listener_ahead_ = rot * Vector3f::NEGATIVE_Z;
  listener_up_ = rot * Vector3f::UNIT_Y;
}

void SDLCALL AudioMixer::sdl_callback(void* userdata, SDL_AudioStream* stream,
                                      int additional_amount, int total_amount) {
  (void)total_amount;
  AudioMixer* mixer = static_cast<AudioMixer*>(userdata);
  const int block_bytes = kFrameSize * 2 * static_cast<int>(sizeof(float));
  while (additional_amount > 0) {
    mixer->mix_block();
    SDL_PutAudioStreamData(stream, mixer->interleaved_.data(), block_bytes);
    additional_amount -= block_bytes;
  }
}

void AudioMixer::mix_block() {
  Vector3f listener_pos, listener_ahead, listener_up;
  {
    std::lock_guard<std::mutex> guard(mutex_);
    listener_pos = listener_pos_;
    listener_ahead = listener_ahead_;
    listener_up = listener_up_;

    for (std::size_t i = 0; i < voices_.size(); ++i) {
      Voice& voice = voices_[i];
      VoiceWork& work = work_[i];
      work.active = voice.playing && voice.clip >= 0 && !clips_[voice.clip].samples.empty();
      if (!work.active) continue;

      const std::vector<float>& samples = clips_[voice.clip].samples;
      const unsigned int total = static_cast<unsigned int>(samples.size());
      for (int s = 0; s < kFrameSize; ++s) {
        if (voice.playhead >= total) {
          if (voice.looping) {
            voice.playhead = 0;
          } else {
            std::fill(work.samples.begin() + s, work.samples.end(), 0.0f);
            voice.playing = false;
            voice.playhead = 0;
            break;
          }
        }
        work.samples[s] = samples[voice.playhead++];
      }
      work.gain = voice.gain;
      work.rolloff = voice.rolloff;
      work.position = voice.position;
      work.direct = voice.direct;
      work.binaural = voice.binaural;
    }
  }

  std::fill(mix_left_.begin(), mix_left_.end(), 0.0f);
  std::fill(mix_right_.begin(), mix_right_.end(), 0.0f);

  for (VoiceWork& work : work_) {
    if (!work.active) continue;

    const float dx = work.position.x() - listener_pos.x();
    const float dy = work.position.y() - listener_pos.y();
    const float dz = work.position.z() - listener_pos.z();
    const float distance = std::sqrt(dx * dx + dy * dy + dz * dz);

    // OpenAL's default inverse-distance-clamped model with reference distance 1
    const float attenuation = 1.0f / (1.0f + work.rolloff * (std::max(distance, 1.0f) - 1.0f));

    float* in_ptr = work.samples.data();
    IPLAudioBuffer in_buffer{1, kFrameSize, &in_ptr};

    IPLDirectEffectParams direct_params{};
    direct_params.flags = IPL_DIRECTEFFECTFLAGS_APPLYDISTANCEATTENUATION;
    direct_params.distanceAttenuation = attenuation;
    iplDirectEffectApply(work.direct, &direct_params, &in_buffer, &mono_direct_);

    IPLVector3 direction = iplCalculateRelativeDirection(
        context_,
        IPLVector3{work.position.x(), work.position.y(), work.position.z()},
        IPLVector3{listener_pos.x(), listener_pos.y(), listener_pos.z()},
        IPLVector3{listener_ahead.x(), listener_ahead.y(), listener_ahead.z()},
        IPLVector3{listener_up.x(), listener_up.y(), listener_up.z()});
    if (direction.x == 0.0f && direction.y == 0.0f && direction.z == 0.0f) {
      direction = IPLVector3{0.0f, 0.0f, -1.0f};  // listener inside the source: straight ahead
    }

    IPLBinauralEffectParams binaural_params{};
    binaural_params.direction = direction;
    binaural_params.interpolation = IPL_HRTFINTERPOLATION_BILINEAR;
    binaural_params.spatialBlend = 1.0f;
    binaural_params.hrtf = hrtf_;
    iplBinauralEffectApply(work.binaural, &binaural_params, &mono_direct_, &stereo_out_);

    for (int s = 0; s < kFrameSize; ++s) {
      mix_left_[s] += stereo_out_.data[0][s] * work.gain;
      mix_right_[s] += stereo_out_.data[1][s] * work.gain;
    }
  }

  for (int s = 0; s < kFrameSize; ++s) {
    interleaved_[2 * s] = mix_left_[s];
    interleaved_[2 * s + 1] = mix_right_[s];
  }
}
