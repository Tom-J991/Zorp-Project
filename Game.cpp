#include "Game.h"

#include "Enemy.h"
#include "Powerup.h"
#include "Food.h"

#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include <random>
#include <time.h>

Game::Game()
	: m_gameOver{ false }
    , m_tempPowerups{ nullptr }
{ }
Game::~Game()
{ 
    delete[] m_powerups;
    delete[] m_enemies;
    delete[] m_food;
}

bool Game::startup()
{
    if (enableVirtualTerminal() == false)
    {
        std::cout << "Virtual terminal processing mode could not be activated." << std::endl;
        std::cout << "Press 'Enter' to exit." << std::endl;
        std::cin.get();
        return false;
    }
    srand(time(nullptr));

    initializeMap();
    initializeEnemies();
    initializePowerups();
    initializeFood();

    m_player.setPosition({ 0, 0 });
    drawWelcomeMessage();

	return true;
}

void Game::update()
{
    Point2D playerPosition = m_player.getPosition();

    if (m_map[playerPosition.y][playerPosition.x].getType() == EXIT)
    {
        m_gameOver = true;
        return;
    }

    int command = getCommand();
    if (command == QUIT)
    {
        m_gameOver = true;
        return;
    }

    m_player.executeCommand(command, &m_map[playerPosition.y][playerPosition.x]);

    for (int i = 0; i < m_enemyCount; i++)
    {
        if (m_enemies[i].isAlive() == false)
        {
            Point2D pos = m_enemies[i].getPosition();
            m_map[pos.y][pos.x].removeGameObject(&m_enemies[i]);
        }
    }
}
void Game::draw()
{
    Point2D playerPosition = m_player.getPosition();

    drawValidDirections();

    m_map[playerPosition.y][playerPosition.x].drawDescription();

    drawMap();
    m_player.draw();
}

bool Game::isGameOver()
{
	return m_gameOver;
}

Powerup *Game::findPowerup(const char *name, bool isLoading) const
{
    if (isLoading == false)
    {
        for (int i = 0; i < m_powerupCount; i++)
        {
            if (strcmp(name, m_powerups[i].getName()) == 0)
                return &m_powerups[i];
        }
    }
    else
    {
        for (int i = 0; i < m_tempPowerupCount; i++)
        {
            if (strcmp(name, m_tempPowerups[i].getName()) == 0)
                return &m_tempPowerups[i];
        }
    }
    return nullptr;
}

