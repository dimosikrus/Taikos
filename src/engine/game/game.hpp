#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>
#include <deque>
#include <memory>
#include <cmath>
#include "../audio/audio.hpp"
#include "../gamestates.hpp"
#include "osuTypes.hpp"
#include "resultScreen.hpp"
#include "../filesystem/filesystem.hpp"
#include "../animations/animations.hpp"
#include "../animations/easing.h" 
#include "../configmanager/configmanager.hpp"

class DrawableObj {
	float radius;
	float border = 5.f;
	sf::CircleShape circlefill;
	sf::CircleShape circleborder;
	sf::Vector2f pos;

	sf::Texture& hct;
	sf::Texture& hcot;
	sf::Sprite hcs;
	sf::Sprite hcos;
public:
	DrawableObj (sf::Vector2f position, sf::Color color, float radius, sf::Texture& hct, sf::Texture& hcot) :
			pos(position), circlefill(radius), circleborder(radius), radius(radius),
			hct(hct), hcot(hcot), hcs(hct), hcos(hcot) {
		hcs.setTexture(hct);
		hcos.setTexture(hcot);
		hcs.setColor(color);

		hcs.setScale ({ .74f + (radius - 40.f) / 100, .74f + (radius - 40.f) / 100 });
		hcos.setScale({ .74f + (radius - 40.f) / 100, .74f + (radius - 40.f) / 100 });

		sf::FloatRect sizeg  = hcs.getGlobalBounds();
		sf::FloatRect sizego = hcos.getGlobalBounds();

		this->pos = sf::Vector2f(pos.x - sizeg.size.x / 2, pos.y - sizeg.size.y / 2);
		hcs.setPosition (pos);
		hcos.setPosition(pos);
	}

	void draw(sf::RenderWindow& window) {
		window.draw(hcs);
		window.draw(hcos);
	}
};

class HitCirclesEffect {
	float radius;
	float border = 5.f;
	
	sf::CircleShape circlefill;
	sf::CircleShape circleborder;
	sf::Vector2f pos;

	sf::Texture& hct;
	sf::Texture& hcot;
	sf::Sprite hcs;
	sf::Sprite hcos;

	float lifetime = 0.f;
public:
	bool active = true;
	HitCirclesEffect (sf::Vector2f position, sf::Color color, float radius, sf::Texture& hct, sf::Texture& hcot) :
			pos(position), circlefill(radius), circleborder(radius), radius(radius),
			hct(hct), hcot(hcot), hcs(hct), hcos(hcot) {
		hcs.setTexture(hct);
		hcos.setTexture(hcot);
		hcs.setColor(color);

		hcs.setScale ({ .74f + (radius - 40.f) / 100, .74f + (radius - 40.f) / 100 });
		hcos.setScale({ .74f + (radius - 40.f) / 100, .74f + (radius - 40.f) / 100 });
		
		sf::FloatRect sizeg  = hcs.getGlobalBounds();
		sf::FloatRect sizego = hcos.getGlobalBounds();

		this->pos = sf::Vector2f(pos.x - sizeg.size.x / 2, pos.y - sizeg.size.y / 2);
		hcs.setPosition (pos);
		hcos.setPosition(pos);
	}
	void draw(sf::RenderWindow& window) {
		if (active) {
			window.draw(hcs);
			window.draw(hcos);
		}
	}
	void update(float dt) {
		lifetime += dt;
		if (lifetime >= 727.f) {
			active = false;
		}
		sf::Vector2f position {
			pos.x - Anim::easeInBack   (lifetime / 727.f) * 100.f,
			pos.y + Anim::easeOutBounce(lifetime / 727.f) * 350.f
		};
		hcs.setPosition(position);
		hcos.setPosition(position);
	}
};

