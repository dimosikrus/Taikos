#pragma once

#include <sstream>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <vector>
#include <functional>
#include <deque>

#include "../gamestates.hpp"
#include "../filesystem/filesystem.hpp"
#include "../audio/audio.hpp"
#include "../game/game.hpp"

class MenuButton {
    sf::Image image;
    sf::Texture texture;
    sf::Font& font;
    sf::Text text;
    sf::Sprite sprite;
    sf::Vector2f callbackPosition;
    std::function<void()> action;
    sf::Color originalColor;
public:
    MenuButton(sf::Vector2f position, sf::Vector2f size, sf::Color color, sf::Font& font, const std::string buttonText = "", std::function<void()> action = []{}) : 
      image(sf::Vector2u(size), color), texture(image), sprite(texture), font(font), text(font, buttonText), action(action), originalColor(color) {
        sprite.setPosition(position);
        sprite.setTextureRect(sf::IntRect(sf::Vector2i(0,0),sf::Vector2i(size)));
        text.setStyle(sf::Text::Regular);
        text.setCharacterSize(sf::Vector2u(0u,static_cast<unsigned int>(size.y)).y-10u);
        sf::Vector2f textsize = text.getGlobalBounds().size;
        text.setPosition(sf::Vector2f(position.x+(size.x/2.f)-(textsize.x/2.f),position.y));
    }
    void draw(sf::RenderWindow& window) {
        window.draw(sprite);
        window.draw(text);
    }
    void setPosition(sf::Vector2f position) {
        sprite.setPosition(position);
        sf::Vector2f textsize = text.getGlobalBounds().size;
        sf::Vector2f spritesize = sprite.getGlobalBounds().size;
        text.setPosition(sf::Vector2f(position.x+(spritesize.x/2.f)-(textsize.x/2.f),position.y+(spritesize.y/2.f)-(textsize.y/2.f)));
    }
    sf::Vector2f getPosition() { return sprite.getPosition(); }
    bool contains(const sf::Vector2f position) const {
        return sprite.getGlobalBounds().contains(position);
    }
    void setText(const std::string buttonText) {
        text.setString(buttonText);
        sf::Vector2f spritesize = sprite.getGlobalBounds().size;
        text.setCharacterSize(sf::Vector2u(0u,static_cast<unsigned int>(spritesize.y)).y-10u);
        sf::Vector2f textsize = text.getGlobalBounds().size;
        text.setPosition(sf::Vector2f(getPosition().x+(spritesize.x/2.f)-(textsize.x/2.f),getPosition().y+(spritesize.y/2.f)-(textsize.y/2.f)));
    }
    sf::String getText() { return text.getString(); }
    void setFillColorText(const sf::Color color) {
        text.setFillColor(color);
    }
    sf::Color getFillColorText() { return text.getFillColor(); }
    void setOutlineColorText(const sf::Color color) {
        text.setOutlineColor(color);
    }
    sf::Color getOutlineColorText() { return text.getOutlineColor(); }
    void setColor(sf::Color color) {
        sprite.setColor(color);
        originalColor = color;
    }
    sf::Color getColor() { return sprite.getColor(); }
    void onHover(const sf::Vector2f position) {
        callbackPosition = position;
        if (contains(position)) {
            sprite.setColor(sf::Color(
                originalColor.r + 35 <= 255 ? originalColor.r + 35 : 255,
                originalColor.g + 35 <= 255 ? originalColor.g + 35 : 255,
                originalColor.b + 35 <= 255 ? originalColor.b + 35 : 255,
                originalColor.a
            ));
        } else {
            sprite.setColor(originalColor);
        }
    }
    void callback() {
        if (contains(callbackPosition) && action) {
            std::cout << "CLick " << callbackPosition.y << '\n';
            action();
        }
    }
    void setCallback(std::function<void()> action) {
        this->action = action;
    }
};