void Game::save()
{
    std::ofstream out;
    out.open("zorp_savegame.dat", std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);

    if (out.is_open())
    {
        if (m_gameOver == true)
        {
            std::cout << EXTRA_OUTPUT_POS << "You have perished. Saving will not save you." << std::endl;
            std::cout << INDENT << "Your progress has not been saved." << RESET_COLOR << std::endl;
        }
        else
        {
            out.write((char *)&m_powerupCount, sizeof(int));
            for (int i = 0; i < m_powerupCount; i++)
                m_powerups[i].save(out);

            out.write((char *)&m_enemyCount, sizeof(int));
            for (int i = 0; i < m_enemyCount; i++)
                m_enemies[i].save(out);

            out.write((char *)&m_foodCount, sizeof(int));
            for (int i = 0; i < m_foodCount; i++)
                m_food[i].save(out);

            m_player.save(out);
        }
    }
    else
    {
        std::cout << EXTRA_OUTPUT_POS << RED << "A grue has corrupted the scroll of rememberance." << std::endl;
        std::cout << INDENT << "Your progress has not been saved." << RESET_COLOR << std::endl;
    }

    out.flush();
    out.close();
}
bool Game::load()
{
    std::ifstream in;
    in.open("zorp_savegame.dat", std::ifstream::in | std::ifstream::binary);

    if (!in.is_open())
        return false;

    if (m_tempPowerups != nullptr)
        delete[] m_tempPowerups;

    in.read((char *)&m_tempPowerupCount, sizeof(int));
    if (in.rdstate() || m_tempPowerupCount < 0) return false;
    if (m_tempPowerupCount > 0)
    {
        m_tempPowerups = new Powerup[m_tempPowerupCount];
        for (int i = 0; i < m_tempPowerupCount; i++)
        {
            if (m_tempPowerups[i].load(in, this) == false)
            {
                delete[] m_tempPowerups;
                m_tempPowerups = nullptr;
                return false;
            }
        }
    }

    int enemyCount;
    in.read((char *)&enemyCount, sizeof(int));
    if (in.rdstate() || enemyCount < 0) return false;
    Enemy *enemies = new Enemy[enemyCount];
    for (int i = 0; i < enemyCount; i++)
    {
        if (enemies[i].load(in, this) == false)
        {
            delete[] enemies;
            delete[] m_tempPowerups;
            m_tempPowerups = nullptr;
            return false;
        }
    }

    int foodCount;
    in.read((char *)&foodCount, sizeof(int));
    if (in.rdstate() || foodCount < 0) return false;
    Food *foods = new Food[foodCount];
    for (int i = 0; i < foodCount; i++)
    {
        if (foods[i].load(in, this) == false)
        {
            delete[] foods;
            delete[] enemies;
            delete[] m_tempPowerups;
            m_tempPowerups = nullptr;
            return false;
        }
    }

    Player player;
    if (player.load(in, this) == false)
    {
        delete[] foods;
        delete[] enemies;
        delete[] m_tempPowerups;
        m_tempPowerups = nullptr;
        return false;
    }

    for (int y = 0; y < MAZE_HEIGHT; y++)
    {
        for (int x = 0; x < MAZE_WIDTH; x++)
        {
            m_map[y][x].clearGameObjects();
        }
    }

    delete[] m_powerups;
    m_powerups = m_tempPowerups;
    m_powerupCount = m_tempPowerupCount;
    m_tempPowerups = nullptr;

    for (int i = 0; i < m_powerupCount; i++)
    {
        Point2D pos = m_powerups[i].getPosition();
        if (pos.x >= 0 && pos.y >= 0)
            m_map[pos.y][pos.x].addGameObject(&m_powerups[i]);
    }

    delete[] m_enemies;
    m_enemies = enemies;
    m_enemyCount = enemyCount;

    for (int i = 0; i < m_enemyCount; i++)
    {
        Point2D pos = m_enemies[i].getPosition();
        if (m_enemies->isAlive())
            m_map[pos.y][pos.x].addGameObject(&m_enemies[i]);
    }

    delete[] m_food;
    m_food = foods;
    m_foodCount = foodCount;

    for (int i = 0; i < m_foodCount; i++)
    {
        Point2D pos = m_food[i].getPosition();
        if (pos.x >= 0 && pos.y >= 0)
            m_map[pos.y][pos.x].addGameObject(&m_food[i]);
    }

    m_player = player;

    return true;
}

bool Game::enableVirtualTerminal()
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
        return false;

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode))
        return false;

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hOut, dwMode))
        return false;

    return true;
}

void Game::initializeMap()
{
    for (int y = 0; y < MAZE_HEIGHT; y++)
        for (int x = 0; x < MAZE_WIDTH; x++)
            m_map[y][x].setPosition({ x, y });

    m_map[0][0].setType(ENTRANCE);
    m_map[MAZE_HEIGHT-1][MAZE_WIDTH-1].setType(EXIT);
}
void Game::initializeEnemies()
{
    m_enemyCount = 1 + rand() % 4;
    m_enemies = new Enemy[m_enemyCount];

    for (int i = 0; i < m_enemyCount; i++)
    {
        int x = 2 + (rand() % (MAZE_WIDTH - 3));
        int y = 2 + (rand() % (MAZE_HEIGHT - 3));

        m_enemies[i].setPosition(Point2D{ x, y });
        m_map[y][x].addGameObject(&m_enemies[i]);
    }
}
void Game::initializePowerups()
{
    m_powerupCount = 3;
    m_powerups = new Powerup[m_powerupCount];

    for (int i = 0; i < m_powerupCount; i++)
    {
        char name[30] = "";
        int x = rand() % (MAZE_WIDTH - 1);
        int y = rand() % (MAZE_HEIGHT - 1);

        float HP = 1;
        float AT = 1;
        float DF = 1;

        switch (i)
        {
        case 0:
            strcpy(name, "potion of ");
            m_powerups[i].setHealthMultiplier(1.1f);
            break;
        case 1:
            strcpy(name, "sword of ");
            m_powerups[i].setAttackMultipler(1.1f);
            break;
        case 2:
            strcpy(name, "shield of ");
            m_powerups[i].setDefenseMultipler(1.1f);
            break;
        }

        strncat(name, itemNames[rand() % 15], 30);
        m_powerups[i].setName(name);
        m_powerups[i].setPosition(Point2D{ x, y });
        m_map[y][x].addGameObject(&m_powerups[i]);
    }
}
void Game::initializeFood()
{
    m_foodCount = 3;
    m_food = new Food[m_foodCount];

    for (int i = 0; i < m_foodCount; i++)
    {
        int x = rand() % (MAZE_WIDTH - 1);
        int y = rand() % (MAZE_HEIGHT - 1);
        m_food[i].setPosition(Point2D{ x, y });
        m_map[y][x].addGameObject(&m_food[i]);
    }
}

