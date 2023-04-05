#pragma once

#include "Character.h"

class Enemy : public Character
{
public:
	Enemy();
	~Enemy() {}

	void onAttacked(int attackPoints);

	void draw();
	void drawDescription();
	void lookAt();

};
