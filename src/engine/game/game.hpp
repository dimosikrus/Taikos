#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>
#include <deque>
#include <memory>
#include "../audio/audio.hpp"
#include "../gamestates.hpp"
#include "osuTypes.hpp"
#include "resultScreen.hpp"

class DrawableObj {
	float radius;
	float border = 5.f;
	sf::CircleShape circlefill;
	sf::CircleShape circleborder;
	sf::Vector2f pos;
public:
	DrawableObj (sf::Vector2f position, sf::Color color, float radius) : pos(position), circlefill(radius), circleborder(radius), radius(radius) {
		circlefill.setFillColor(color);
		circleborder.setFillColor(sf::Color::Transparent);
		circleborder.setOutlineColor(sf::Color::White);
		circleborder.setOutlineThickness(border);
		circlefill.setPosition(sf::Vector2f(pos.x, pos.y - radius / 2.f));
		circleborder.setPosition(sf::Vector2f(pos.x, pos.y - radius / 2.f));
	}

	void draw(sf::RenderWindow& window) {
		window.draw(circlefill);
		window.draw(circleborder);
	}
};

class Game {
	OsuFile& osufile;
	GameState& localState;
	int offset = 0;
	std::deque<DrawableObj> drawable;
	std::vector<HitObject> hitobjects;
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
	sf::RectangleShape x0Rect;
	sf::RectangleShape x50Rect;
	sf::RectangleShape x100Rect;
	sf::RectangleShape x300Rect;
	ResultScreen& localResults;
	bool ofScanning = true;
	std::vector<Taps> taps;