class Game {
	OsuFile& osufile;
	GameState& localState;
	ConfigFile& configFile;
	int offset = 0;
	std::deque<DrawableObj> drawable;
	std::vector<HitObject> hitobjects;
	std::vector<HitCirclesEffect> hitCirclesEffect;
	Audio& audio;
	int startOffset = 0;
	bool audioStarted = false;
	bool atimerStarted = false;
	bool gameIsRunning = true;
	sf::Clock prevTimer;
	sf::Font& font;
	sf::Text text1;
	sf::Text text2;
	sf::Text text3;
	sf::RectangleShape centerRect;
	// sf::RectangleShape centerRectH;
	sf::RectangleShape horRect1;
	sf::RectangleShape horRect2;
	sf::RectangleShape x0Rect;
	sf::RectangleShape x50Rect;
	sf::RectangleShape x100Rect;
	sf::RectangleShape x300Rect;
	ResultScreen& localResults;
	sf::RenderWindow& window;
	bool ofScanning = true;
	std::vector<Taps> taps;
	sf::Vector2f mousePosition{ 0.f, 0.f };
	Keyss keyss;
	Keyss keyss2;

	sf::RectangleShape key1r;
	sf::RectangleShape key2r;
	sf::RectangleShape key3r;
	sf::RectangleShape key4r;

	bool checkHit(int HitObjectTime, int TapTime) {
		if (TapTime > HitObjectTime - 20 && TapTime < HitObjectTime + 20) {
			score.x300++;
			score.score += score.combo != 0 ? 300 * score.combo : 300;
			return true;
		}

		if (TapTime > HitObjectTime - 40 && TapTime < HitObjectTime + 40) {
			score.x100++;
			score.score += score.combo != 0 ? 100 * score.combo : 100;
			return true;
		}

		if (TapTime > HitObjectTime - 55 && TapTime < HitObjectTime + 55) {
			score.x50++;
			score.score += score.combo != 0 ? 50 * score.combo : 50;
			return true;
		}

		score.x0++;
		return false;
	}

	void checkCombo() {
		if (score.maxCombo < score.combo) {
			score.maxCombo = score.combo;
		}

		if (comboMissBuffer < score.x0) {
			score.combo = 0;
			comboMissBuffer = score.x0;
		}
	}

	void setResults() {
		ResultRows rr{};
		rr.x300 = score.x300;
		rr.x100 = score.x100;
		rr.x50 = score.x50;
		rr.x0 = score.x0;
		rr.xMaxCombo = score.maxCombo;
		rr.xScore = score.score;
		localResults.updateRows(rr);
	}

	sf::Texture hct;
	sf::Texture hcot;
public:
	int REDS = 0,
		BLUES = 0,
		OTHERS = 0;
	int comboMissBuffer = 0;
	Score score;

	Game(Audio& audio, OsuFile& osufile, sf::Font& font, ResultScreen& results, GameState& state, sf::RenderWindow& window, ConfigFile& configFile) : audio(audio), window(window), osufile(osufile), font(font), text1(font), text2(font), text3(font),
			centerRect({ 4.f, 140.f }), /*centerRectH({ 1280.f, 4.f }),*/ x0Rect({ 140.f, 60.f }), x50Rect({ 110.f, 60.f }), x100Rect({ 80.f, 60.f }), x300Rect({ 40.f, 60.f }),
			localResults(results), localState(state), configFile(configFile), horRect1({ 1280.f, 4.f }), horRect2({ 1280.f, 4.f }),
			key1r({ 30.f, 4.f }), key2r({ 30.f, 4.f }), key3r({ 30.f, 4.f }), key4r({ 30.f, 4.f }) {
		centerRect.setFillColor(sf::Color::Magenta);
		// centerRectH.setFillColor(sf::Color::White);
		horRect1.setFillColor(sf::Color::White);
		horRect2.setFillColor(sf::Color::White);
		x0Rect.setFillColor(sf::Color::Red);
		x50Rect.setFillColor(sf::Color::Yellow);
		x100Rect.setFillColor(sf::Color::Green);
		x300Rect.setFillColor(sf::Color::Cyan);
		key1r.setFillColor(configFile.getKaColour());
		key2r.setFillColor(configFile.getDonColour());
		key3r.setFillColor(configFile.getDonColour());
		key4r.setFillColor(configFile.getKaColour());
		centerRect.setPosition({ 148.f, 140.f });
		// centerRectH.setPosition({ 0.f, 210.f });
		x0Rect.setPosition({ 80.f, 180.f }); // 150-(140/2) = 150-70 = 80
		x50Rect.setPosition({ 95.f, 180.f }); // 150-(110/2) = 150-55 = 95
		x100Rect.setPosition({ 110.f, 180.f }); // 150-(80/2) = 150-40 = 110
		x300Rect.setPosition({ 130.f, 180.f }); // 150-(40/2) = 150-20 = 130
		key1r.setPosition({ 200.f, 400.f });
		key2r.setPosition({ 240.f, 400.f });
		key3r.setPosition({ 280.f, 400.f });
		key4r.setPosition({ 320.f, 400.f });
		text1.setPosition({ 15.f, 40.f });
		text2.setPosition({ 15.f, 65.f });
		text3.setPosition({ 400.f, 15.f });
		horRect1.setPosition({ 0.f, 147.f });
		horRect2.setPosition({ 0.f, 273.f });

		if (!hct.loadFromFile(fs::path(get_executable_path() / "assets/hitcircle.png"))) {
			std::cout << "NOT assets/hitcircle.png" << '\n';
		}
		if (!hcot.loadFromFile(fs::path(get_executable_path() / "assets/hitcircleover.png"))) {
			std::cout << "NOT assets/hitcircleover.png" << '\n';
		}
	};

