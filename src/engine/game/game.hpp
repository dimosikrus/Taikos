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
	sf::RectangleShape rect;
public:
	Game(Audio& audio, OsuFile& osufile): audio(audio), osufile(osufile), rect(sf::Vector2f(4.f,120.f)) {
		rect.setFillColor(sf::Color::Cyan);
		rect.setPosition({ 148.f, 140.f });
	};
	void show(sf::RenderWindow& window) {
		window.draw(rect);
		for (DrawableObj obj : drawable) {
			obj.draw(window);
		}
	}
	void update() {
		int audioPos = audio.getPos();
		drawable.clear();
		for (HitObject& hobj : hitobjects) {
			if (hobj.time >= audioPos - 200 && hobj.time <= audioPos + 1280) {
				float a = static_cast<float>(audioPos);
				float b = static_cast<float>(hobj.time);
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
					for (int i = 0; i < hobj.length; i++) {
						drawable.emplace_front(sf::Vector2f(150.f + (b - a) + i, 200.f), sf::Color::Yellow, (hobj.hitSound & HitSound::Finish) == HitSound::Finish ? 50.f : 40.f);
					}
				}
				else if ((hobj.type & Type::Spinner) != Type::None) {
					drawable.emplace_front(sf::Vector2f(150.f + (b - a), 200.f), sf::Color::Green, 40.f);
				}
			}
		}
	}
	void load(OsuFile& osufile) {
		std::cout << "Selected: " << osufile.version << '\n';
		this->osufile = osufile;
		hitobjects = std::move(parseHitObjects(osufile.filepath));
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
