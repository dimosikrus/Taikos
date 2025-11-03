#pragma once

#include <sstream>
#include <string>
#include <SFML/Graphics.hpp>
#include "game.hpp"

struct ResultRows {
	int x300;
	int x100;
	int x50;
	int x0;
	int xMaxCombo;
	int xScore;
};

class ResultScreen {
	sf::Font& font;
	sf::Text xScore;
	sf::Text x300;
	sf::Text x100;
	sf::Text x50;
	sf::Text xMiss;
	sf::Text xMaxCombo;
public:
	ResultScreen(sf::Font& font) : font(font), xScore(font, "Score 0"),
			x300(font, "x300 0"), x100(font, "x100 0"), x50(font, "x50 0"),
			xMiss(font, "xMiss 0"), xMaxCombo(font, "MaxCombo 0") {

		xScore.setPosition({ 20.f, 78.f }); 
		x300.setPosition({ 20.f, 140.f }); /* adasd asddad */
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
	}

	void updateRows(ResultRows resultRows) {
		/*ResultRows rr{};
		rr = std::move(resultRows);*/
		std::string scoreTEXT =		"Score " +		std::to_string(resultRows.xScore);
		std::string x300TEXT =		"x300 " +		std::to_string(resultRows.x300);
		std::string x100TEXT =		"x100 " +		std::to_string(resultRows.x100);
		std::string x50TEXT =		"x50 " +		std::to_string(resultRows.x50);
		std::string xMissTEXT =		"xMiss " +		std::to_string(resultRows.x0);
		std::string MaxComboTEXT =	"MaxCombo " +	std::to_string(resultRows.xMaxCombo);
		xScore.setString(scoreTEXT);
		x300.setString(x300TEXT);
		x100.setString(x100TEXT);
		x50.setString(x50TEXT);
		xMiss.setString(xMissTEXT);
		xMaxCombo.setString(MaxComboTEXT);
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