	void pushTap(Taps tapz) {
		taps.emplace_back(tapz);
	}

	void changeClickDown() {
		keyss.LK ? key1r.setPosition({ 200.f, 410.f }) : key1r.setPosition({ 200.f, 400.f });
		keyss.LD ? key2r.setPosition({ 240.f, 410.f }) : key2r.setPosition({ 240.f, 400.f });
		keyss.RD ? key3r.setPosition({ 280.f, 410.f }) : key3r.setPosition({ 280.f, 400.f });
		keyss.RK ? key4r.setPosition({ 320.f, 410.f }) : key4r.setPosition({ 320.f, 400.f });

		if (keyss2.LK != keyss.LK) {
			if (keyss.LK) PlayRed();
			keyss2.LK = keyss.LK;
		}
		if (keyss2.LD != keyss.LD) {
			if (keyss.LD) PlayBlue();
			keyss2.LD = keyss.LD;
		}
		if (keyss2.RD != keyss.RD) {
			if (keyss.RD) PlayBlue();
			keyss2.RD = keyss.RD;
		}
		if (keyss2.RK != keyss.RK) {
			if (keyss.RK) PlayRed();
			keyss2.RK = keyss.RK;
		}

	}
	
	void PlayRed() {
		audio.playSound("hitZ");
	}
	void PlayBlue() {
		audio.playSound("hitX");
	}

	void show(sf::RenderWindow& window) {
		window.draw(x0Rect);
		window.draw(x50Rect);
		window.draw(x100Rect);
		window.draw(x300Rect);
		window.draw(centerRect);
		// window.draw(centerRectH);
		window.draw(horRect1);
		window.draw(horRect2);
		//for (DrawableObj obj : drawable) {
		//	obj.draw(window);
		//}
		for (size_t i = drawable.size(); i > 0; i--) {
			drawable.at(i-1).draw(window);
		}
		for (size_t i = hitCirclesEffect.size(); i > 0; i--) {
			hitCirclesEffect.at(i-1).draw(window);
		}
		window.draw(text1);
		window.draw(text2);
		window.draw(text3);

		window.draw(key1r);
		window.draw(key2r);
		window.draw(key3r);
		window.draw(key4r);
	}

	bool Running() const { return gameIsRunning;  }

	void reset() {
		offset = 0;
		ofScanning = true;
		REDS = 0;
		BLUES = 0;
		OTHERS = 0;
		score.x0 = 0;
		score.x50 = 0;
		score.x100 = 0;
		score.x300 = 0;
		score.combo = 0;
		comboMissBuffer = 0;
		score.maxCombo = 0;
		score.score = 0;
		audioStarted = false;
		atimerStarted = false;
		for (auto& hobj : hitobjects) {
			hobj.hitted = false;
			hobj.next = false;
		}
	}

	sf::Vector2f rotatePoint(float x, float y, float cx, float cy, float angleDeg) {
		const float angle = angleDeg * 3.141f / 180.f;
		
		const float dx = x - cx;
		const float dy = y - cy;
		
		const float xPrime = dx * cos(angle) - dy * sin(angle);
		const float yPrime = dx * sin(angle) + dy * cos(angle);

		return sf::Vector2f({ xPrime + cx, yPrime + cy });
	}

