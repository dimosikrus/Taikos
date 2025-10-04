#pragma once

#include <SFML/Graphics.hpp>
#include <sstream>
#include "game.hpp"

class ResultScreen {
	sf::Font& font;
	sf::Text text;
public:
	ResultScreen(Game& game, sf::Font& font) : font(font), text(text) {
		text.setPosition({15.f, 15.f});
		std::ostringstream oss;
		oss << game.score;
		text.setString(oss.str());
	}
	void draw(sf::RenderWindow& window) {
		window.draw(text);
	}
};