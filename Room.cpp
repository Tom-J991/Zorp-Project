#include "Room.h"

#include "GameDefines.h"
#include "GameObject.h"
#include "Enemy.h"
#include "Powerup.h"
#include "Food.h"

#include <iostream>
#include <algorithm>

Room::Room()
	: m_type{ EMPTY }
	, m_mapPosition{ 0, 0 }
{ }
Room::~Room()
{ }

void Room::setPosition(Point2D position)
{
	m_mapPosition = position;
}
void Room::setType(int type)
{
	m_type = type;
}

void Room::addGameObject(GameObject *object)
{
    m_objects.push_back(object);
    std::sort(m_objects.begin(), m_objects.end(), GameObject::compare);
}
void Room::removeGameObject(GameObject *object)
{
    for (auto it = m_objects.begin(); it != m_objects.end(); it++)
    {
        if (*it == object)
        {
            (*it)->setPosition(Point2D{ -1, -1 });
            m_objects.erase(it);
            return;
        }
    }
}
void Room::clearGameObjects()
{
    m_objects.clear();
}

int Room::getType()
{
	return m_type;
}

Enemy *Room::getEnemy()
{
    for (GameObject *pObj : m_objects)
    {
        Enemy *enemy = dynamic_cast<Enemy *>(pObj);
        if (enemy != nullptr)
            return enemy;
    }
    return nullptr;
}
Powerup *Room::getPowerup()
{
    for (GameObject *pObj : m_objects)
    {
        Powerup *powerup = dynamic_cast<Powerup *>(pObj);
        if (powerup != nullptr)
            return powerup;
    }
    return nullptr;
}
Food *Room::getFood()
{
    for (GameObject *pObj : m_objects)
    {
        Food *food = dynamic_cast<Food *>(pObj);
        if (food != nullptr)
            return food;
    }
    return nullptr;
}

void Room::draw()
{
    int outX = INDENT_X + (6 * m_mapPosition.x) + 1;
    int outY = MAP_Y + m_mapPosition.y;

    std::cout << CSI << outY << ";" << outX << "H";
    switch (m_type)
    {
    case EMPTY:
        if (m_objects.size() > 0)
            m_objects[0]->draw();
        else
            std::cout << "[ " << GREEN << "\xb0" << RESET_COLOR << " ] ";
        break;
    case ENTRANCE:
        std::cout << "[ " << WHITE << "\x9d" << RESET_COLOR << " ] ";
        break;
    case EXIT:
        std::cout << "[ " << WHITE << "\xFE" << RESET_COLOR << " ] ";
        break;
    }
}
void Room::drawDescription()
{
    std::cout << RESET_COLOR;
    std::cout << CSI << ROOM_DESC_Y << ";" << 0 << "H";
    std::cout << CSI << "4M" << CSI << "4L" << std::endl;

    switch (m_type)
    {
    case EMPTY:
        if (m_objects.size() > 0)
            m_objects[0]->drawDescription();
        else
            std::cout << INDENT << "You are in an empty meadow, There is nothing of note here." << std::endl;
        break;
    case ENTRANCE:
        std::cout << INDENT << "The entrance you used to enter this maze is blocked. There is no going back." << std::endl;
        break;
    case EXIT:
        std::cout << INDENT << "Despite all odds, you made it to the exit. Congratulations." << std::endl;
        break;
    }
}
void Room::lookAt()
{
    if (m_objects.size() > 0)
        m_objects[0]->lookAt();
    else
        std::cout << EXTRA_OUTPUT_POS << RESET_COLOR << "You look around, but see nothing worth mentioning." << std::endl;
}