	void update(float dt) {
		if (audio.getMusicPos() - 727 > hitobjects.back().time) {
			gameIsRunning = false;
			std::cout << "Map is End." << "\n";
			localState = GameState::GameResults;
			setResults();
		}

		if (startOffset <= 3000) {
			if (!atimerStarted) {
				if (audio.checkMusicIsActive()) {
					audio.stopAudio();
				}
				gameIsRunning = true;
				prevTimer.start();
				atimerStarted = true;
			}

			if (prevTimer.getElapsedTime().asMilliseconds() >= startOffset && !audioStarted) {
				prevTimer.stop();
				prevTimer.reset();
				audio.loadAudio(osufile.filepath.parent_path() / osufile.audioFileName);
				audio.playAudio();
				audioStarted = true;
			}
		}

		else if (startOffset >= 3000) {
			if (!audioStarted) {
				gameIsRunning = true;
				if (audio.checkMusicIsActive()) {
					audio.stopAudio();
				}
				audio.loadAudio(osufile.filepath.parent_path() / osufile.audioFileName);
				audio.playAudio();
				audio.setPos(static_cast<double>(startOffset)/1000);
				audioStarted = true;
			}
		}

		checkCombo();
		std::vector<bool> removeIndices;
		for(auto& hce : hitCirclesEffect) {
			hce.update(dt);
			if(hce.active == false) {
				removeIndices.emplace_back(true);
			}
		}
		if (removeIndices.size() == hitCirclesEffect.size() && hitCirclesEffect.size() > 0) {
			hitCirclesEffect.clear();
		}

		// Screen Log
		std::ostringstream oss1;
		oss1 << "R/B/O " << REDS << "/" << BLUES << "/" << OTHERS;
		text1.setString(oss1.str());
		std::ostringstream oss2;
		oss2 << "x0/x50/x100/x300 " << score.x0 << "/" << score.x50 << "/" << score.x100 << "/" << score.x300;
		text2.setString(oss2.str());
		std::ostringstream oss3;
		oss3 << "x/s " << score.combo << "/" << score.score;
		text3.setString(oss3.str());

		int audioPos = 0;

		switch (audioStarted) {
			case false: audioPos = prevTimer.getElapsedTime().asMilliseconds() - startOffset; break;
			default: audioPos = audio.getMusicPos(); break;
		}

		drawable.clear();
		ofScanning = true;
		// bool first = true;
		int index = offset;

		// Taps
		for (Taps& tap : taps) {
			if (tap.key == KeyssKey::LK && tap.down == TapsDir::Down) keyss.LK = true;
			if (tap.key == KeyssKey::LD && tap.down == TapsDir::Down) keyss.LD = true;
			if (tap.key == KeyssKey::RD && tap.down == TapsDir::Down) keyss.RD = true;
			if (tap.key == KeyssKey::RK && tap.down == TapsDir::Down) keyss.RK = true;
			if (tap.key == KeyssKey::LK && tap.down == TapsDir::Up) keyss.LK = false;
			if (tap.key == KeyssKey::LD && tap.down == TapsDir::Up) keyss.LD = false;
			if (tap.key == KeyssKey::RD && tap.down == TapsDir::Up) keyss.RD = false;
			if (tap.key == KeyssKey::RK && tap.down == TapsDir::Up) keyss.RK = false;
			if (tap.key == KeyssKey::CLEAR) {
				keyss.LK = false;
				keyss.LD = false;
				keyss.RD = false;
				keyss.RK = false;
			}
		}
		// End Taps

		changeClickDown();

		while (ofScanning) {
			if (index + 1 > hitobjects.size()) { ofScanning = false; break; }

			HitObject& hobj = hitobjects.at(index);

			if (hobj.time > audioPos + 1450) { ofScanning = false; break; }

			if (hobj.time < audioPos - 150) { // hobj.time >= audioPos - 100 && // && hobj.time <= audioPos - 100 // first
				// first = false;
				offset = index;
				hobj.next = true;
			}

			float a = static_cast<float>(audioPos);
			float b = static_cast<float>(hobj.time);

			// sf::Vector2f drawCoords{150.f + (b - a) / 1480.f * 1130.f, 200.f};
			
			sf::Vector2f drawCoords{150.f + (b - a) * 1.3f, 212.f};
			float drawSize = (hobj.hitSound & HitSound::Finish) == HitSound::Finish ? 50.f : 40.f;

			// std::cout << hobj << '\n';

			// Taps
			for (Taps& tap : taps) {
				if (hobj.time < audioPos + 70 && hobj.time > audioPos - 70 && !hobj.hitted) {
					if ((hobj.type & Type::HitCircle) == Type::HitCircle) {
						if (((hobj.hitSound & HitSound::Whistle) != HitSound::None
								|| (hobj.hitSound & HitSound::Clap) != HitSound::None)
								&& tap.hs == HitSound::Whistle && (keyss.LK || keyss.RK)) {
							hobj.hitted = true; BLUES++; score.combo++; tap.hitted = true;
							if(checkHit(hobj.time, tap.time)) {
								hitCirclesEffect.emplace_back(
									drawCoords,
									configFile.getKaColour(), drawSize,
									hct, hcot
								);
							};
						} else if (((hobj.hitSound & HitSound::Normal) != HitSound::None
								|| hobj.hitSound == HitSound::None)
								&& tap.hs == HitSound::Normal && (keyss.LD || keyss.RD)) {
							hobj.hitted = true; REDS++; score.combo++; tap.hitted = true;
							if(checkHit(hobj.time, tap.time)) {
								hitCirclesEffect.emplace_back(
									drawCoords,
									configFile.getDonColour(), drawSize,
									hct, hcot
								);
							};
						} else if ((hobj.hitSound == HitSound::Finish
								|| ((hobj.hitSound & HitSound::Finish) == HitSound::Finish
									&& (hobj.hitSound & HitSound::Normal) == HitSound::Normal))
								&& tap.hs == HitSound::Normal && (keyss.LD || keyss.RD)) {
							hobj.hitted = true; REDS++; score.combo++; tap.hitted = true;
							if(checkHit(hobj.time, tap.time)) {
								hitCirclesEffect.emplace_back(
									drawCoords,
									configFile.getDonColour(), drawSize,
									hct, hcot
								);
							};
						}
					} else if ((hobj.type & Type::Slider) == Type::Slider && !hobj.sliderTail
														&& (keyss.LD || keyss.RD || keyss.LK || keyss.RK)) {
						hobj.hitted = true; OTHERS++; score.combo++; tap.hitted = true;
						if(checkHit(hobj.time, tap.time)) {
							hitCirclesEffect.emplace_back(
								drawCoords,
								configFile.getSliderColour(), drawSize,
								hct, hcot
							);
						};
					} else if ((hobj.type & Type::Spinner) == Type::Spinner && !hobj.spinnerTail
														&& (keyss.LD || keyss.RD || keyss.LK || keyss.RK)) {
						hobj.hitted = true; OTHERS++; score.combo++; tap.hitted = true;
						if(checkHit(hobj.time, tap.time)) {
							hitCirclesEffect.emplace_back(
								drawCoords,
								configFile.getSpinnerColour(), 40.f,
								hct, hcot
							);
						};
					}
				}
			}
			// End Taps

			if (!hobj.hitted && hobj.time < audioPos - 150 && !hobj.sliderTail && !hobj.spinnerTail) {
				hobj.hitted = true;
				score.x0++;
			}

			if (!hobj.hitted) {
				if ((hobj.type & Type::HitCircle) == Type::HitCircle) {
					if (((hobj.hitSound & HitSound::Whistle) == HitSound::Whistle)
						|| ((hobj.hitSound & HitSound::Clap) == HitSound::Clap)) {
							drawable.emplace_back(drawCoords, configFile.getKaColour(), drawSize, hct, hcot);
						} else {
							drawable.emplace_back(drawCoords, configFile.getDonColour(), drawSize, hct, hcot);
						}
				} else if ((hobj.type & Type::Slider) == Type::Slider
						&& !hobj.sliderTail) {
					drawable.emplace_back(drawCoords, configFile.getSliderColour(), drawSize, hct, hcot);
				} else if ((hobj.type & Type::Slider) == Type::Slider
						&& !hobj.sliderTail) {
					drawable.emplace_back(drawCoords, configFile.getSliderColour(), drawSize - 5.f, hct, hcot);
				} else if ((hobj.type & Type::Spinner) == Type::Spinner
						&& !hobj.spinnerTail) {
					drawable.emplace_back(drawCoords, configFile.getSpinnerColour(), 40.f, hct, hcot);
				} else if ((hobj.type & Type::Spinner) == Type::Spinner
						&& hobj.spinnerTail) {
					drawable.emplace_back(drawCoords, configFile.getSpinnerColour(), 35.f, hct, hcot);
				} else {
					drawable.emplace_back(drawCoords, sf::Color::White, 30.f, hct, hcot);
				}
			}
			
			index++;
		}
		taps.clear();
	}

