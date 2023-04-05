#include "Character.h"

#include "Game.h"
#include "Powerup.h"

#include <algorithm>

Character::Character()
	: GameObject{ { 0, 0 } }
	, m_healthPoints{ 0 }
	, m_attackPoints{ 0 }
	, m_defensePoints{ 0 }
{ }
Character::Character(Point2D position, int health, int attack, int defense)
	: GameObject{ position }
	, m_healthPoints{ health }
	, m_attackPoints{ attack }
	, m_defensePoints{ defense }
{ }
Character::~Character()
{ }

void Character::addPowerup(Powerup *pPowerup)
{
	m_powerups.push_back(pPowerup);
	std::sort(m_powerups.begin(), m_powerups.end(), Powerup::compare);
}

void Character::save(std::ofstream &out)
{
	if (!out.is_open())
		return;

	out.write((char *)&m_priority, sizeof(int));
	out.write((char *)&m_mapPosition, sizeof(Point2D));
	out.write((char *)&m_healthPoints, sizeof(int));
	out.write((char *)&m_attackPoints, sizeof(int));
	out.write((char *)&m_defensePoints, sizeof(int));

	int count = m_powerups.size();
	out.write((char *)&count, sizeof(int));
	for (int i = 0; i < count; i++)
		out.write(m_powerups[i]->getName(), 30);
}
bool Character::load(std::ifstream &in, const Game *game)
{
	if (!in.is_open())
		return false;

	in.read((char *)&m_priority, sizeof(int));
	if (in.rdstate()) return false;
	in.read((char *)&m_mapPosition, sizeof(Point2D));
	if (in.rdstate()) return false;
	in.read((char *)&m_healthPoints, sizeof(int));
	if (in.rdstate()) return false;
	in.read((char *)&m_attackPoints, sizeof(int));
	if (in.rdstate()) return false;
	in.read((char *)&m_defensePoints, sizeof(int));
	if (in.rdstate()) return false;

	int powerupCount;
	char name[30] = { 0 };
	in.read((char *)&powerupCount, sizeof(int));
	if (in.rdstate()) return false;
	for (int i = 0; i < powerupCount; i++)
	{
		name[0] = 0;
		in.read(name, 30);
		if (in.rdstate()) return false;

		Powerup *up = game->findPowerup(name, true);
		m_powerups.push_back(up);
	}

	return true;
}
