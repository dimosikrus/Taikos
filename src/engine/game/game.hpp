#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>
//#include <unordered_map>
//#include <vector>
//#include <functional>
#include <deque>
#include <memory>
#include "../audio/audio.hpp"
#include "osuTypes.hpp"

//#include <type_traits>
//template<typename T>
//	requires std::is_enum_v<T>
//T operator|(T lhs, T rhs) {
//	using Underlying = std::underlying_type_t<T>;
//	return static_cast<T>(static_cast<Underlying>(lhs) | static_cast<Underlying>(rhs));
//}
//template<typename T>
//	requires std::is_enum_v<T>
//T operator&(T lhs, T rhs) {
//	using Underlying = std::underlying_type_t<T>;
//	return static_cast<T>(static_cast<Underlying>(lhs) & static_cast<Underlying>(rhs));
//}

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
	int offset = 0;
	std::deque<DrawableObj> drawable;
	std::vector<HitObject> hitobjects;
	Audio& audio;
	sf::Font& font;
	sf::Text text1;
	sf::Text text2;
	sf::Text text3;
	sf::RectangleShape centerRect;
	sf::RectangleShape x0Rect;
	sf::RectangleShape x50Rect;
	sf::RectangleShape x100Rect;
	sf::RectangleShape x300Rect;
	bool ofScanning = true;
	std::vector<Taps> taps;
	int REDS = 0,
		BLUES = 0,
		OTHERS = 0;
	int x0 = 0,
		x50 = 0,
		x100 = 0,
		x300 = 0;
	int combo = 0,
		comboMissBuffer = 0,
		score = 0;
	void checkHit(int HitObjectTime, int TapTime) {
		if (TapTime > HitObjectTime - 20 && TapTime < HitObjectTime + 20) {
			x300++;
			score += combo != 0 ? 300 * combo : 300;
			return;
		}
		if (TapTime > HitObjectTime - 40 && TapTime < HitObjectTime + 40) {
			x100++;
			score += combo != 0 ? 100 * combo : 100;
			return;
		}
		if (TapTime > HitObjectTime - 55 && TapTime < HitObjectTime + 55) {
			x50++;
			score += combo != 0 ? 50 * combo : 50;
			return;
		}
		x0++;
		return;
	}
	void checkCombo() {
		if (comboMissBuffer < x0) {
			combo = 0;
			comboMissBuffer = x0;
		}
	}
public:
	Game(Audio& audio, OsuFile& osufile, sf::Font& font) : audio(audio), osufile(osufile), font(font), text1(font), text2(font), text3(font),
			centerRect({ 4.f, 120.f }), x0Rect({ 140.f, 40.f }), x50Rect({ 110.f, 40.f }), x100Rect({ 80.f, 40.f }), x300Rect({ 40.f, 40.f }) {
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
	void update() {
		checkCombo();
		// Screen Log
		std::ostringstream oss1;
		oss1 << "R/B/O " << REDS << "/" << BLUES << "/" << OTHERS;
		text1.setString(oss1.str());
		std::ostringstream oss2;
		oss2 << "x0/x50/x100/x300 " << x0 << "/" << x50 << "/" << x100 << "/" << x300;
		text2.setString(oss2.str());
		std::ostringstream oss3;
		oss3 << "x/s " << combo << "/" << score;
		text3.setString(oss3.str());

		int audioPos = audio.getPos();
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
							combo++;
							tap.hitted = true;
							checkHit(hobj.time, tap.time);
						}
						if (((hobj.hitSound & HitSound::Normal) != HitSound::None ||
								hobj.hitSound == HitSound::None) &&
								(tap.hs & HitSound::Normal) != HitSound::None) {
							hobj.hitted = true;
							REDS++;
							combo++;
							tap.hitted = true;
							checkHit(hobj.time, tap.time);
						}
					}
					else {
						hobj.hitted = true;
						OTHERS++;
						combo++;
						tap.hitted = true;
						checkHit(hobj.time, tap.time);
					}
				}
			}
			// End Taps

			if (!hobj.hitted && hobj.time < audioPos - 150) {
				hobj.hitted = true;
				x0++;
			}

			float a = static_cast<float>(audioPos);
			float b = static_cast<float>(hobj.time);
			if (!hobj.hitted) {
				if ((hobj.type & Type::HitCircle) != Type::None) {
					if ((hobj.hitSound & HitSound::Whistle) != HitSound::None ||
						(hobj.hitSound & HitSound::Clap) != HitSound::None) {
						drawable.emplace_front(sf::Vector2f(150.f + (b - a), 200.f), sf::Color::Blue, (hobj.hitSound & HitSound::Finish) == HitSound::Finish ? 50.f : 40.f);
					}
					else {
						drawable.emplace_front(sf::Vector2f(150.f + (b - a), 200.f), sf::Color::Red, (hobj.hitSound & HitSound::Finish) == HitSound::Finish ? 50.f : 40.f);
					}
				}
				else if ((hobj.type & Type::Slider) != Type::None) {
					drawable.emplace_front(sf::Vector2f(150.f + (b - a), 200.f), sf::Color::Yellow, (hobj.hitSound & HitSound::Finish) == HitSound::Finish ? 50.f : 40.f);
				}
				else if ((hobj.type & Type::Spinner) != Type::None) {
					drawable.emplace_front(sf::Vector2f(150.f + (b - a), 200.f), sf::Color::Green, 40.f);
				}
			}
			//
			index++;
		}
		taps.clear();
	}
	void load(OsuFile& osufile) {
		std::cout << "Selected: " << osufile.version << '\n';
		this->osufile = osufile;
		hitobjects = std::move(parseHitObjects(osufile.filepath));
		offset = 0;
		ofScanning = true;
		REDS = 0;
		BLUES = 0;
		OTHERS = 0;
		x0 = 0;
		x50 = 0;
		x100 = 0;
		x300 = 0;
		combo = 0;
		comboMissBuffer = 0;
		score = 0;
		//
		int countOfCircles = 0;
		int countOfSliders = 0;
		int countOfSpinners = 0;
		for (HitObject& hobj : hitobjects) {
			if ((static_cast<unsigned int>(hobj.type) & static_cast<unsigned int>(Type::HitCircle)) != 0) {
				countOfCircles++;
			}
			else if ((static_cast<unsigned int>(hobj.type) & static_cast<unsigned int>(Type::Slider)) != 0) {
				countOfSliders++;
			}
			else if ((static_cast<unsigned int>(hobj.type) & static_cast<unsigned int>(Type::Spinner)) != 0) {
				countOfSpinners++;
			}
		}
		std::cout << "Count Of Circles: " << countOfCircles << '\n';
		std::cout << "Count Of Sliders: " << countOfSliders << '\n';
		std::cout << "Count Of Spinners: " << countOfSpinners << '\n';
	}
};
