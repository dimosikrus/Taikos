#pragma once

#include <iostream>
#include <ctime>

struct Ansi {
    enum class Color {
        RESET,
        BLACK,
        RED,
        GREEN,
        YELLOW,
        BLUE,
        MAGENTA,
        CYAN,
        WHITE,
        BRIGHT_BLACK,
        BRIGHT_RED,
        BRIGHT_GREEN,
        BRIGHT_YELLOW,
        BRIGHT_BLUE,
        BRIGHT_MAGENTA,
        BRIGHT_CYAN,
        BRIGHT_WHITE
    };

    static inline std::string RESET = "\033[0m";
    static inline std::string BLACK = "\033[30m";
    static inline std::string RED = "\033[31m";
    static inline std::string GREEN = "\033[32m";
    static inline std::string YELLOW = "\033[33m";
    static inline std::string BLUE = "\033[34m";
    static inline std::string MAGENTA = "\033[35m";
    static inline std::string CYAN = "\033[36m";
    static inline std::string WHITE = "\033[37m";
    static inline std::string BRIGHT_BLACK = "\033[90m";
    static inline std::string BRIGHT_RED = "\033[91m";
    static inline std::string BRIGHT_GREEN = "\033[92m";
    static inline std::string BRIGHT_YELLOW = "\033[93m";
    static inline std::string BRIGHT_BLUE = "\033[94m";
    static inline std::string BRIGHT_MAGENTA = "\033[95m";
    static inline std::string BRIGHT_CYAN = "\033[96m";
    static inline std::string BRIGHT_WHITE = "\033[97m";
};

enum class LogLevel {
    INFO,
    WARN,
    ERR,
    DEBUG
};

class Logger {
    std::string getT(LogLevel level) {
        switch (level) {
        case LogLevel::INFO: return Ansi::CYAN;
        case LogLevel::WARN: return Ansi::YELLOW;
        case LogLevel::ERR: return Ansi::RED;
        case LogLevel::DEBUG: return Ansi::MAGENTA;
        default: return Ansi::CYAN;
        }
    }

    std::string getD(LogLevel level) {
        switch (level) {
        case LogLevel::INFO: return "[INFO]" + Ansi::BRIGHT_CYAN;
        case LogLevel::WARN: return "[WARN]" + Ansi::BRIGHT_YELLOW;
        case LogLevel::ERR: return "[ERROR]" + Ansi::BRIGHT_RED;
        case LogLevel::DEBUG: return "[DEBUG]" + Ansi::BRIGHT_MAGENTA;
        default: return Ansi::CYAN;
        }
    }

    std::string getL(LogLevel level) {
        switch (level) {
        case LogLevel::INFO: return "[INFO]";
        case LogLevel::WARN: return "[WARN]";
        case LogLevel::ERR: return "[ERROR]";
        case LogLevel::DEBUG: return "[DEBUG]";
        default: return Ansi::CYAN;
        }
    }

    std::string getA(Ansi::Color color) {
        switch (color)
        {
        case Ansi::Color::RESET: return Ansi::RESET;
        case Ansi::Color::BLACK: return Ansi::BLACK;
        case Ansi::Color::RED: return Ansi::RED;
        case Ansi::Color::GREEN: return Ansi::GREEN;
        case Ansi::Color::YELLOW: return Ansi::YELLOW;
        case Ansi::Color::BLUE: return Ansi::BLUE;
        case Ansi::Color::MAGENTA: return Ansi::MAGENTA;
        case Ansi::Color::CYAN: return Ansi::CYAN;
        case Ansi::Color::WHITE: return Ansi::WHITE;
        case Ansi::Color::BRIGHT_BLACK: return Ansi::BRIGHT_BLACK;
        case Ansi::Color::BRIGHT_RED: return Ansi::BRIGHT_RED;
        case Ansi::Color::BRIGHT_GREEN: return Ansi::BRIGHT_GREEN;
        case Ansi::Color::BRIGHT_YELLOW: return Ansi::BRIGHT_YELLOW;
        case Ansi::Color::BRIGHT_BLUE: return Ansi::BRIGHT_BLUE;
        case Ansi::Color::BRIGHT_MAGENTA: return Ansi::BRIGHT_MAGENTA;
        case Ansi::Color::BRIGHT_CYAN: return Ansi::BRIGHT_CYAN;
        case Ansi::Color::BRIGHT_WHITE: return Ansi::BRIGHT_WHITE;
        default: return Ansi::RESET;
        }
    }

    std::string getCT() {
        time_t now = time(0);
        tm* ltm = localtime(&now);
        char buffer[9];
        strftime(buffer, sizeof(buffer), "%H:%M:%S", ltm);
        return std::string(buffer);
    }
public:
    Logger() = default;

    void log(LogLevel level, const std::string text) {
        std::cout << getT(level) << "[" << getCT() << "] " << getD(level) << " " << text << Ansi::RESET << '\n';
    }

    void log(LogLevel level, const std::string text, Ansi::Color color) {
        std::cout << getT(level) << "[" << getCT() << "] " << getL(level) << " " << getA(color) << text << Ansi::RESET << '\n';
    }
};