	std::string getSelectedFromOsuFile(OsuFile& osufile) {
		OsuFile& osf = osufile;
		std::ostringstream oss;
		oss << osf.artist << " - " << osf.title << "[" << osf.version << "] (" << osf.creator << ")";
		return oss.str();
	}

	std::string getSelected() {
		return getSelectedFromOsuFile(this->osufile);
	}

	void load(OsuFile& osufile) {
		std::cout << "Selected: " << getSelectedFromOsuFile(osufile) << '\n';
		this->osufile = osufile;
		hitobjects = std::move(parseHitObjects(osufile.filepath));
		//
		startOffset = 0;
		int firstObjPos = hitobjects.at(0).time;
		//
		if (firstObjPos < 1200) {
			startOffset = 2000;
		} else if (firstObjPos >= 5000) {
			startOffset = firstObjPos - 2000;
		}
		//
		reset();
		gameIsRunning = true;
		//
		int countOfCircles = 0;
		int countOfSliders = 0;
		int countOfSpinners = 0;
		//
		for (HitObject& hobj : hitobjects) {
			if ((static_cast<unsigned int>(hobj.type) & static_cast<unsigned int>(Type::HitCircle)) != 0) {
				countOfCircles++;
			} else if ((static_cast<unsigned int>(hobj.type) & static_cast<unsigned int>(Type::Slider)) != 0) {
				countOfSliders++;
			} else if ((static_cast<unsigned int>(hobj.type) & static_cast<unsigned int>(Type::Spinner)) != 0) {
				countOfSpinners++;
			}
		}
		//
		std::cout << "C/S/S: " << countOfCircles << "/" << countOfSliders << "/" << countOfSpinners << '\n';
	}
};

