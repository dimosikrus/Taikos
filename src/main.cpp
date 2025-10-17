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
#include "engine/game/resultScreen.hpp"
#include "engine/logger.hpp"

static bool spriteCollision(const sf::Sprite& sprite, const sf::Vector2f position) {
    return sprite.getGlobalBounds().contains(position);
};

static void events(std::optional<sf::Event> event, sf::RenderWindow& window,
            sf::Vector2f& mousePosition, Audio& audio, GameState& gameState,
            SpecialState& specialState, FloatingMenu& flmenu, SongSelectionMenu& ssm,
            Game& game) {
    sf::Mouse::Button mButton;

    if (event->is<sf::Event::Closed>()) window.close();

    if (event->is<sf::Event::MouseButtonPressed>()) {
        mButton = event->getIf<sf::Event::MouseButtonPressed>()->button;
        switch(mButton) {
            case sf::Mouse::Button::Left:
                if (gameState == GameState::MenuIntro) {
                    flmenu.click();
                } else if (gameState == GameState::MenuSongSelection) {
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
                    audio.upSoundsVol();
                }
                else if (scrolled < 0) {
                    audio.downAudioVol();
                    audio.downSoundsVol();
                }
            }
        }
    }

    if (event->is<sf::Event::KeyPressed>()) {
        sf::Keyboard::Key code = event->getIf<sf::Event::KeyPressed>()->code;
        Taps taps;
        switch(code) {
        //case sf::Keyboard::Key::Escape: window.close(); break;
            case sf::Keyboard::Key::Escape:
                if (gameState == GameState::MenuSongSelection) { 
                    gameState = GameState::MenuIntro;
                    audio.stopAudio();
                    break;
                }
                if (gameState == GameState::GamePlaying) {
                    gameState = GameState::GamePause;
                    if (!audio.paused()) {
                        audio.pauseAudio();
                    }
                    break;
                }
                if (gameState == GameState::GamePause) {
                    gameState = GameState::GamePlaying;
                    if (audio.paused()) {
                        audio.unPauseAudio();
                    }
                    break;
                }
                if (gameState == GameState::GameResults) {
                    gameState = GameState::MenuSongSelection;
                    break;
                }
                break;
            case sf::Keyboard::Key::Q:
                gameState = GameState::MenuSongSelection;
                if (audio.paused()) {
                    audio.unPauseAudio();
                }
                break;
            case sf::Keyboard::Key::E:
                audio.resetPos();
                game.reset();
                break;
            case sf::Keyboard::Key::D:
                audio.playSound("hitZ");
                taps.time = audio.getPos();
                taps.hs = HitSound::Whistle;
                game.pushTap(taps);
                break;
            case sf::Keyboard::Key::F:
                audio.playSound("hitX");
                taps.time = audio.getPos();
                taps.hs = HitSound::Normal;
                game.pushTap(taps);
                break;
            case sf::Keyboard::Key::J:
                audio.playSound("hitX");
                taps.time = audio.getPos();
                taps.hs = HitSound::Normal;
                game.pushTap(taps);
                break;
            case sf::Keyboard::Key::K:
                audio.playSound("hitZ");
                taps.time = audio.getPos();
                taps.hs = HitSound::Whistle;
                game.pushTap(taps);
                break;
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
    Logger logger;

    GameState gameState = GameState::Loading;
    SpecialState specialState = SpecialState::None;

    logger.log(LogLevel::INFO, "INFO Logger");
    logger.log(LogLevel::WARN, "WARNING Logger");
    logger.log(LogLevel::ERR, "ERROR Logger");
    logger.log(LogLevel::DEBUG, "DEBUG Logger");

    Audio audio;
    audio.loadDefault();
    audio.setSoundsVolume(0.4f);

    logger.log(LogLevel::DEBUG, "AUDIO INITIALIZED");

    sf::VideoMode video_mode({ 1280,720 });
    constexpr auto window_style = sf::Style::Titlebar | sf::Style::Close;
    sf::RenderWindow window(video_mode, "Taikos Loading", window_style);
    sf::Vector2f windowFSize({static_cast<float>(window.getSize().x),static_cast<float>(window.getSize().y)});
    sf::Clock clock;
    logger.log(LogLevel::DEBUG, "WINDOW INITIALIZED");

    sf::Font BASICFONT(get_executable_path() / "assets\\arial.ttf");;
    
    GameState prevState = gameState;

    sf::RectangleShape pauseRect(windowFSize);

    pauseRect.setPosition({ 0, 0 });
    pauseRect.setFillColor({ 1, 1, 1, 152 });

    float fps = 0;
    float dt = 1;
    int fpsDelayCounter = 0;

    sf::Vector2f mousePosition(-0.1f,-0.1f);

    OsuFile osuempty;

    FloatingMenu flmenu({ 200.f,200.f }, BASICFONT, gameState);
    ResultScreen results(BASICFONT);
    Game game(audio, osuempty, BASICFONT, results, gameState);
    SongSelectionMenu ssm(BASICFONT, gameState, audio, game);

    sf::Text fpsText(BASICFONT, "FPS: 0");
    fpsText.setPosition({ 15.f, 15.f });

    gameState = GameState::MenuIntro;
    logger.log(LogLevel::DEBUG, "Window events handler running...");
    window.setTitle("Taikos Idle");

    while (window.isOpen()) {
        clock.start();
        while (std::optional<sf::Event> event = window.pollEvent()) events(event, window, mousePosition, audio, gameState, specialState, flmenu, ssm, game);
        //audio.updateEngine();
        window.clear(sf::Color::Black);

        if (fpsDelayCounter > 500) {
            std::ostringstream oss;
            oss << "FPS: " << fps;
            fpsText.setString(oss.str());
            fpsDelayCounter = 0;
        }

        fpsDelayCounter++;
        window.draw(fpsText);
        
        switch(gameState) {
            case GameState::Loading:
                break;
            case GameState::MenuIntro:
                if (prevState != gameState) {
                    prevState = gameState;
                    window.setTitle("Taikos Idle");
                }
                flmenu.draw(window, mousePosition);
                break;
            case GameState::MenuSongSelection:
                if (prevState != gameState) {
                    prevState = gameState;
                    window.setTitle("Taikos Song Selection");
                }
                ssm.show(window, mousePosition);
                break;
            case GameState::GamePlaying:
                if (prevState != gameState) {
                    prevState = gameState;
                    window.setTitle("Taikos " + game.getSelected());
                }
                game.update();
                game.show(window);
                break;
            case GameState::GamePause:
                if (prevState != gameState) {
                    prevState = gameState;
                    window.setTitle("Taikos Pause");
                }
                game.show(window);
                window.draw(pauseRect);
                audio.pauseAudio();
                break;
            case GameState::GameResults:
                if (prevState != gameState) {
                    prevState = gameState;
                    window.setTitle("Taikos Results");
                }
                results.draw(window);
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
        fps = 1000 / (dt > 0 ? dt : 1);
        clock.reset();
    }

    window.close();
    return 0;
};

