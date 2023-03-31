#include "Game.h"

#include <iostream>
#include <windows.h>
#include <random>
#include <time.h>

Game::Game()
	: m_gameOver{ false }
{ }
Game::~Game()
{ }

bool Game::startup()
{
    if (enableVirtualTerminal() == false)
    {
        std::cout << "Virtual terminal processing mode could not be activated." << std::endl;
        std::cout << "Press 'Enter' to exit." << std::endl;
        std::cin.get();
        return false;
    }

    initializeMap();
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
    if (m_player.executeCommand(command, m_map[playerPosition.y][playerPosition.x].getType()))
        return;
    m_map[playerPosition.y][playerPosition.x].executeCommand(command);
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
    srand(time(nullptr));
    for (int y = 0; y < MAZE_HEIGHT; y++)
    {
        for (int x = 0; x < MAZE_WIDTH; x++)
        {
            int type = rand() % (MAX_RANDOM_TYPE * 2);
            if (type < MAX_RANDOM_TYPE)
            {
                if (type == TREASURE)
                    type = rand() % 3 + TREASURE_HP;
                m_map[y][x].setType(type);
            }
            else
                m_map[y][x].setType(EMPTY);
            m_map[y][x].setPosition({ x, y });
        }
    }

    m_map[0][0].setType(ENTRANCE);
    m_map[MAZE_HEIGHT-1][MAZE_WIDTH-1].setType(EXIT);
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
    std::cout << CSI << "4M";
    std::cout << CSI << "4L";

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
            if (strcmp(input, "north") == 0)
                return NORTH;
            if (strcmp(input, "south") == 0)
                return SOUTH;
            if (strcmp(input, "east") == 0)
                return EAST;
            if (strcmp(input, "west") == 0)
                return WEST;
        }

        if (strcmp(input, "look") == 0)
            return LOOK;
        if (strcmp(input, "fight") == 0)
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

        char next = std::cin.peek();
        if (next == '\n' || next == EOF)
            break;

        std::cin >> input;
    }

    return 0;
}
