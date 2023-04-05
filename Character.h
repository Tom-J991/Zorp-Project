#pragma once

#include "GameObject.h"

#include <fstream>
#include <string>
#include <vector>

class Powerup;

class Character : public GameObject
{
public:
	Character();
	Character(Point2D position, int health, int attack, int defense);
	~Character();

	void addPowerup(Powerup *pPowerup);

	virtual void draw() = 0;
	virtual void drawDescription() = 0;
	virtual void lookAt() = 0;

	void save(std::ofstream &out);
	bool load(std::ifstream &in, const Game *game);

	int getHP() { return m_healthPoints; }
	int getAT() { return m_attackPoints; }
	int getDF() { return m_defensePoints; }

	bool isAlive() { return (m_healthPoints > 0); }

protected:
	std::vector<Powerup *> m_powerups;

	int m_healthPoints;
	int m_attackPoints;
	int m_defensePoints;

};
