#pragma once

#include <iostream>
#include <optional>
#include <filesystem>
#include <unordered_map>
#include <SFML/Audio.hpp>

namespace fs = std::filesystem;

class Sound {
    sf::SoundBuffer buffer;
    sf::Sound sound;
public:
    Sound(const fs::path& path): buffer(path), sound(buffer) {}
    void play() { sound.play(); }
    void pause() { sound.pause(); }
    void stop() { sound.stop(); }

    float getVolume() { return sound.getVolume(); }
    void setVolume(float volume) { sound.setVolume(volume); }

    float getPitch() { return sound.getPitch(); }
    void setPitch(float pitch) { sound.setPitch(pitch); }

    bool isLooping() { return sound.isLooping(); }
    void setLooping(bool loop) { sound.setLooping(loop); }

    sf::Time getOffset() { return sound.getPlayingOffset(); }
    void setOffset(sf::Time offset) { sound.setPlayingOffset(offset); }
};

class Sounds {
    std::unordered_map<std::string, std::unique_ptr<Sound>> sounds;
public:
    Sounds() = default;
    void reset() { this->sounds.clear(); }
    void load(const std::string& key, const fs::path& path) {
        this->sounds.emplace(key, std::make_unique<Sound>(path));
    }
    
    void setPitch(const std::string& key, float pitch) {
        if (this->sounds.find(key) != this->sounds.end()) {
            this->sounds.at(key)->setPitch(pitch);
        };
    }
    void play(const std::string& key) {
        if (this->sounds.find(key) != this->sounds.end()) {
            this->sounds.at(key)->play();
        };
    }
    void pause(const std::string& key) {
        if (this->sounds.find(key) != this->sounds.end()) {
            this->sounds.at(key)->pause();
        };
    }
    void stop(const std::string& key) {
        if (this->sounds.find(key) != this->sounds.end()) {
            this->sounds.at(key)->stop();
        };
    }

    void setVolume(float volume) {
        for (auto& [_, sound] : this->sounds) {
            sound->setVolume(volume);
        }
    }
    void setPitchAll(float pitch) {
        for (auto& [_, sound] : this->sounds) {
            sound->setPitch(pitch);
        }
    }
    void stopAll() {
        for (auto& [_, sound] : this->sounds) {
            sound->stop();
        }
    }
};

class Music {
    sf::Music music;
public:
    Music() = default;
    void load(const fs::path& path) { music.openFromFile(path); }
    void play() { music.play(); }
    void pause() { music.pause(); }
    void stop() { music.stop(); }

    float getVolume() { return music.getVolume(); }
    void setVolume(float volume) { music.setVolume(volume); }

    float getPitch() { return music.getPitch(); }
    void setPitch(float pitch) { return music.setPitch(pitch); }

    bool isLooping() { return music.isLooping(); }
    void setLooping(bool loop) { music.setLooping(loop); }

    sf::Time getOffset() { return music.getPlayingOffset(); }
    void setOffset(sf::Time offset) { music.setPlayingOffset(offset); }

    sf::Time getDuration() { return music.getDuration(); }

    unsigned int getSampleRate() { return music.getSampleRate(); }

    sf::SoundSource::Status getStatus() { return music.getStatus(); }

    // sf::SoundSource::Status status = music.getStatus();
    // sf::SoundSource::Status::Paused;
    // sf::SoundSource::Status::Playing;
    // sf::SoundSource::Status::Stopped;
};

class AudioEngine {
    Music music;
    Sounds sounds;
    float musicVolume = 10.f;
    float soundsVolume = 10.f;
    bool pausedAudio = false;
public:
    AudioEngine() {
        sounds.load("hitZ", fs::path("assets/hitsound1.ogg"));
        sounds.load("hitX", fs::path("assets/hitsound2.ogg"));
    }

    // Sounds
    void playSound(const std::string& key) { sounds.play(key); }

    void setSoundsVolume(float volume) {
        soundsVolume = volume;
        sounds.setVolume(soundsVolume);
    }
    float getSoundsVolume() { return soundsVolume; }
    void upSoundsVolume() {
        if (soundsVolume <= 100.f) {
            soundsVolume += 1.f;
            sounds.setVolume(soundsVolume);
        }
        else soundsVolume = 1.f;
    }
    void downSoundsVolume() {
        if (soundsVolume >= 0) {
            soundsVolume -= 1.f;
            sounds.setVolume(soundsVolume);
        }
        else soundsVolume = 0;
    }

    // Music
    void resetAudioOffset() { music.setOffset(sf::seconds(0)); }
    sf::Time getAudioDuration() { return music.getDuration(); }
    void setAudioOffset(sf::Time offset) { music.setOffset(offset); }
    sf::Time getAudioOffset() { return music.getOffset(); }

    void setAudioPitch(float pitch) { music.setPitch(pitch); }
    float getAudioPitch() { return music.getPitch(); }

    unsigned int getSampleRate() { return music.getSampleRate(); }

    bool paused() { return pausedAudio; }
    bool checkMusicIsActive() {
        if (music.getStatus() == sf::SoundSource::Status::Playing) return true;
        return false;
    }

    void loadAudio(const std::filesystem::path &path) { music.load(path); }

    void playAudio() { music.play(); }

    void pauseAudio() {
        if (!pausedAudio) {
            pausedAudio = true;
            music.pause();
        }
    }
    void unPauseAudio() {
        if (pausedAudio) {
            pausedAudio = false;
            music.play();
        }
    }

    void stopAudio() { music.stop(); }
    
    void setAudioVolume(float volume) {
        musicVolume = volume;
        music.setVolume(musicVolume);
    }
    float getAudioVolume() { return musicVolume; }
    void upAudioVolume() {
        if (musicVolume <= 100.f) {
            musicVolume += 1.f;
            music.setVolume(musicVolume);
        }
        else musicVolume = 1.f;
    }
    void downAudioVolume() {
        if (musicVolume >= 0) {
            musicVolume -= 1.f;
            music.setVolume(musicVolume);
        }
        else musicVolume = 0;
    }
};