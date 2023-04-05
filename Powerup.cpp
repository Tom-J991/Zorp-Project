#include "Powerup.h"

#include "GameDefines.h"

#include <cstring>
#include <iostream>

Powerup::Powerup()
	: m_healthMultiplier{ 1 }
	, m_attackMultiplier{ 1 }
	, m_defenseMultiplier { 1 }
{ 
	m_priority = PRIORITY_POWERUP;
	m_name[0] = 0;
}
Powerup::Powerup(const char name[30], float health, float attack, float defense)
	: m_healthMultiplier{ health }
	, m_attackMultiplier{ attack }
	, m_defenseMultiplier{ defense }
{ 
	m_priority = PRIORITY_POWERUP;
	strcpy(m_name, name);
}
Powerup::~Powerup()
{ }

char *Powerup::getName()
{
	return m_name;
}
float Powerup::getHealthMultipler()
{
	return m_healthMultiplier;
}
float Powerup::getAttackMultipler()
{
	return m_attackMultiplier;
}
float Powerup::getDefenseMultipler()
{
	return m_defenseMultiplier;
}

void Powerup::setName(const char *name)
{
	strncpy(m_name, name, 30);
}

bool Powerup::compare(const Powerup *p1, const Powerup *p2)
{
	return (strcmp(p1->m_name, p2->m_name) < 0) ? true : false;
}

void Powerup::draw()
{
	std::cout << "[ " << YELLOW << "$" << RESET_COLOR << " ] ";
}
void Powerup::drawDescription()
{
	std::cout << INDENT << "There appears to be some treasure here. Perhaps you should investigate further." << std::endl;
}
void Powerup::lookAt()
{
	std::cout << EXTRA_OUTPUT_POS << RESET_COLOR << "There is some treasure here. It looks small enough to pick up." << std::endl;
}

void Powerup::save(std::ofstream &out)
{
	if (!out.is_open())
		return;

	out.write((char *)&m_priority, sizeof(int));
	out.write((char *)&m_mapPosition, sizeof(Point2D));
	out.write(m_name, 30);
	out.write((char *)&m_healthMultiplier, sizeof(float));
	out.write((char *)&m_attackMultiplier, sizeof(float));
	out.write((char *)&m_defenseMultiplier, sizeof(float));
}
bool Powerup::load(std::ifstream &in, const Game *game)
{
	if (!in.is_open())
		return false;

	in.read((char *)&m_priority, sizeof(int));
	if (in.rdstate()) return false;
	in.read((char *)&m_mapPosition, sizeof(Point2D));
	if (in.rdstate()) return false;
	in.read(m_name, 30);
	if (in.rdstate()) return false;
	in.read((char *)&m_healthMultiplier, sizeof(float));
	if (in.rdstate()) return false;
	in.read((char *)&m_attackMultiplier, sizeof(float));
	if (in.rdstate()) return false;
	in.read((char *)&m_defenseMultiplier, sizeof(float));
	if (in.rdstate()) return false;

	return true;
}
