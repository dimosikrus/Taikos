#pragma once

#include <iostream>
#include <optional>
#include <unordered_map>
#include <memory>

#include <bass/bass.h>
#include <bass_fx/bass_fx.h>
#include "../filesystem/filesystem.hpp"

class DefaultSounds {
    std::unordered_map<std::string, fs::path> files;
public:
    DefaultSounds() {
        pushSound("hitZ", fs::path("assets\\hitsound1.wav"));
        pushSound("hitX", fs::path("assets\\hitsound2.wav"));
    }
    void pushSound(const std::string& key, const fs::path& path) {
        if(chechFileExist(path))
          files.emplace(key, path);
    }
    std::optional<fs::path> getSound(const std::string& key) {
        if(files.find(key) != files.end()) {
            return files.at(key);
        };
        return std::nullopt;
    }
    std::unordered_map<std::string, fs::path> getSounds() {
        return files;
    }
};

class Sound {
    HSAMPLE file;
    HCHANNEL channel;
public:
    Sound(const fs::path& path) {
        if (!chechFileExist(path)) 
            std::cerr << "Error load Sound: " << path << '\n';
        file = BASS_SampleLoad(FALSE, path.c_str(), 0, 0, 3, BASS_SAMPLE_OVER_POS);
        channel = BASS_SampleGetChannel(file, FALSE);
    }
    ~Sound() { BASS_SampleFree(file); }
    HCHANNEL getChannel() {
        return channel;
    }
    void play() {
        BASS_ChannelPlay(channel, TRUE);
    }
};

class Sounds {
    std::unordered_map<std::string, std::unique_ptr<Sound>> sounds;
public:
    Sounds() = default;
    void reset() {
        sounds.clear();
    }
    void load(const std::string& key, const fs::path& path) {
        sounds.emplace(key, std::make_unique<Sound>(path));
    }
    void setVolume(float volume) {
        for (auto& [_, sound] : sounds) {
            BASS_ChannelSetAttribute(sound->getChannel(), BASS_ATTRIB_VOL, volume);
        }
    }
    void playSound(const std::string& key) {
        if(sounds.find(key) != sounds.end()) {
            sounds.at(key)->play();
        };
    }
};

class Music {
    HSTREAM stream;
    DWORD fxStream;
    void getLength() {
        length = static_cast<int>(
            BASS_ChannelBytes2Seconds(
                stream,
                BASS_ChannelGetLength(
                    stream,
                    BASS_POS_BYTE
                )
            ) * 1000);
    };
public:
    int length;
    Music() = default;
    void reset() {
        BASS_StreamFree(stream);
    }
    void load(const fs::path& path) {
        stream = BASS_StreamCreateFile(FALSE, path.c_str(), 0, 0, BASS_STREAM_AUTOFREE);
        getLength();
    }
    int getPos() const {
            return static_cast<int>(
                BASS_ChannelBytes2Seconds(
                    stream,
                    BASS_ChannelGetPosition(
                        stream,
                        BASS_POS_BYTE
                    )
                ) * 1000);
    }
    void Play() {
        BASS_ChannelPlay(stream, FALSE);
    }
    void Pause() {
        BASS_ChannelPause(stream);
    }
    void Stop() {
        BASS_ChannelStop(stream);
        reset();
        length = 0;
    }
    void SetVolume(float volume) {
        BASS_ChannelSetAttribute(stream, BASS_ATTRIB_VOL, volume);
    }
    bool checkActive() {
        return BASS_ChannelIsActive(stream);
    }
};

class Audio {
private:
    fs::path exe_path = get_executable_path();
    DefaultSounds defaultSounds;
    Sounds sounds;
    Music music;
    float musicVolume = 1.f;
    float soundsVolume = 1.f;
public:
    Audio() {
        if(!BASS_Init(-1, 44100, 0, nullptr, nullptr)) 
          std::cerr << "Ошибка инициализации BASS: " << BASS_ErrorGetCode() << '\n';
    }
    ~Audio() { BASS_Free(); }
    void loadDefault() {
        for (auto [key, value] : defaultSounds.getSounds()) {
            sounds.load(key, value);
        }
        setSoundsVolume(.2f);
    }
    int getPos() const { return music.getPos(); }
    void playSound(const std::string& key) {
        sounds.playSound(key);
    }
    void setSoundsVolume(float volume) {
        soundsVolume = volume;
        sounds.setVolume(soundsVolume);
    }
    void upSoundsVol() {
        if(soundsVolume <= 1.f) {
            soundsVolume += .1f;
            sounds.setVolume(soundsVolume);
        }
    }
    void downSoundsVol() {
        if(soundsVolume >= 0) {
            soundsVolume -= .1f;
            sounds.setVolume(soundsVolume);
        }
    }
    void playAudio(const fs::path& path) {
        music.load(path);
        setAudioVolume(0.4f);
        music.SetVolume(musicVolume);
        music.Play();
    }
    void setAudioVolume(float volume) {
        musicVolume = volume;
    }
    void upAudioVol() {
        if(musicVolume < 1.f) {
            musicVolume += .1f;
            music.SetVolume(musicVolume);
        }
    }
    void downAudioVol() {
        if(musicVolume > 0) {
            musicVolume -= .1f;
            music.SetVolume(musicVolume);
        }
    }
    bool checkAudioIsActive() {
        return music.checkActive();
    }
    void pauseAudio() {
        music.Pause();
    }
    void stopAudio() {
        music.Stop();
    }
};
