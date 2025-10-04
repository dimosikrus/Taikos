#pragma once

#include <cstdint>

enum class GameState : uint8_t {
    Exit, // Exit
    Loading, // not usable
    MenuIntro, // Main Menu
    MenuSongSelection,
    GamePlaying,
    GamePause,
    GameResults,
    SettingsMain,
    SettingsAudio // not usable
};

enum class SpecialState : uint8_t {
    None,
    Alt,
    Control,
    Shift
};