#pragma once

#include <sstream>
#include <string>
#include <SFML/Graphics.hpp>
#include "game.hpp"

class ResultScreen {
	sf::Font& font;
	sf::Text xScore;
	sf::Text x300;
	sf::Text x100;
	sf::Text x50;
	sf::Text xMiss;
	sf::Text xMaxCombo;
public:
	ResultScreen(Game& game, sf::Font& font) : font(font), xScore(font),
			x300(font), x100(font), x50(font), xMiss(font), xMaxCombo(font) {

		xScore.setPosition({ 20.f, 78.f });
		x300.setPosition({ 20.f, 140.f });
		x100.setPosition({ 120.f, 140.f });
		x50.setPosition({ 220.f, 140.f });
		xMiss.setPosition({ 320.f, 140.f });
		xMaxCombo.setPosition({ 20.f, 200.f });

		xScore.setCharacterSize(34u);
		x300.setCharacterSize(24u);
		x100.setCharacterSize(24u);
		x50.setCharacterSize(24u);
		xMiss.setCharacterSize(24u);
		xMaxCombo.setCharacterSize(28u);

		xScore.setString("Score " + std::to_string(game.score.score));
		x300.setString("x300 " + std::to_string(game.score.x300));
		x100.setString("x100 " + std::to_string(game.score.x100));
		x50.setString("x50 " + std::to_string(game.score.x50));
		xMiss.setString("xMiss " + std::to_string(game.score.x0));
		xMaxCombo.setString("Max Combo " + std::to_string(game.score.maxCombo));
	}
	void draw(sf::RenderWindow& window) {
		window.draw(xScore);
		window.draw(x300);
		window.draw(x100);
		window.draw(x50);
		window.draw(xMiss);
		window.draw(xMaxCombo);
	}
};