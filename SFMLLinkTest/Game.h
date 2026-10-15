#pragma once
#include<iostream>
#include<ctime>
#include<vector>
#include<sstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "Player.h"
#include "SwagBall.h"

enum class GameState
{
    Running,
    Paused,
    Ended
};

class Game
{
private:
    sf::VideoMode videoMode;
    sf::RenderWindow* window;
    bool endGame;
    sf::Event sfmlEvent;

    Player player;

    int points;

    sf::Font font;
    sf::Text guiText;
    sf::Text endGameText;

    std::vector<SwagBall> swagBalls;
    float spawnTimerMax;
    float spawnTimer;
    size_t maxSwagBalls;

    GameState gameState = GameState::Running;

    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;

    sf::Music backgroundMusic;

    void initVariables();
    void initWindow();
    void initFonts();
    void initText();

public:
    Game();
    ~Game();

    const bool& getEndGame() const;
    const bool running() const;
    void pollEvents();
    void spawnSwagBalls();
    const int randBallType() const;
    void updatePlayer();
    void updateCollision();
    void updateGui();
    void update();
    void renderGui(sf::RenderTarget* target);
    void render();
    void restart();
    void resume();
    void quit();
    GameState getGameState() const;
};

