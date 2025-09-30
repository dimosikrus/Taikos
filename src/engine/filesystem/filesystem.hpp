#pragma once

#include <iostream>
#include <cmath>   // Для std::round
#include <optional>
#include <filesystem>
#include <windows.h>
#include <functional>
#include <fstream>
#include <algorithm>
#include "../game/osuTypes.hpp"

namespace fs = std::filesystem;
inline bool chechFileExist(const fs::path& path) {
    return fs::exists(path) && fs::is_regular_file(path);
}
inline bool checkFolderExist(const fs::path& path) {
    return fs::exists(path) && fs::is_directory(path);
}
fs::path get_executable_path() {
    WCHAR buffer[MAX_PATH];
    GetModuleFileNameW(nullptr, buffer, MAX_PATH); // Получаем полный путь к .exe
    return fs::path(buffer).parent_path(); // Путь без имени файла
}

void removeSpaces(std::string& line) {
    size_t colon_pos = line.find(':');
    if (colon_pos != std::string::npos) {
        // Находим первый непробельный символ после двоеточия
        size_t first_non_space = line.find_first_not_of(' ', colon_pos + 1);
        
        if (first_non_space != std::string::npos) {
            // Удаляем пробелы между ':' и первым непробельным символом
            line.erase(colon_pos + 1, first_non_space - colon_pos - 1);
        } else {
            // Если после ':' только пробелы - удаляем все
            line.erase(colon_pos + 1);
        }
    }
}

std::vector<std::string> split(const std::string& input, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::stringstream ss(input); // Создаем строковый поток из входной строки

    // В цикле извлекаем токены, разделенные 'delimiter'
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

OsuFile parseOsuFile(const fs::path& path) {
    std::ifstream file(path);
    std::string line;
    OsuFile osufile;
    osufile.filepath = path;
    while (std::getline(file, line)) {
        if (line.find("AudioFilename:") != std::string::npos) {
            removeSpaces(line);
            size_t pos = line.find("AudioFilename:");
            if (pos != std::string::npos) {
                std::string value = line.substr(pos + 14);
                osufile.audioFileName = value;
            }
        }
        if (line.find("AudioLeadIn:") != std::string::npos) {
            removeSpaces(line);
            size_t pos = line.find("AudioLeadIn:");
            if (pos != std::string::npos) {
                std::string value = line.substr(pos + 12);
                osufile.audioLeadIn = stoi(value);
            }
        }
        if (line.find("PreviewTime:") != std::string::npos) {
            removeSpaces(line);
            size_t pos = line.find("PreviewTime:");
            if (pos != std::string::npos) {
                std::string value = line.substr(pos + 12);
                osufile.previewTime = stoi(value);
            }
        }
        if (line.find("Mode:") != std::string::npos) {
            removeSpaces(line);
            size_t pos = line.find("Mode:");
            if (pos != std::string::npos) {
                std::string value = line.substr(pos + 5);
                osufile.mode = stoi(value);
                if (osufile.mode != 1) { break; }
            }
        }
        if (line.find("Title:") != std::string::npos) {
            removeSpaces(line);
            size_t pos = line.find("Title:");
            if (pos != std::string::npos) {
                std::string value = line.substr(pos + 6);
                osufile.title = value;
            }
        }
        if (line.find("Artist:") != std::string::npos) {
            removeSpaces(line);
            size_t pos = line.find("Artist:");
            if (pos != std::string::npos) {
                std::string value = line.substr(pos + 7);
                osufile.artist = value;
            }
        }
        if (line.find("Creator:") != std::string::npos) {
            removeSpaces(line);
            size_t pos = line.find("Creator:");
            if (pos != std::string::npos) {
                std::string value = line.substr(pos + 8);
                osufile.creator= value;
            }
        }
        if (line.find("Version:") != std::string::npos) {
            removeSpaces(line);
            size_t pos = line.find("Version:");
            if (pos != std::string::npos) {
                std::string value = line.substr(pos + 8);
                osufile.version = value;
            }
        }
        if (line.find("[Events]") != std::string::npos) {
            break;
        }
    }
    file.close();
    return osufile;
}

static bool fromMinToMax(const HitObject& first, const HitObject& second) {
    return first.time < second.time;
}

std::vector<HitObject> parseHitObjects(const fs::path& path) {
    std::ifstream file(path);
    std::string line;
    std::vector<HitObject> hitobjects;
    bool readhits = false;
    while (std::getline(file, line)) {
        if (readhits) {
            std::vector<std::string> params = split(line, ',');
            HitObject hitObj;
            hitObj.time = stoi(params.at(2));
            hitObj.type = static_cast<Type>(stoi(params.at(3)));
            hitObj.hitSound = static_cast<HitSound>(stoi(params.at(4)));
            if (params.size() > 7) {
                hitObj.length = stoi(params.at(7));
            }
            hitobjects.emplace_back(hitObj);
        }
        if (line.find("[HitObjects]") != std::string::npos) {
            readhits = true;
        }
    }
    file.close();
    // sorted
    std::sort(hitobjects.begin(), hitobjects.end(), fromMinToMax);
    //
    return hitobjects;
};

size_t count_elements(const fs::path& dir) {
    size_t count = 0;
    for (auto it = fs::recursive_directory_iterator(dir);
        it != fs::recursive_directory_iterator(); ++it) {
        ++count;
    }
    return count;
}

std::vector<OsuFile> listDirOsus(const fs::path& path) {
    std::vector<OsuFile> osus;
    float count_of_files = static_cast<float>(count_elements(path));
    float count = 0;
    float now = 0;
    for(const auto& file : fs::recursive_directory_iterator(path)) {
        const auto& itFile = file.path();
        if(chechFileExist(itFile)) {
            if (itFile.filename().extension().string().find(".osu") != std::string::npos) {
                // std::cout << itFile.string() << '\n';
                OsuFile osufile = parseOsuFile(itFile);
                if (osufile.mode == 1) {
                    osus.emplace_back(osufile);
                }
                float a = std::round(count / count_of_files * 100);
                if (now != a) {
                    now = a;
                    std::cout << "Scanning... " << now << "% \t" << itFile.parent_path().string() << '\r';
                }
            };
        }
        count++;
    }
    std::cout << '\n';
    return osus;
};
