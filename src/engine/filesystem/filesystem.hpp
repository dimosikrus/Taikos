#pragma once

#include <iostream> // std::cout
#include <cmath>   // ��� std::round
#include <functional>
#include <algorithm>
#include <optional> // return obj or null

#include <filesystem> // fs
#include <fstream> // file open
#include <string>

#ifdef _WIN32
#include <windows.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h> // Специально для macOS
#else
#include <unistd.h>
#include <climits>
#endif

#include "../game/osuTypes.hpp"
#include "../logger.hpp"

namespace fs = std::filesystem;

inline bool chechFileExist(const fs::path& path) {
    return fs::exists(path) && fs::is_regular_file(path);
}

inline bool checkFolderExist(const fs::path& path) {
    return fs::exists(path) && fs::is_directory(path);
}

fs::path get_executable_path() {
#ifdef _WIN32
    WCHAR buffer[MAX_PATH];
    GetModuleFileNameW(nullptr, buffer, MAX_PATH);
    return fs::path(buffer).parent_path();
#elif defined(__APPLE__)
    char buffer[PATH_MAX];
    uint32_t size = sizeof(buffer);
    if (_NSGetExecutablePath(buffer, &size) == 0) {
        return fs::path(buffer).parent_path();
    }
    throw std::runtime_error("Failed to get executable path on macOS");
#else
    // Это остается для Linux
    char buffer[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", buffer, PATH_MAX);
    if (count == -1) {
        throw std::runtime_error("Failed to get executable path on Linux");
    }
    buffer[count] = '\0';
    return fs::path(buffer).parent_path();
#endif
}

void removeSpaces(std::string& line) {
    size_t colon_pos = line.find(':');
    if (colon_pos != std::string::npos) {
        // ������� ������ ������������ ������ ����� ���������
        size_t first_non_space = line.find_first_not_of(' ', colon_pos + 1);

        if (first_non_space != std::string::npos)
            // ������� ������� ����� ':' � ������ ������������ ��������
            line.erase(colon_pos + 1, first_non_space - colon_pos - 1);
        else
            // ���� ����� ':' ������ ������� - ������� ���
            line.erase(colon_pos + 1);
    }
}

std::vector<std::string> split(const std::string& input, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::stringstream ss(input); // ������� ��������� ����� �� ������� ������

    // � ����� ��������� ������, ����������� 'delimiter'
    while (std::getline(ss, token, delimiter))
        tokens.push_back(token);

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

                if (osufile.mode != 1) break;
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
                osufile.creator = value;
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

        if (line.find("[Events]") != std::string::npos) break;
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
            // std::cout << "LINE: " << line << '\n';
            std::vector<std::string> params = split(line, ',');
            HitObject hitObj;
            hitObj.time = stoi(params.at(2));
            hitObj.type = static_cast<Type>(stoi(params.at(3)));
            hitObj.hitSound = static_cast<HitSound>(stoi(params.at(4)));
            if ((hitObj.type & Type::Spinner) == Type::Spinner && params.size() > 5) {
                hitObj.endTime = stoi(params.at(5));
                // std::cout << "params5: " << params.at(5) << '\n';
            }
            if ((hitObj.type & Type::Slider) == Type::Slider && params.size() > 7) {
                hitObj.length = stod(params.at(7));
                // std::cout << "params6: " << params.at(7) << '\n';
            }
            hitobjects.emplace_back(hitObj);
        }

        if (line.find("[HitObjects]") != std::string::npos)
            readhits = true;
    }
    file.close();
    std::vector<HitObject> newObjects;
    for(const auto& hobj : hitobjects) {
        // std::cout << "TEST: " << hobj << '\n';
        if ((hobj.type & Type::Spinner) == Type::Spinner && !hobj.spinnerTail) {
            float step = static_cast<float>(hobj.endTime - hobj.time) / 9.f;
            // std::cout << hobj << " Step: " << step << '\n';
            for (int i = 1; i < 10; ++i) {
                int newTime = static_cast<int>(hobj.time + (i * step));
                HitObject newHitObject = hobj;
                newHitObject.time = newTime;
                newHitObject.spinnerTail = true;
                newObjects.push_back(newHitObject);
                // std::cout << "Index: " << i << " NewTime: " << newTime << '\n';
            }
        }
        if ((hobj.type & Type::Slider) == Type::Slider && !hobj.sliderTail) {
            int EndTime = hobj.time + static_cast<int>(hobj.length * 4);
            // std::cout << " EndTime: " << EndTime << '\n';
            float step = static_cast<float>(EndTime - hobj.time) / 9.f;
            // std::cout << hobj << " Step: " << step << '\n';
            for (int i = 1; i < 10; ++i) {
                int newTime = static_cast<int>(hobj.time + (i * step));
                if (newTime < 0) newTime = 0;
                HitObject newHitObject = hobj;
                newHitObject.time = newTime;
                newHitObject.sliderTail = true;
                newObjects.push_back(newHitObject);
                // std::cout << "Index: " << i << " NewTime: " << newTime << '\n';
            }
        }
    }
    hitobjects.insert(hitobjects.end(), newObjects.begin(), newObjects.end());
    // sorted
    std::sort(hitobjects.begin(), hitobjects.end(), fromMinToMax);
    //
    return hitobjects;
};

size_t count_elements(const fs::path& dir) {
    size_t count = 0;

    for (auto it = fs::recursive_directory_iterator(dir); it != fs::recursive_directory_iterator(); ++it)
        ++count;

    return count;
}

std::vector<OsuFile> listDirOsus(const fs::path& path) {
    Logger logger;
    std::vector<OsuFile> osus;
    float count_of_files = static_cast<float>(count_elements(path));
    float count = 0;
    float now = 0;

    for (const auto& file : fs::recursive_directory_iterator(path)) {
        const auto& itFile = file.path();

        if (chechFileExist(itFile)) {
            if (itFile.filename().extension().string().find(".osu") != std::string::npos) {
                OsuFile osufile = parseOsuFile(itFile);

                if (osufile.mode == 1)
                    osus.emplace_back(osufile);

                float a = std::round(count / count_of_files * 100);

                if (now != a) {
                    now = a;
                    std::ostringstream oss;
                    oss << "Scanning... " << now << "% " << itFile.parent_path().filename().string();
                    logger.log(LogLevel::WARN, oss.str());
                }
            };
        }
        count++;
    }
    return osus;
};