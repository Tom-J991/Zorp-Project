#pragma once

#include "GameObject.h"

class Food : public GameObject
{
public:
	Food();
	Food(Point2D position);
	~Food() {};

	int getHP() { return m_healthPoints; }

	void draw();
	void drawDescription();
	void lookAt();

	void save(std::ofstream &out);
	bool load(std::ifstream &in, const Game *game);

private:
	int m_healthPoints;

};
