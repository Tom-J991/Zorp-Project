#pragma once

#include "GameObject.h"

class Powerup : public GameObject
{
public:
	Powerup();
	Powerup(const char name[30], float health, float attack, float defense);
	~Powerup();

	char *getName();
	float getHealthMultipler();
	float getAttackMultipler();
	float getDefenseMultipler();

	void setName(const char* name);
	void setHealthMultiplier(float health) { m_healthMultiplier = health; }
	void setAttackMultipler(float attack) { m_attackMultiplier = attack; }
	void setDefenseMultipler(float defense) { m_defenseMultiplier = defense; }

	static bool compare(const Powerup *p1, const Powerup *p2);

	void draw();
	void drawDescription();
	void lookAt();

	void save(std::ofstream &out);
	bool load(std::ifstream &in, const Game *game);

private:
	char m_name[30];

	float m_healthMultiplier;
	float m_attackMultiplier;
	float m_defenseMultiplier;

};
