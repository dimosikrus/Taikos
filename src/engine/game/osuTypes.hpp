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

struct Taps {
    int time;
    HitSound hs;
    bool hitted = false;
};

struct Score {
    int x0 = 0;
    int x50 = 0;
    int x100 = 0;
    int x300 = 0;
    int combo = 0;
    int maxCombo = 0;
    int score = 0;
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


//std::ostream& operator<<(std::ostream& os, const Type type) {
//    int d = 0;
//    if (type == Type::None) os << "Type.None";
//    else {
//        os << "Type.";
//        if ((type & Type::HitCircle) != Type::None) {
//            os << (d != 0 ? "|" : "") << "HitCircle";
//            d++;
//        }
//        if ((type & Type::Slider) != Type::None) {
//            os << (d != 0 ? "|" : "") << "Slider";
//            d++;
//        }
//        if ((type & Type::NewCombo) != Type::None) {
//            d++;
//            os << (d != 0 ? "|" : "") << "NewCombo";
//        }
//        if ((type & Type::Spinner) != Type::None) {
//            os << (d != 0 ? "|" : "") << "Spinner";
//            d++;
//        }
//        if ((type & Type::Skip1) != Type::None) {
//            os << (d != 0 ? "|" : "") << "Skip1";
//            d++;
//        }
//        if ((type & Type::Skip2) != Type::None) {
//            os << (d != 0 ? "|" : "") << "Skip2";
//            d++;
//        }
//        if ((type & Type::Skip3) != Type::None) {
//            os << (d != 0 ? "|" : "") << "Skip3";
//            d++;
//        }
//    }
//    return os;
//}
//
//std::ostream& operator<<(std::ostream& os, const HitSound hitSound) {
//    int d = 0;
//    if (hitSound == HitSound::None) os << "HitSound.None";
//    else {
//        os << "HitSound.";
//        if ((hitSound & HitSound::Normal) != HitSound::None) {
//            os << (d != 0 ? "|" : "") << "Normal";
//            d++;
//        }
//        if ((hitSound & HitSound::Whistle) != HitSound::None) {
//            os << (d != 0 ? "|" : "") << "Whistle";
//            d++;
//        }
//        if ((hitSound & HitSound::Finish) != HitSound::None) {
//            os << (d != 0 ? "|" : "") << "Finish";
//            d++;
//        }
//        if ((hitSound & HitSound::Clap) != HitSound::None) {
//            os << (d != 0 ? "|" : "") << "Clap";
//            d++;
//        }
//    }
//    return os;
//}
//
//std::ostream& operator<<(std::ostream& os, const HitObject& hobj) {
//    os << "HitObject("
//        << "hitted=" << (hobj.hitted ? "true" : "false")
//        << ", next=" << (hobj.next ? "true" : "false")
//        << ", time=" << hobj.time
//        << ", " << hobj.type
//        << ", " << hobj.hitSound
//        << ", length=" << hobj.length
//        << ")";
//    return os;
//}

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
};