class VolumeGraph {
	sf::RectangleShape soundRect;
	sf::RectangleShape musicRect;
	sf::Clock clock;
	bool showing = false;
	Audio& localaudio;
public:
	VolumeGraph(Audio& audio) : localaudio(audio) {
		soundRect.setSize(sf::Vector2f(10, 300));
		musicRect.setSize(sf::Vector2f(10, 300));

		soundRect.setFillColor(sf::Color::Blue);
		musicRect.setFillColor(sf::Color::Cyan);

		soundRect.setPosition({ 1270, 420 });
		musicRect.setPosition({ 1258, 420 });
	}

	void draw(sf::RenderWindow& window) {
		if (showing) {
			window.draw(soundRect);
			window.draw(musicRect);
		}
	}

	void update() {
		if (showing) {
			if (!clock.isRunning()) {
				clock.reset();
				clock.start();
			}

			if (clock.getElapsedTime().asSeconds() > 5) {
				clock.reset();
				clock.stop();
				showing = false;
			}

			float sVol = localaudio.getSoundVolume();
			soundRect.setSize(sf::Vector2f(10, 300 * sVol ? 300 * sVol : 2));
			soundRect.setPosition({ 1270, 720 - 300 * sVol ? 720 - 300 * sVol : 2 });

			float aVol = localaudio.getMusicVolume();
			musicRect.setSize(sf::Vector2f(10, 300 * aVol ? 300 * aVol : 2));
			musicRect.setPosition({ 1258, 720 - 300 * aVol ? 720 - 300 * aVol : 2 });
		}
		else {
			if (clock.isRunning()) {
				clock.reset();
				clock.stop();
			}
		}
	};

	void show() {
		showing = true;
		clock.reset();
	}
};

