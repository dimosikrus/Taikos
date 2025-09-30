using fn = int;

#include <iostream>
#include <memory>

#include <SFML/Graphics.hpp>

#include "engine/gamestates.hpp"
#include "engine/audio/audio.hpp"
#include "engine/filesystem/filesystem.hpp"
// #include "menu/menu.hpp"
#include "engine/menu/menu.hpp"
#include "engine/game/game.hpp"

static bool spriteCollision(const sf::Sprite& sprite, const sf::Vector2f position) {
    return sprite.getGlobalBounds().contains(position);
};

static void events(std::optional<sf::Event> event, sf::RenderWindow& window,
            sf::Vector2f& mousePosition, Audio& audio, GameState& gameState, SpecialState& specialState, FloatingMenu& flmenu, SongSelectionMenu& ssm) {
    sf::Mouse::Button mButton;
    if (event->is<sf::Event::Closed>()) window.close();
    if (event->is<sf::Event::MouseButtonPressed>()) {
        mButton = event->getIf<sf::Event::MouseButtonPressed>()->button;
        switch(mButton) {
            case sf::Mouse::Button::Left:
                if (gameState == GameState::MenuIntro) {
                    std::cout << "INTROMENUCLICK" << '\n';
                    flmenu.click();
                } else if (gameState == GameState::MenuSongSelection) {
                    std::cout << "SONGSELECTIONCLICK" << '\n';
                    ssm.click();
                }
                break;
            default: break;
        }
    }
    if (event->is<sf::Event::MouseMoved>()) {
        mousePosition = sf::Vector2f(event->getIf<sf::Event::MouseMoved>()->position);
    }
    if (event->is<sf::Event::MouseWheelScrolled>()) {
        float scrolled = event->getIf<sf::Event::MouseWheelScrolled>()->delta;
        if (gameState == GameState::MenuSongSelection) {
            if (specialState != SpecialState::Alt) {
                if (scrolled > 0) {
                    ssm.up();
                } else if (scrolled < 0) {
                    ssm.down();
                }
            }
            if (specialState == SpecialState::Alt) {
                if (scrolled > 0) {
                    audio.upAudioVol();
                } else if (scrolled < 0) {
                    audio.downAudioVol();
                }
            }
        }
        if (gameState == GameState::GamePlaying) {
            if (specialState == SpecialState::Alt) {
                if (scrolled > 0) {
                    audio.upAudioVol();
                }
                else if (scrolled < 0) {
                    audio.downAudioVol();
                }
            }
        }
    }
    if (event->is<sf::Event::KeyPressed>()) {
        sf::Keyboard::Key code = event->getIf<sf::Event::KeyPressed>()->code;
        switch(code) {
        //case sf::Keyboard::Key::Escape: window.close(); break;
            case sf::Keyboard::Key::Escape:
                if (gameState == GameState::MenuSongSelection) { gameState = GameState::MenuIntro; audio.stopAudio(); break; }
                if (gameState == GameState::GamePlaying) { gameState = GameState::MenuSongSelection; break; }
                break;
            case sf::Keyboard::Key::D: audio.playSound("hitZ"); break;
            case sf::Keyboard::Key::F: audio.playSound("hitX"); break;
            case sf::Keyboard::Key::J: audio.playSound("hitX"); break;
            case sf::Keyboard::Key::K: audio.playSound("hitZ"); break;
            case sf::Keyboard::Key::Up: ssm.up(); break;
            case sf::Keyboard::Key::Down: ssm.down(); break;
            case sf::Keyboard::Key::LAlt: specialState = SpecialState::Alt; break;
            case sf::Keyboard::Key::RAlt: specialState = SpecialState::Alt; break;
            case sf::Keyboard::Key::LShift: specialState = SpecialState::Shift; break;
            case sf::Keyboard::Key::LControl: specialState = SpecialState::Control; break;
            default: break;
        }
    }
    if (event->is<sf::Event::KeyReleased>()) {
        sf::Keyboard::Key code = event->getIf<sf::Event::KeyReleased>()->code;
        switch(code) {
            case sf::Keyboard::Key::LAlt: specialState = SpecialState::None; break;
            case sf::Keyboard::Key::RAlt: specialState = SpecialState::None; break;
            case sf::Keyboard::Key::LShift: specialState = SpecialState::None; break;
            case sf::Keyboard::Key::LControl: specialState = SpecialState::None; break;
            default: break;
        }
    }
};

fn main() {
    GameState gameState = GameState::Loading;
    SpecialState specialState = SpecialState::None;

    Audio audio;
    audio.loadDefault();
    audio.setSoundsVolume(0.4f);

    sf::VideoMode video_mode(sf::Vector2u(1280,720));
    constexpr auto window_style = sf::Style::Titlebar | sf::Style::Close;
    sf::RenderWindow window(video_mode, "SFML Title", window_style);
    sf::Clock clock;

    sf::Font BASICFONT(get_executable_path() / "assets\\arial.ttf");

    sf::Texture texture;
    if (!texture.loadFromFile(get_executable_path() / "assets\\image.png")) {
        std::cout << "Error load texture" << std::endl;
        return -1;
    }
    sf::Sprite testsprite(texture);
    testsprite.setScale(sf::Vector2f(6.f,3.f));
    sf::Vector2f mousePosition(-0.1f,-0.1f);
    FloatingMenu flmenu(sf::Vector2f(200.f,200.f), BASICFONT, gameState);
    float dt = 1;
    OsuFile osuempty;
    Game game(audio, osuempty);
    SongSelectionMenu ssm(BASICFONT, gameState, audio, game);

    gameState = GameState::MenuIntro;
    while (window.isOpen()) {
        clock.start();
        while (std::optional<sf::Event> event = window.pollEvent()) events(event, window, mousePosition, audio, gameState, specialState, flmenu, ssm);
        //audio.updateEngine();
        window.clear(sf::Color::Black);

        window.draw(testsprite);
        // std::cout << mousePosition.x << " | " << mousePosition.y << '\n';
        
        switch(gameState) {
            case GameState::Loading:
                break;
            case GameState::MenuIntro:
                flmenu.draw(window, mousePosition);
                break;
            case GameState::MenuSongSelection:
                ssm.show(window, mousePosition);
                break;
            case GameState::GamePlaying:
                game.update();
                game.show(window);
                break;
            case GameState::SettingsMain: [[fallthrough]];
            case GameState::SettingsAudio: [[fallthrough]];
            case GameState::Exit:
                window.close();
                break;
            default: break;
        }

        window.display();
        dt = static_cast<float>(clock.getElapsedTime().asMicroseconds())/1000;
        clock.reset();
    }

    window.close();
    return 0;
};

