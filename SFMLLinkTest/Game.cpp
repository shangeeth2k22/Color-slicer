#include "Game.h"

void Game::initVariables()
{
	this->endGame = false;
	this->spawnTimerMax = 10.f;
	this->spawnTimer = this->spawnTimerMax;
	this->maxSwagBalls = 10;
	this->points = 0;
}

void Game::initWindow()
{
	this->videoMode = sf::VideoMode(800, 600);
	this->window = new sf::RenderWindow(this->videoMode, "Game 2", sf::Style::Close | sf::Style::Titlebar);
	this->window->setFramerateLimit(60);
}

void Game::initFonts()
{
	if (!this->font.loadFromFile("Fonts/PANICKO.ttf"))
	{
		std::cout << "! ERROR::GAME::INITFONTS::COULD NOT LOAD PANICKO.ttf" << "\n";
	}
}

void Game::initText()
{
	//Gui text init
	this->guiText.setFont(this->font);
	this->guiText.setFillColor(sf::Color::White);
	this->guiText.setCharacterSize(32);
	

	//End Game Text
	this->endGameText.setFont(this->font);
	this->endGameText.setFillColor(sf::Color::Red);
	this->endGameText.setCharacterSize(60);
	this->endGameText.setPosition(sf::Vector2f(20, 300));
	this->endGameText.setString("       YOU ARE DEAD,EXIT THE GAME!");
}

//constructors and destructors

Game::Game()
{
    this->initVariables();
    this->initWindow();
    this->initFonts();
    this->initText();

    // Load background image
    if (!this->backgroundTexture.loadFromFile("Images/background.png"))
    {
        std::cout << "ERROR::GAME::COULD NOT LOAD Images/background.png" << std::endl;
    }
    else
    {
        this->backgroundSprite.setTexture(this->backgroundTexture);
        this->backgroundSprite.setScale(
            float(this->window->getSize().x) / this->backgroundTexture.getSize().x,
            float(this->window->getSize().y) / this->backgroundTexture.getSize().y
        );
    }

    // Load and play background music
    if (this->backgroundMusic.openFromFile("sound/background.wav"))
    {
        this->backgroundMusic.setLoop(true);
        this->backgroundMusic.play();
    }
    else
    {
        std::cout << "ERROR::GAME::COULD NOT LOAD background.wav" << std::endl;
    }
}

Game::~Game()
{
	delete this->window;
}

const bool& Game::getEndGame() const
{
	return this->endGame;
}

//Functions
const bool Game::running() const
{
	return this->window->isOpen() /* && this->endGame == false*/;
}

void Game::pollEvents()
{
    while (this->window->pollEvent(this->sfmlEvent))
    {
        switch (this->sfmlEvent.type)
        {
        case sf::Event::Closed:
            this->window->close();
            break;
        case sf::Event::KeyPressed:
            if (this->sfmlEvent.key.code == sf::Keyboard::Escape)
                this->window->close();

            if (this->gameState == GameState::Paused)
            {
                if (this->sfmlEvent.key.code == sf::Keyboard::C)
                    this->resume();

                if (this->sfmlEvent.key.code == sf::Keyboard::R)
                    this->restart();

                if (this->sfmlEvent.key.code == sf::Keyboard::Q)
                    this->quit();
            }
            else
            {
                if (this->sfmlEvent.key.code == sf::Keyboard::P)
                    this->gameState = GameState::Paused;
            }

            if (this->sfmlEvent.key.code == sf::Keyboard::F12)
            {
                sf::Texture screenTex;
                screenTex.create(this->window->getSize().x, this->window->getSize().y);
                screenTex.update(*this->window);
                screenTex.copyToImage().saveToFile("screenshot.png");
            }
            break;
        }
    }
}

void Game::spawnSwagBalls()
{
	//Timer
	if (this->spawnTimer < this->spawnTimerMax)
		this->spawnTimer += 1.f;
	else
	{
		if (this->swagBalls.size() < this->maxSwagBalls)
		{
			this->swagBalls.push_back(SwagBall(*this->window,this->randBallType()));
			this->spawnTimer = 0.f;
		}
		
	}
}

const int Game::randBallType() const
{
	int type = SwagBallTypes::DEFAULT;
	int randValue = rand() % 100 + 1;

	if(randValue > 60 && randValue <= 80)
		type = SwagBallTypes::DAMAGING;
	else if(randValue > 80 && randValue <= 100)
		type = SwagBallTypes::HEALING;

	return type;
}

