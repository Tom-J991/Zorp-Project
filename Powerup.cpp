#include "Powerup.h"

Powerup::Powerup(const char name[30], float health, float attack, float defense)
	: m_name{ *name }
	, m_healthMultiplier{ health }
	, m_attackMultiplier{ attack }
	, m_defenseMultiplier{ defense }
{ }
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