void Game::drawWelcomeMessage()
{
    std::cout << TITLE << MAGENTA << "Welcome to ZORP!" << RESET_COLOR << std::endl;
    std::cout << INDENT << "ZORP is a game of adventure, danger, and low cunning." << std::endl;
    std::cout << INDENT << "It is definitely not related to any other text-based adventure game." << std::endl << std::endl;
}
void Game::drawMap()
{
    std::cout << RESET_COLOR;
    for (int y = 0; y < MAZE_HEIGHT; y++)
    {
        std::cout << INDENT;
        for (int x = 0; x < MAZE_WIDTH; x++)
        {
            m_map[y][x].draw();
        }
        std::cout << std::endl;
    }
}
void Game::drawValidDirections()
{
    Point2D position = m_player.getPosition();

    std::cout << RESET_COLOR;
    std::cout << CSI << MOVEMENT_DESC_Y + 1 << ";" << 0 << "H";
    std::cout << INDENT << "You can see paths leading to the " <<
        ((position.x > 0) ? "west, " : "") <<
        ((position.x < MAZE_WIDTH - 1) ? "east, " : "") <<
        ((position.y > 0) ? "north, " : "") <<
        ((position.y < MAZE_HEIGHT - 1) ? "south, " : "") << std::endl;
}

int Game::getCommand()
{
    char input[50] = "\0";

    std::cout << CSI << PLAYER_INPUT_Y << ";" << 0 << "H";
    std::cout << CSI << "5M";
    std::cout << CSI << "5L";

    std::cout << INDENT << "Enter a command.";
    std::cout << CSI << PLAYER_INPUT_Y << ";" << PLAYER_INPUT_X << "H" << YELLOW;

    std::cin.clear();
    std::cin.ignore(std::cin.rdbuf()->in_avail());

    std::cin >> input;
    std::cout << RESET_COLOR;

    bool bMove = false;
    bool bPickup = false;
    while (input)
    {
        if (strcmp(input, "move") == 0)
        {
            bMove = true;
        }
        else if (bMove == true)
        {
            if (strcmp(input, "north") == 0 || strcmp(input, "up") == 0)
                return NORTH;
            if (strcmp(input, "south") == 0 || strcmp(input, "down") == 0)
                return SOUTH;
            if (strcmp(input, "east") == 0 || strcmp(input, "right") == 0)
                return EAST;
            if (strcmp(input, "west") == 0 || strcmp(input, "left") == 0)
                return WEST;
        }

        if (strcmp(input, "look") == 0)
            return LOOK;
        if (strcmp(input, "fight") == 0 || strcmp(input, "attack") == 0)
            return FIGHT;

        if (strcmp(input, "pick") == 0)
        {
            bPickup = true;
        }
        else if (bPickup == true)
        {
            if (strcmp(input, "up") == 0)
                return PICKUP;
        }

        if (strcmp(input, "save") == 0)
        {
            save();
            return SAVE;
        }
        if (strcmp(input, "load") == 0)
        {
            if (!load())
            {
                std::cout << EXTRA_OUTPUT_POS << RED << "A grue has corrupted the scroll of rememberance." << std::endl;
                std::cout << INDENT << "Your progress has not been resurrected." << RESET_COLOR << std::endl;
                return 0;
            }
            return LOAD;
        }

        if (strcmp(input, "exit") == 0 || strcmp(input, "quit") == 0)
            return QUIT;

        char next = std::cin.peek();
        if (next == '\n' || next == EOF)
            break;

        std::cin >> input;
    }

    return 0;
}
