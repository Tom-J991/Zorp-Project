#pragma once

#include "Character.h"

#include <vector>

class Powerup;
class Room;
class Enemy;

class Player : public Character
{
public:
	Player();
	Player(int x, int y);
	Player(Point2D position);
	~Player();

	void draw();
	void drawDescription();
	void lookAt();

	void executeCommand(int command, Room* pRoom);

private:
	void pickup(Room *pRoom);
	void attack(Enemy *pEnemy);

};
