#pragma once

#include "../filesystem/filesystem.hpp"
#include <SFML/Graphics.hpp>

class ConfigFile {
    fs::path binPath;
    fs::path configPath;
    bool createNewConfigFile() {
        std::ofstream cfgFile(configPath, std::ios::out);
        if (cfgFile.is_open()) {
            cfgFile << "HitSoundsVolume: 0.1\n";
            cfgFile << "MusicVolume: 0.1\n";
            cfgFile << "KaColour: 0, 0, 255, 255\n";
            cfgFile << "DonColour: 255, 0, 0, 255\n";
            cfgFile << "SliderColour: 255, 255, 0, 255\n";
            cfgFile << "SpinnerColour: 255, 0, 255, 255\n";
            cfgFile.close();
            return true;
        }
        return false;
    }
    bool checkConfig() {
        std::ifstream cfgFile(configPath);
        std::string line;
        size_t checked = 0;
        while(std::getline(cfgFile, line)) {
            if (line.find("HitSoundsVolume:") != std::string::npos) {
                checked += 1;
            }
            if (line.find("MusicVolume:") != std::string::npos) {
                checked += 1;
            }
            if (line.find("KaColour:") != std::string::npos) {
                checked += 1;
            }
            if (line.find("DonColour:") != std::string::npos) {
                checked += 1;
            }
            if (line.find("SliderColour:") != std::string::npos) {
                checked += 1;
            }
            if (line.find("SpinnerColour:") != std::string::npos) {
                checked += 1;
            }
        }
        cfgFile.close();
        if (checked >= 6) {
            return true;
        }
        return false;
    }
    float HitSoundsVolume{.0f};
    float MusicVolume{.0f};
    sf::Color KaColour{10,10,10,255};
    sf::Color DonColour{10,10,10,255};
    sf::Color SliderColour{10,10,10,255};
    sf::Color SpinnerColour{10,10,10,255};

public:
    ConfigFile() : binPath(get_executable_path()), configPath(binPath / fs::path("config.ini")) {
        if(!chechFileExist(configPath)) {
            if(!createNewConfigFile())
                throw std::runtime_error("Failed to create Config File.");
        } else {
            if(!checkConfig()) {
                fs::remove(configPath);
                if(!createNewConfigFile())
                    throw std::runtime_error("Failed to create Config File.");
            }
        }
        if(!loadConfig()) {
            throw std::runtime_error("Failed to load Config File.");
        }
    }

    bool loadConfig() {
        std::ifstream cfgFile(configPath);
        std::string line;
        size_t checked = 0;
        while(std::getline(cfgFile, line)) {
            if (line.find("HitSoundsVolume:") != std::string::npos) {
                removeSpaces(line);
                size_t pos = line.find("HitSoundsVolume:");

                if (pos != std::string::npos) {
                    std::string value = line.substr(pos + sizeof("HitSoundsVolume:"));
                    HitSoundsVolume = stof(value);
                }
                checked += 1;
            }
            if (line.find("MusicVolume:") != std::string::npos) {
                removeSpaces(line);
                size_t pos = line.find("MusicVolume:");

                if (pos != std::string::npos) {
                    std::string value = line.substr(pos + sizeof("MusicVolume:"));
                    MusicVolume = stof(value);
                }
                checked += 1;
            }
            if (line.find("KaColour:") != std::string::npos) {
                removeSpaces(line);
                size_t pos = line.find("KaColour:");

                if (pos != std::string::npos) {
                    std::vector<std::string> params = split(line.substr(sizeof("KaColour:") - 1), ',');
                    KaColour = params.size() < 4 ?
                        sf::Color::Color(stoi(params.at(0)),stoi(params.at(1)),stoi(params.at(2))) :
                        sf::Color::Color(stoi(params.at(0)),stoi(params.at(1)),stoi(params.at(2)),stoi(params.at(3)));
                }
                checked += 1;
            }
            if (line.find("DonColour:") != std::string::npos) {
                removeSpaces(line);
                size_t pos = line.find("DonColour:");

                if (pos != std::string::npos) {
                    std::vector<std::string> params = split(line.substr(sizeof("DonColour:") - 1), ',');
                    DonColour = params.size() < 4 ?
                        sf::Color::Color(stoi(params.at(0)),stoi(params.at(1)),stoi(params.at(2))) :
                        sf::Color::Color(stoi(params.at(0)),stoi(params.at(1)),stoi(params.at(2)),stoi(params.at(3)));
                }
                checked += 1;
            }
            if (line.find("SliderColour:") != std::string::npos) {
                removeSpaces(line);
                size_t pos = line.find("SliderColour:");

                if (pos != std::string::npos) {
                    std::vector<std::string> params = split(line.substr(sizeof("SliderColour:") - 1), ',');
                    SliderColour = params.size() < 4 ?
                        sf::Color::Color(stoi(params.at(0)),stoi(params.at(1)),stoi(params.at(2))) :
                        sf::Color::Color(stoi(params.at(0)),stoi(params.at(1)),stoi(params.at(2)),stoi(params.at(3)));
                }
                checked += 1;
            }
            if (line.find("SpinnerColour:") != std::string::npos) {
                removeSpaces(line);
                size_t pos = line.find("SpinnerColour:");

                if (pos != std::string::npos) {
                    std::vector<std::string> params = split(line.substr(sizeof("SpinnerColour:") - 1), ',');
                    SpinnerColour = params.size() < 4 ?
                        sf::Color::Color(stoi(params.at(0)),stoi(params.at(1)),stoi(params.at(2))) :
                        sf::Color::Color(stoi(params.at(0)),stoi(params.at(1)),stoi(params.at(2)),stoi(params.at(3)));
                }
                checked += 1;
            }
        }
        cfgFile.close();
        if (checked >= 6) {
            return true;
        }
        return false;
    }

    float getSoundsVol() { return HitSoundsVolume; }
    float getMisucVol() { return MusicVolume; }
    sf::Color getKaColour() { return KaColour; }
    sf::Color getDonColour() { return DonColour; }
    sf::Color getSliderColour() { return SliderColour; }
    sf::Color getSpinnerColour() { return SpinnerColour; }
    // TODO: Setters
};