class FloatingMenu {
    std::vector<std::unique_ptr<MenuButton>> buttons;
    sf::Vector2f originalPosition;
    GameState& localstate;
public:
    FloatingMenu(sf::Vector2f position, sf::Font& font, GameState& state) : originalPosition(position), localstate(state) {
        buttons.push_back(std::make_unique<MenuButton>(position+sf::Vector2f(0,0), sf::Vector2f(300.f,50.f), sf::Color(102, 102, 153, 180), font, "Play", [&]{ localstate = GameState::MenuSongSelection; }));
        buttons.push_back(std::make_unique<MenuButton>(position+sf::Vector2f(0,70.f), sf::Vector2f(300.f,50.f), sf::Color(102, 102, 153, 180), font, "Settings", [&]{ localstate = GameState::SettingsMain; }));
        buttons.push_back(std::make_unique<MenuButton>(position+sf::Vector2f(0,140.f), sf::Vector2f(300.f,50.f), sf::Color(102, 102, 153, 180), font, "Exit", [&]{ localstate = GameState::Exit; }));
    };
    void draw(sf::RenderWindow& window, sf::Vector2f position) {
        for (auto& button : buttons) {
            button->onHover(position);
            button->draw(window);
        }
    }
    void click() {
        for (auto& button : buttons) {
            button->callback();
        }
    }
    void moveTo(sf::Vector2f position) {
        for (auto& button : buttons) {
            sf::Vector2f pos = button->getPosition();
            button->setPosition(pos + (position - originalPosition));
        }
    }
};

class SongSelectionMenu {
    std::deque<std::unique_ptr<MenuButton>> buttons;
    std::vector<OsuFile> osusPaths;
    sf::Font& font;
    int offset = 0;
    const int limit = 11;
    GameState& localstate;
    Game& localGame;
    Audio& audio;
    void update(int lim) {
        buttons.clear();
        for (int i = 0; i< lim; i++) {
            std::ostringstream oss;
            OsuFile& osufile = osusPaths.at(offset + i);
            oss << osufile.artist << " - " << osufile.title << " [" << osufile.version << "]"; // (" << osufile.creator << ")";
            std::string mapname = oss.str();
            buttons.push_back(std::make_unique<MenuButton>(sf::Vector2f(10.f,10.f+i*60.f), sf::Vector2f(1260.f,60.f), sf::Color(102, 102, 153, 180), font, mapname, [=]{
                if (audio.checkAudioIsActive()) {
                    audio.stopAudio();
                }
                std::cout << mapname << '\n';
                std::cout << fs::path(osufile.filepath.parent_path() / osufile.audioFileName).string() << '\n';
                audio.playAudio(osufile.filepath.parent_path() / osufile.audioFileName);
                localGame.load(osusPaths.at(offset + i));
                localstate = GameState::GamePlaying;
            }));
        }
    }
public:
    SongSelectionMenu(sf::Font& font, GameState& state, Audio& audio, Game& game) : font(font), localstate(state), audio(audio), localGame(game) {
        //osusPaths = std::move(listDirOsus(fs::path("C:\\cpps\\test2\\Songs")));
        osusPaths = std::move(listDirOsus(get_executable_path() / "Songs"));
        if (osusPaths.size() >= limit) {
            update(limit);
        } else {
            update(static_cast<int>(osusPaths.size()));
        }
    }
    void show(sf::RenderWindow& window, sf::Vector2f position) {
        for (auto& button : buttons) {
            button->onHover(position);
            button->draw(window);
        }
        if (localstate != GameState::MenuSongSelection) {
            offset = 0;
        }
    }
    void click() {
        for (auto& button : buttons) {
            button->callback();
        }
    }
    void down() {
        if (localstate == GameState::MenuSongSelection && offset + limit <= static_cast<float>(osusPaths.size()) - 1.f){
            offset++;
            update(limit);
        }
    }
    void up() {
        if (localstate == GameState::MenuSongSelection && offset > 0 && static_cast<float>(osusPaths.size()) - 1.f >= limit) {
            offset--;
            update(limit);
        }
    }
};