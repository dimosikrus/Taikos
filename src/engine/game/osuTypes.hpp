#pragma once

//#include <iostream>*/
#include <filesystem>

namespace fs = std::filesystem;

enum class Type : uint8_t {
    None = 0,
    HitCircle = 1 << 0,
    Slider = 1 << 1,
    NewCombo = 1 << 2,
    Spinner = 1 << 3,
    Skip1 = 1 << 4,
    Skip2 = 1 << 5,
    Skip3 = 1 << 6
};

static inline Type operator|(const Type a, const Type b) {
    return static_cast<Type>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}
static inline Type operator&(const Type a, const Type b) {
    return static_cast<Type>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
}

enum class HitSound : uint8_t {
    None = 0,
    Normal = 1 << 0,
    Whistle = 1 << 1,
    Finish = 1 << 2,
    Clap = 1 << 3
};

static inline HitSound operator|(const HitSound a, const HitSound b) {
    return static_cast<HitSound>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}
static inline HitSound operator&(const HitSound a, const HitSound b) {
    return static_cast<HitSound>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
}

struct HitObject {
    bool hitted = false;
    bool next = false;
    //int x;
    //int y;
    int time;
    Type type;
    HitSound hitSound;
    //objectParams;
    //slides (Slider);
    int length = 0; // Slider;
};

struct OsuFile {
    fs::path filepath;
    std::string audioFileName;
    int audioLeadIn;
    int previewTime;
    int mode;

    std::string title;
    std::string artist;
    std::string creator;
    std::string version;

    std::vector<HitObject> hitobjects;

    // hp
    // cs
    // od
    // ar
};