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
#include "engine/configmanager/configmanager.hpp"

// FPS 2
#include <chrono>
#include <thread>
//

#ifdef _WIN32
    #include <windows.h>
#endif

static bool spriteCollision(const sf::Sprite& sprite, const sf::Vector2f position) {
    return sprite.getGlobalBounds().contains(position);
};

float avg(const float arr[], int size) {
    if (size <= 0) {
        return 0.0f;
    }

    float sum = 0.0f;
    for (int i = 0; i < size; ++i) {
        sum += arr[i];
    }

    return sum / static_cast<float>(size);
}
void pushValueFpS(float arr[], int size, float val) {
    for (int i = size-2; i >= 0; i--) {
        arr[i + 1] = arr[i];
    }
    *arr = val;
}

void set_high_priority() {
#ifdef _WIN32
    HANDLE hProcess = GetCurrentProcess();
    
    // Set Priority HIGH_PRIORITY_CLASS
    if (SetPriorityClass(hProcess, HIGH_PRIORITY_CLASS)) {
        std::cout << "Windows Priority now is High.\n";
    } else {
        std::cerr << "Error set priority: " << GetLastError() << "\n";
    }
#else
    // macos and linux ignore this code
#endif
}

class FPSCounter {
private:
    using clock = std::chrono::steady_clock;
    using time_point = clock::time_point;
    using duration = std::chrono::duration<float>;

    time_point last_time;
    float current_fps = 0.0f;
    int frame_count = 0;
    float update_interval = 0.5f; // Update fps every 0.5s
    float accumulated_time = 0.0f;

public:
    FPSCounter() : last_time(clock::now()) {}

    void update() {
        auto now = clock::now();
        duration delta = now - last_time;
        last_time = now;

        float dt = delta.count();
        accumulated_time += dt;
        frame_count++;

        if (accumulated_time >= update_interval) {
            current_fps = static_cast<float>(frame_count) / accumulated_time;
            frame_count = 0;
            accumulated_time = 0.0f;
        }
    }

    float get_fps() const { return current_fps; }
};