	void checkHit(int HitObjectTime, int TapTime) {
		if (TapTime > HitObjectTime - 20 && TapTime < HitObjectTime + 20) {
			score.x300++;
			score.score += score.combo != 0 ? 300 * score.combo : 300;
			return;
		}

		if (TapTime > HitObjectTime - 40 && TapTime < HitObjectTime + 40) {
			score.x100++;
			score.score += score.combo != 0 ? 100 * score.combo : 100;
			return;
		}

		if (TapTime > HitObjectTime - 55 && TapTime < HitObjectTime + 55) {
			score.x50++;
			score.score += score.combo != 0 ? 50 * score.combo : 50;
			return;
		}

		score.x0++;
		return;
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
public:
	int REDS = 0,
		BLUES = 0,
		OTHERS = 0;
	int comboMissBuffer = 0;
	Score score;

	Game(Audio& audio, OsuFile& osufile, sf::Font& font, ResultScreen& results, GameState& state) : audio(audio), osufile(osufile), font(font), text1(font), text2(font), text3(font),
			centerRect({ 4.f, 120.f }), x0Rect({ 140.f, 40.f }), x50Rect({ 110.f, 40.f }), x100Rect({ 80.f, 40.f }), x300Rect({ 40.f, 40.f }), localResults(results), localState(state) {
		centerRect.setFillColor(sf::Color::Magenta);
		x0Rect.setFillColor(sf::Color::Red);
		x50Rect.setFillColor(sf::Color::Yellow);
		x100Rect.setFillColor(sf::Color::Green);
		x300Rect.setFillColor(sf::Color::Cyan);
		centerRect.setPosition({ 148.f, 140.f });
		x0Rect.setPosition({ 80.f, 180.f }); // 150-(140/2) = 150-70 = 80
		x50Rect.setPosition({ 95.f, 180.f }); // 150-(110/2) = 150-55 = 95
		x100Rect.setPosition({ 110.f, 180.f }); // 150-(80/2) = 150-40 = 110
		x300Rect.setPosition({ 130.f, 180.f }); // 150-(40/2) = 150-20 = 130
		text1.setPosition({ 15.f, 40.f });
		text2.setPosition({ 15.f, 65.f });
		text3.setPosition({ 400.f, 15.f });
	};

	void pushTap(Taps tapz) {
		taps.emplace_back(tapz);
	}

	void show(sf::RenderWindow& window) {
		window.draw(x0Rect);
		window.draw(x50Rect);
		window.draw(x100Rect);
		window.draw(x300Rect);
		window.draw(centerRect);
		for (DrawableObj obj : drawable) {
			obj.draw(window);
		}
		window.draw(text1);
		window.draw(text2);
		window.draw(text3);
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

	void update() {
		if (audio.getPos() + 200 > hitobjects.at(hitobjects.size() - 1).time) {
			gameIsRunning = false;
			std::cout << "Map is End." << "\n";
			localState = GameState::GameResults;
			setResults();
		}

		if (startOffset <= 3000) {
			if (!atimerStarted) {
				if (audio.checkAudioIsActive()) {
					audio.stopAudio();
				}
				gameIsRunning = true;
				prevTimer.start();
				atimerStarted = true;
			}

			if (prevTimer.getElapsedTime().asMilliseconds() >= startOffset && !audioStarted) {
				prevTimer.stop();
				prevTimer.reset();
				audio.playAudio(osufile.filepath.parent_path() / osufile.audioFileName);
				audioStarted = true;
			}
		}

		else if (startOffset >= 3000) {
			if (!audioStarted) {
				gameIsRunning = true;
				if (audio.checkAudioIsActive()) {
					audio.stopAudio();
				}
				audio.playAudio(osufile.filepath.parent_path() / osufile.audioFileName);
				audio.setPos(static_cast<double>(startOffset)/1000);
				audioStarted = true;
			}
		}

		checkCombo();

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
		if (!audioStarted) {
			audioPos = prevTimer.getElapsedTime().asMilliseconds() - startOffset;
		} else {
			audioPos = audio.getPos();
		}

		drawable.clear();
		ofScanning = true;
		bool first = true;
		int index = offset;

		while (ofScanning) { // start with offset ; scanning in range ; break on greater than range break ; offset > scan range > break;
			if (index + 1 > hitobjects.size()) { ofScanning = false; break; }

			HitObject& hobj = hitobjects.at(index);

			if (hobj.time > audioPos + 1280) { ofScanning = false; break; }

			if (first && hobj.time >= audioPos - 200 && hobj.time <= audioPos + 1280) {
				first = false;
				offset = index;
				hobj.next = true;
			}

			// Taps
			for (Taps& tap : taps) { // all taps at frame
				if (hobj.time < audioPos + 70 && hobj.time > audioPos - 70 && !hobj.hitted && !tap.hitted) {
					int result = hobj.time - tap.time;
					if ((hobj.type & Type::HitCircle) != Type::None) {
						if (((hobj.hitSound & HitSound::Whistle) != HitSound::None ||
								(hobj.hitSound & HitSound::Clap) != HitSound::None) &&
								(tap.hs & HitSound::Whistle) != HitSound::None) {
							hobj.hitted = true;
							BLUES++;
							score.combo++;
							tap.hitted = true;
							checkHit(hobj.time, tap.time);
						}

						if (((hobj.hitSound & HitSound::Normal) != HitSound::None ||
								hobj.hitSound == HitSound::None) &&
								(tap.hs & HitSound::Normal) != HitSound::None) {
							hobj.hitted = true;
							REDS++;
							score.combo++;
							tap.hitted = true;
							checkHit(hobj.time, tap.time);
						}
					} else {
						hobj.hitted = true;
						OTHERS++;
						score.combo++;
						tap.hitted = true;
						checkHit(hobj.time, tap.time);
					}
				}
			}
			// End Taps

			if (!hobj.hitted && hobj.time < audioPos - 150) {
				hobj.hitted = true;
				score.x0++;
			}

			float a = static_cast<float>(audioPos);
			float b = static_cast<float>(hobj.time);
			if (!hobj.hitted) {
				if ((hobj.type & Type::HitCircle) != Type::None) {
					if ((hobj.hitSound & HitSound::Whistle) != HitSound::None ||
						(hobj.hitSound & HitSound::Clap) != HitSound::None) {
						drawable.emplace_front(sf::Vector2f(150.f + (b - a), 200.f), sf::Color::Blue, (hobj.hitSound & HitSound::Finish) == HitSound::Finish ? 50.f : 40.f);
					} else {
						drawable.emplace_front(sf::Vector2f(150.f + (b - a), 200.f), sf::Color::Red, (hobj.hitSound & HitSound::Finish) == HitSound::Finish ? 50.f : 40.f);
					}
				} else if ((hobj.type & Type::Slider) != Type::None) {
					drawable.emplace_front(sf::Vector2f(150.f + (b - a), 200.f), sf::Color::Yellow, (hobj.hitSound & HitSound::Finish) == HitSound::Finish ? 50.f : 40.f);
				} else if ((hobj.type & Type::Spinner) != Type::None) {
					drawable.emplace_front(sf::Vector2f(150.f + (b - a), 200.f), sf::Color::Green, 40.f);
				}
			}
			//
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