void Game::updatePlayer()
{
	this->player.update(this->window);

	if (this->player.getHp() <= 0)
		this->endGame = true;
}

void Game::updateCollision()
{
    //Check the collision
    for (size_t i = 0; i < this->swagBalls.size(); i++)
    {
        if (this->player.getShape().getGlobalBounds().intersects(this->swagBalls[i].getShape().getGlobalBounds()))
        {
            switch (this->swagBalls[i].getType())
            {
            case SwagBallTypes::DEFAULT:
                this->points++;
                // Removed beep sound play
                break;

            case SwagBallTypes::DAMAGING:
                this->player.takeDamage(1);
                break;

            case SwagBallTypes::HEALING:
                this->player.gainHealth(1);
                break;
            }

            //Remove Ball
            this->swagBalls.erase(this->swagBalls.begin() + i);
        }
    }
}
	
void Game::updateGui()
{
	std::stringstream ss;
	ss << " - points: "<< this->points<<"\n"
		<<"- Health:"<<this->player.getHp()<<" / "<<this->player.getHpMax()<<"\n";
	this->guiText.setString(ss.str());
}

void Game::update()
{
    this->pollEvents();
    if (this->gameState == GameState::Running && this->endGame == false)
    {
        this->spawnSwagBalls();
        this->updatePlayer();
        this->updateCollision();
        this->updateGui();
    }
}

void Game::renderGui(sf::RenderTarget* target)
{
	target->draw(this->guiText);
}

void Game::render()
{
    this->window->clear(); // Default: black background

    // Draw background image
    this->window->draw(this->backgroundSprite);

    //Render stuff
    this->player.render(this->window);

    for (auto i : this->swagBalls)
    {
        i.render(*this->window);
    }

    //Render Gui
    this->renderGui(this->window);

    //Render end text
    if (this->endGame == true)
        this->window->draw(this->endGameText);

    // Render pause menu
    if (this->gameState == GameState::Paused)
    {
        sf::RectangleShape menuBg(sf::Vector2f(400, 300));
        menuBg.setFillColor(sf::Color(0, 0, 0, 200));
        menuBg.setPosition(200, 150);
        this->window->draw(menuBg);

        sf::Text pauseText;
        pauseText.setFont(this->font);
        pauseText.setString("PAUSED");
        pauseText.setCharacterSize(48);
        pauseText.setFillColor(sf::Color::Yellow);
        pauseText.setPosition(320, 170);
        this->window->draw(pauseText);

        sf::Text resumeText;
        resumeText.setFont(this->font);
        resumeText.setString("C - Resume");
        resumeText.setCharacterSize(32);
        resumeText.setFillColor(sf::Color::White);
        resumeText.setPosition(270, 250);
        this->window->draw(resumeText);

        sf::Text restartText;
        restartText.setFont(this->font);
        restartText.setString("R - Restart");
        restartText.setCharacterSize(32);
        restartText.setFillColor(sf::Color::White);
        restartText.setPosition(270, 300);
        this->window->draw(restartText);

        sf::Text quitText;
        quitText.setFont(this->font);
        quitText.setString("Q - Quit");
        quitText.setCharacterSize(32);
        quitText.setFillColor(sf::Color::White);
        quitText.setPosition(270, 350);
        this->window->draw(quitText);
    }

    this->window->display();

    static int frameCounter = 0;
    frameCounter++;
    if (frameCounter == 60)
    {
        sf::Texture screenTex;
        screenTex.create(this->window->getSize().x, this->window->getSize().y);
        screenTex.update(*this->window);
        screenTex.copyToImage().saveToFile("screenshot.png");
    }
}

// Restart the game: reset variables, player, swagBalls, and endGame flag
void Game::restart()
{
    this->initVariables();
    this->player = Player(); // Reset player
    this->swagBalls.clear();
    this->gameState = GameState::Running;
    this->endGame = false;
}

// Resume the game: set state to Running if paused
void Game::resume()
{
    if (this->gameState == GameState::Paused)
        this->gameState = GameState::Running;
}

// Quit the game: close the window and set state to Ended
void Game::quit()
{
    if (this->window->isOpen())
        this->window->close();
    this->gameState = GameState::Ended;
    this->endGame = true;
}