static void events(std::optional<sf::Event> event, sf::RenderWindow& window,
            sf::Vector2f& mousePosition, Audio& audio, GameState& gameState,
            SpecialState& specialState, FloatingMenu& flmenu, SongSelectionMenu& ssm,
            Game& game, VolumeGraph& volGraph, ConfigFile& configfile) {
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
        if (gameState == GameState::MenuSongSelection || gameState == GameState::GamePlaying) {
            if (gameState == GameState::MenuSongSelection && specialState == SpecialState::None) {
                if (scrolled > 0) ssm.up();
                else if (scrolled < 0) ssm.down();
            }
            if (specialState == SpecialState::Alt) {
                volGraph.show();
                if (scrolled > 0) audio.upAudioVol();
                else if (scrolled < 0) audio.downAudioVol();
            }
            if (specialState == SpecialState::Control) {
                volGraph.show();
                if (scrolled > 0) audio.upSoundsVol();
                else if (scrolled < 0) audio.downSoundsVol();
            }
        }
    }

    if (event->is<sf::Event::KeyPressed>()) {
        sf::Keyboard::Key code = event->getIf<sf::Event::KeyPressed>()->code;
        Taps taps;
        switch(code) {
        //case sf::Keyboard::Key::Escape: window.close(); break;
            case sf::Keyboard::Key::F5:
                configfile.loadConfig();
                break;
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
            case sf::Keyboard::Key::R:
                audio.resetPos();
                game.reset();
                break;
            case sf::Keyboard::Key::S:
                // audio.playSound("hitZ");
                taps.time = audio.getMusicPos();
                taps.hs = HitSound::Whistle;
                taps.down = TapsDir::Down;
                taps.key = KeyssKey::LK;
                game.pushTap(taps);
                break;
            case sf::Keyboard::Key::D:
                // audio.playSound("hitX");
                taps.time = audio.getMusicPos();
                taps.hs = HitSound::Normal;
                taps.down = TapsDir::Down;
                taps.key = KeyssKey::LD;
                game.pushTap(taps);
                break;
            case sf::Keyboard::Key::K:
                // audio.playSound("hitX");
                taps.time = audio.getMusicPos();
                taps.hs = HitSound::Normal;
                taps.down = TapsDir::Down;
                taps.key = KeyssKey::RD;
                game.pushTap(taps);
                break;
            case sf::Keyboard::Key::L:
                // audio.playSound("hitZ");
                taps.time = audio.getMusicPos();
                taps.hs = HitSound::Whistle;
                taps.down = TapsDir::Down;
                taps.key = KeyssKey::RK;
                game.pushTap(taps);
                break;
            case sf::Keyboard::Key::U:
                audio.setPos(audio.GetMusicLengthD_S() - 2.0);
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

    if (event->is<sf::Event::FocusLost>()) {
        Taps tap;
        tap.key = KeyssKey::CLEAR;
        game.pushTap(tap);
        if (gameState == GameState::GamePlaying) {
            gameState = GameState::GamePause;
        }
    }

    if (event->is<sf::Event::KeyReleased>()) {
        sf::Keyboard::Key code = event->getIf<sf::Event::KeyReleased>()->code;
        Taps taps;
        switch(code) {
            case sf::Keyboard::Key::S:
                taps.time = audio.getMusicPos();
                taps.hs = HitSound::Whistle;
                taps.down = TapsDir::Up;
                taps.key = KeyssKey::LK;
                game.pushTap(taps);
                break;
            case sf::Keyboard::Key::D:
                taps.time = audio.getMusicPos();
                taps.hs = HitSound::Normal;
                taps.down = TapsDir::Up;
                taps.key = KeyssKey::LD;
                game.pushTap(taps);
                break;
            case sf::Keyboard::Key::K:
                taps.time = audio.getMusicPos();
                taps.hs = HitSound::Normal;
                taps.down = TapsDir::Up;
                taps.key = KeyssKey::RD;
                game.pushTap(taps);
                break;
            case sf::Keyboard::Key::L:
                taps.time = audio.getMusicPos();
                taps.hs = HitSound::Whistle;
                taps.down = TapsDir::Up;
                taps.key = KeyssKey::RK;
                game.pushTap(taps);
                break;
            case sf::Keyboard::Key::LAlt: specialState = SpecialState::None; break;
            case sf::Keyboard::Key::RAlt: specialState = SpecialState::None; break;
            case sf::Keyboard::Key::LShift: specialState = SpecialState::None; break;
            case sf::Keyboard::Key::LControl: specialState = SpecialState::None; break;
            default: break;
        }
    }
};

fn main() {
    // set_high_priority();
    Logger logger;

    GameState gameState = GameState::Loading;
    SpecialState specialState = SpecialState::None;

    logger.log(LogLevel::INFO, "INFO Logger");
    logger.log(LogLevel::WARN, "WARNING Logger");
    logger.log(LogLevel::ERR, "ERROR Logger");
    logger.log(LogLevel::DEBUG, "DEBUG Logger");

    Audio audio;
    audio.loadDefault();
    ConfigFile configFile;
    audio.setSoundsVolume(configFile.getSoundsVol());
    audio.setAudioVolume(configFile.getMisucVol());

    logger.log(LogLevel::DEBUG, "AUDIO INITIALIZED");

    sf::VideoMode video_mode({ 1280,720 });
    constexpr auto window_style = sf::Style::Titlebar | sf::Style::Close;
    sf::RenderWindow window(video_mode, "Taikos Loading", window_style);
    sf::Vector2f windowFSize({static_cast<float>(window.getSize().x),static_cast<float>(window.getSize().y)});
    sf::Clock clock;
    logger.log(LogLevel::DEBUG, "WINDOW INITIALIZED");
    // window.setFramerateLimit(240);

    // sf::Font BASICFONT(get_executable_path() / "assets\\arial.ttf");
    sf::Font BASICFONT(get_executable_path() / fs::path("assets/arial.ttf"));
    
    GameState prevState = gameState;

    sf::RectangleShape pauseRect(windowFSize);

    pauseRect.setPosition({ 0, 0 });
    pauseRect.setFillColor({ 1, 1, 1, 152 });

    float fps = 0;
    float dt = 1;
    int fpsDelayCounter = 0;
    int msGraphCounter = 0;

    sf::Vector2f mousePosition(-0.1f,-0.1f);

    OsuFile osuempty;

    FloatingMenu flmenu({ 200.f,200.f }, BASICFONT, gameState);
    ResultScreen results(BASICFONT);
    Game game(audio, osuempty, BASICFONT, results, gameState, window, configFile);
    SongSelectionMenu ssm(BASICFONT, gameState, audio, game);

    VolumeGraph volGraph(audio);

    sf::Text fpsText2(BASICFONT, "FPS: 0");
    sf::Text delayText(BASICFONT, "FPS: 0");
    fpsText2.setPosition({ 15.f, 15.f });
    delayText.setPosition({ 1200.f, 700.f });
    fpsText2.setCharacterSize(18u);
    delayText.setCharacterSize(16u);

    float mss[30];
    for (int i = 0; i < 30; i++) { // fill ms array
        mss[i] = 0;
    }
    FPSCounter ffpss;

    sf::RectangleShape delayGraph({ 0.f, 4.f });
    delayGraph.setFillColor(sf::Color::Red);
    delayGraph.setPosition({ 1280.f, 716.f });

    gameState = GameState::MenuIntro;
    logger.log(LogLevel::DEBUG, "Window events handler running...");
    window.setTitle("Taikos Idle");

    while (window.isOpen()) {
        ffpss.update();
        clock.start();
        while (std::optional<sf::Event> event = window.pollEvent()) events(event, window, mousePosition, audio, gameState, specialState, flmenu, ssm, game, volGraph, configFile);
        //audio.updateEngine();
        window.clear(sf::Color::Black);

        pushValueFpS(mss, 10, dt);
        if (fpsDelayCounter > 100) {
            fpsDelayCounter = 0;
            
            std::ostringstream oss3;
            oss3 << "ms: " << avg(mss, 30);
            delayText.setString(oss3.str());
        }
        if (msGraphCounter > 200) {
            msGraphCounter = 0;
            
            float mssval = avg(mss, 30);
            delayGraph.setPosition({ 1280.f - (mssval * 100.f), 716.f });
            delayGraph.setSize({ mssval * 100.f, 4.f });
        }

        std::ostringstream oss2;
        oss2 << "FPS: " << ffpss.get_fps();
        fpsText2.setString(oss2.str());
        

        fpsDelayCounter++;
        msGraphCounter++;
        window.draw(fpsText2);
        window.draw(delayText);
        window.draw(delayGraph);
        volGraph.update();
        
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
                game.update(dt);
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
                results.draw(window, dt);
                break;
            case GameState::SettingsMain: [[fallthrough]];
            case GameState::SettingsAudio: [[fallthrough]];
            case GameState::Exit:
                window.close();
                break;
            default: break;
        }

        volGraph.draw(window);

        window.display();
        dt = static_cast<float>(clock.getElapsedTime().asMicroseconds())/1000;
        fps = 1000 / (dt > 0 ? dt : 1);
        clock.reset();
    }

    window.close();
    return 0;
};

