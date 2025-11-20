#include <iostream>

#include "audio.hpp"

#include <unordered_map>

#include <bass/bass.h>
#include <bass_fx/bass_fx.h>

// FILESYSTEM BIT
#include <filesystem>
namespace fs = std::filesystem;
inline bool chechFileExist(const fs::path& path) {
    return fs::exists(path) && fs::is_regular_file(path);
}

inline bool checkFolderExist(const fs::path& path) {
    return fs::exists(path) && fs::is_directory(path);
}
// FILESYSTEM BIT

DefaultSounds::DefaultSounds() {
    pushSound("hitZ", fs::path("assets\\hitsound1.wav"));
    pushSound("hitX", fs::path("assets\\hitsound2.wav"));
}
void DefaultSounds::pushSound(const std::string& key, const fs::path& path) {
    if (chechFileExist(path))
        files.emplace(key, path);
}
std::optional<fs::path> DefaultSounds::getSound(const std::string& key) {
    if (files.find(key) != files.end()) {
        return files.at(key);
    };
    return std::nullopt;
}
std::unordered_map<std::string, fs::path> DefaultSounds::getSounds() {
    return files;
}

Sound::Sound(const fs::path& path) {
    if (!chechFileExist(path))
        std::cerr << "Error load Sound: " << path << '\n';
    this->file = BASS_SampleLoad(FALSE, path.c_str(), 0, 0, 3, BASS_SAMPLE_OVER_POS);
    this->channel = BASS_SampleGetChannel(this->file, FALSE);
}

Sound::~Sound() { BASS_SampleFree(this->file); }

HCHANNEL Sound::getChannel() {
    return this->channel;
}

void Sound::play() {
    BASS_ChannelPlay(this->channel, TRUE);
}


void Sounds::reset() {
    this->sounds.clear();
}

void Sounds::load(const std::string& key, const fs::path& path) {
    this->sounds.emplace(key, std::make_unique<Sound>(path));
}

void Sounds::setVolume(float volume) {
    for (auto& [_, sound] : this->sounds) {
        BASS_ChannelSetAttribute(sound->getChannel(), BASS_ATTRIB_VOL, volume);
    }
}

void Sounds::playSound(const std::string& key) {
    if (this->sounds.find(key) != this->sounds.end()) {
        this->sounds.at(key)->play();
    };
}

int Music::get_length() {
    return static_cast<int>(
        BASS_ChannelBytes2Seconds(
            stream,
            BASS_ChannelGetLength(
                stream,
                BASS_POS_BYTE
            )
        ) * 1000);
};

double Music::get_double_length_S() {
    return BASS_ChannelBytes2Seconds(
            stream,
            BASS_ChannelGetLength(
                stream,
                BASS_POS_BYTE
            )
        );
};

int Music::get_pos() {
    return static_cast<int>(
        BASS_ChannelBytes2Seconds(
            stream,
            BASS_ChannelGetPosition(
                stream,
                BASS_POS_BYTE
            )
        ) * 1000);
};

void Music::reset() {
    BASS_StreamFree(stream);
}

void Music::load(const fs::path& path) {
    stream = BASS_StreamCreateFile(FALSE, path.c_str(), 0, 0, BASS_STREAM_AUTOFREE);
    this->length = get_length();
}

int Music::GetPos() { return get_pos(); };

int Music::GetLength() { return get_length(); };

int Music::GetLengthD_S() { return get_double_length_S(); };

void Music::ResetPos() {
    BASS_ChannelSetPosition(stream, 0, BASS_POS_BYTE);
}

void Music::SetPos(double pos) {
    BASS_ChannelSetPosition(stream, BASS_ChannelSeconds2Bytes(stream, pos), BASS_POS_BYTE);
}

void Music::Play() {
    BASS_ChannelPlay(stream, FALSE);
}

void Music::Pause() {
    BASS_ChannelPause(stream);
}

void Music::Stop() {
    BASS_ChannelStop(stream);
    reset();
    length = 0;
}

void Music::SetVolume(float volume) {
    BASS_ChannelSetAttribute(stream, BASS_ATTRIB_VOL, volume);
}

bool Music::checkActive() {
    return BASS_ChannelIsActive(stream);
}

// Audio
Audio::Audio() {
    if (!BASS_Init(-1, 44100, 0, nullptr, nullptr))
        std::cerr << "Ошибка инициализации BASS: " << BASS_ErrorGetCode() << '\n';
}

Audio::~Audio() { BASS_Free(); }

void Audio::playSound(const std::string& key) {
    this->sounds.playSound(key);
}

void Audio::setSoundsVolume(float volume) {
    this->soundsVolume = volume;
    this->sounds.setVolume(soundsVolume);
}

void Audio::upSoundsVol() {
    if (this->soundsVolume <= 1.f) {
        this->soundsVolume += .1f;
        this->sounds.setVolume(soundsVolume);
    }
    else this->soundsVolume = 1.f;
}

void Audio::downSoundsVol() {
    if (this->soundsVolume >= 0) {
        this->soundsVolume -= .1f;
        this->sounds.setVolume(soundsVolume);
    }
    else this->soundsVolume = 0;
}

// Music
void Audio::resetPos() {
    this->music.ResetPos();
}

void Audio::setPos(double pos) {
    this->music.SetPos(pos);
}


void Audio::loadAudio(const fs::path& path) {
    this->music.load(path);
    this->music.SetVolume(this->musicVolume);
}

void Audio::playAudio() {
    this->music.Play();
}

void Audio::unPauseAudio() {
    if (this->pausedAudio) {
        this->pausedAudio = false;
        this->music.Play();
    }
}

void Audio::pauseAudio() {
    if (!this->pausedAudio) {
        this->pausedAudio = true;
        this->music.Pause();
    }
}

void Audio::stopAudio() {
    this->music.Stop();
}


void Audio::setAudioVolume(float volume) {
    this->musicVolume = volume;
}

void Audio::upAudioVol() {
    if (this->musicVolume < 1.f) {
        this->musicVolume += .1f;
        this->music.SetVolume(musicVolume);
    }
    else this->musicVolume = 1.f;
}

void Audio::downAudioVol() {
    if (this->musicVolume > 0) {
        this->musicVolume -= .1f;
        this->music.SetVolume(musicVolume);
    }
    else musicVolume = 0;
}

int Audio::getMusicPos() { return this->music.GetPos(); }
float Audio::getSoundVolume() { return this->soundsVolume; };
float Audio::getMusicVolume() { return this->musicVolume; };
bool Audio::checkMusicIsActive() { return this->music.checkActive(); }
bool Audio::paused() { return this->pausedAudio; }
int Audio::GetMusicLength() { return music.GetLength(); };
int Audio::GetMusicLengthD_S() { return music.GetLengthD_S(); };

void Audio::loadDefault() {
    for (auto& [key, value] : defaultSounds.getSounds()) {
        sounds.load(key, value);
    }
    setSoundsVolume(soundsVolume);
}