#pragma once

#include <iostream>
#include <unordered_map>
#include <optional>

#include <filesystem> // Добавь это, если нет
namespace fs = std::filesystem; // Оставь это для удобства

typedef unsigned long DWORD;
typedef DWORD HSAMPLE;
typedef DWORD HCHANNEL;
typedef DWORD HSTREAM;

class DefaultSounds {
    std::unordered_map<std::string, fs::path> files;
public:
    DefaultSounds();
    void pushSound(const std::string& key, const fs::path& path);
    std::optional<fs::path> getSound(const std::string& key);
    std::unordered_map<std::string, fs::path> getSounds();
};

class Sound {
    HSAMPLE file;
    HCHANNEL channel;
public:
    Sound(const fs::path& path);

    ~Sound();

    HCHANNEL getChannel();

    void play();
};

class Sounds {
    std::unordered_map<std::string, std::unique_ptr<Sound>> sounds;
public:
    Sounds() = default;

    void reset();

    void load(const std::string& key, const fs::path& path);

    void setVolume(float volume);

    void playSound(const std::string& key);
};

class Music {
    HSTREAM stream;
    DWORD fxStream;
    int length;
    int pos;

    int get_length();

    double get_double_length_S();

    int get_pos();
public:
    Music() = default;

    void reset();

    void load(const fs::path& path);

    int GetPos();

    int GetLength();

    int GetLengthD_S();

    void ResetPos();

    void SetPos(double pos);

    void Play();

    void Pause();

    void Stop();

    void SetVolume(float volume);

    bool checkActive();
};

class Audio {
    Sounds sounds;
    Music music;
    float musicVolume = .1f;
    float soundsVolume = .1f;
    bool pausedAudio = false;

    DefaultSounds defaultSounds;
public:
    Audio();

    ~Audio();

    // Sounds
    void playSound(const std::string& key);

    void setSoundsVolume(float volume);

    void upSoundsVol();

    void downSoundsVol();

    float getSoundVolume();

    void loadDefault();

    // Music
    void resetPos();

    void setPos(double pos);

    void loadAudio(const fs::path& path);

    void playAudio();

    void unPauseAudio();

    void pauseAudio();

    void stopAudio();

    void setAudioVolume(float volume);

    void upAudioVol();

    void downAudioVol();

    int getMusicPos();

    int GetMusicLength();

    int GetMusicLengthD_S();

    float getMusicVolume();

    bool checkMusicIsActive();

    bool paused();
};
