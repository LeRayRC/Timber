// Timber.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//

#include <iostream>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

using namespace sf;

// Function declaration prototype
void updateBranches(int seed);
void resetBranches();
void updateFPStext(float dt, Text &text);

const int NUM_BRANCHES = 6;

enum class side { NONE, RIGHT, LEFT };
side branchPositions[NUM_BRANCHES];

Sprite branches[NUM_BRANCHES];

int main()
{
	/* BACKGROUND*/
    // Create a video mode object
    VideoMode vm(1920, 1080);

    RenderWindow window(vm, "Timber!!", Style::Default);

	// Create a texture to hold a graphic on the GPU
	Texture textureBackground;

	// Load a graphic into the texture
	textureBackground.loadFromFile("graphics/background.png");

	// Create a sprite
	Sprite spriteBackground;

	// Attach the texture to the sprite
	spriteBackground.setTexture(textureBackground);

	// Set the spriteBackground to cover the screen
	spriteBackground.setPosition(0, 0);

	// Make tree sprite
	Texture textureTree;
	textureTree.loadFromFile("graphics/tree.png");
	Sprite spriteTree;
	spriteTree.setTexture(textureTree);
	spriteTree.setPosition(810, 0);

	// Prepare the bee
	Texture textureBee;
	textureBee.loadFromFile("graphics/bee.png");
	Sprite spriteBee;
	spriteBee.setTexture(textureBee);
	spriteBee.setPosition(-200, 800);

	// Is the be moving
	bool beeActive = false;

	// How fast can a bee fly
	float beeSpeed = 0.0f;

	// Make 3 cloud sprites from 1 texture
	Texture textureCloud;

	// Load 1 new texture
	textureCloud.loadFromFile("graphics/cloud.png");

	const int NUM_CLOUDS = 4;

	// Arrays for clouds parameters
	Sprite spriteClouds[NUM_CLOUDS];
	bool cloudsActive[NUM_CLOUDS] {false, false, false};
	float cloudsSpeed[NUM_CLOUDS] {0.0f, 0.0f, 0.0f};

	for (int i = 0; i < NUM_CLOUDS; i++) {
		spriteClouds[i].setTexture(textureCloud);
		spriteClouds[i].setPosition(-500, 0 + i * 250);
	}

	/*BACKGROUND*/

	Texture textureBranch;
	textureBranch.loadFromFile("graphics/branch.png");

	for (int i = 0; i < NUM_BRANCHES; i++) {
		branches[i].setTexture(textureBranch);
		branches[i].setPosition(-2000, -2000);
		branches[i].setOrigin(220, 20);
	}

	// Variables to control time itself
	Clock clock;

	// Time Bar

	RectangleShape timeBar;
	float timeBarStartWidth = 400;
	float timeBarHeight = 80;
	timeBar.setSize(Vector2f(timeBarStartWidth, timeBarHeight));
	timeBar.setFillColor(Color::Red);
	timeBar.setPosition((1920 / 2) - timeBarStartWidth / 2, 980);

	Time gameTimeTotal;
	float timeRemaining = 6.0f;
	float timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;


	// Track wheter the game is running
	bool paused = true;

	// Draw some text
	int score = 0;

	Text messageText;
	Text scoreText;
	Text fpsText;

	// Choose a font
	Font font;
	font.loadFromFile("fonts/KOMIKAP_.ttf");

	// Set the font to our message
	messageText.setFont(font);
	scoreText.setFont(font);
	fpsText.setFont(font);

	//Make texts big
	messageText.setCharacterSize(75);
	scoreText.setCharacterSize(100);
	fpsText.setCharacterSize(100);

	// Set the font to our message
	messageText.setString("Press Enter to start!!");
	scoreText.setString("Score = 0");
	fpsText.setString("FPS = IDK");

	// Choose a color
	messageText.setFillColor(Color::White);
	scoreText.setFillColor(Color::White);
	fpsText.setFillColor(Color::White);


	// Position the text
	FloatRect textRect = messageText.getLocalBounds();

	messageText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
	messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);

	scoreText.setPosition(20, 20);
	fpsText.setPosition(2000, 20);

	int seed_counter = 0;
	bool acceptInput = true;

	/*SOUNDS*/
	//Prepare the sounds
	SoundBuffer chopBuffer;
	chopBuffer.loadFromFile("sound/chop.wav");
	Sound chop;
	chop.setBuffer(chopBuffer);

	// The player has met his end under a branch
	SoundBuffer deathBuffer;
	deathBuffer.loadFromFile("sound/death.wav");
	Sound death;
	death.setBuffer(deathBuffer);
	// Out of time
	SoundBuffer ootBuffer;
	ootBuffer.loadFromFile("sound/out_of_time.wav");
	Sound outOfTime;
	outOfTime.setBuffer(ootBuffer);

	/*SOUNDS*/

	/*PLAYER*/
	// Prepare the player
	Texture texturePlayer;
	texturePlayer.loadFromFile("graphics/player.png");
	Sprite spritePlayer;
	spritePlayer.setTexture(texturePlayer);
	spritePlayer.setPosition(580, 720);

	side playerSide = side::LEFT;

	//Prepare the gravestone
	Texture textureRIP;
	textureRIP.loadFromFile("graphics/rip.png");
	Sprite spriteRIP;
	spriteRIP.setTexture(textureRIP);
	spriteRIP.setPosition(675, 2000);

	// Prepare the axe
	Texture textureAxe;
	textureAxe.loadFromFile("graphics/axe.png");
	Sprite spriteAxe;
	spriteAxe.setTexture(textureAxe);
	spriteAxe.setPosition(700, 830);

	// Line the axe up with the tree
	const float AXE_POSITION_LEFT = 700;
	const float AXE_POSITION_RIGHT = 1075;

	// Prepare the flying log
	Texture textureLog;
	textureLog.loadFromFile("graphics/log.png");
	Sprite spriteLog;
	spriteLog.setTexture(textureLog);
	spriteLog.setPosition(2810, 720);

	// Some other useful log related variables
	bool logActive = false;
	float logSpeedX = 1000;
	float logSpeedY = -1500;

	/*PLAYER*/

	float fpsUpdateInterval = 1.5;
	float waitingTime = 0.0f;

	while (window.isOpen())
	{

		/*
		****************************************
		Handle the players input
		****************************************
		*/

		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::KeyReleased && !paused)
			{
				// Listen for key presses again
				acceptInput = true;
				// hide the axe
				spriteAxe.setPosition(2000,
					spriteAxe.getPosition().y);
			}

			if (event.type == Event::TextEntered && !paused)
			{
				// Listen for key presses again
				acceptInput = true;
				// hide the axe
				spriteAxe.setPosition(2000,
					spriteAxe.getPosition().y);
			}

		}
		
		if (Keyboard::isKeyPressed(Keyboard::Escape))
		{
			window.close();
		}

		if (Keyboard::isKeyPressed(Keyboard::Return) && paused) {
			paused = false;

			resetBranches();
			spritePlayer.setPosition(580, 720);
			spriteRIP.setPosition(2000, 2000);

			// Reset the time and the score
			score = 0;
			timeRemaining = 6;
		}

		//if (Keyboard::isKeyPressed(Keyboard::Space) && acceptInput) {
		//	updateBranches(seed_counter % 100);
		//	seed_counter++;
		//	acceptInput = false;
		//}

		if (Keyboard::isKeyPressed(Keyboard::F1) && acceptInput) {
			if (fpsText.getPosition().x > 1920) {
				fpsText.setPosition(1360, 20);
			} 
			else {
				fpsText.setPosition(2000, 20);
			}
			acceptInput = false;
		}

		if (acceptInput && !paused) {
			if (Keyboard::isKeyPressed(Keyboard::Right)) {
				playerSide = side::RIGHT;
				score++;
				// Add to the amount of time remaining
				timeRemaining += (2 / score) + .15;
				spriteAxe.setPosition(AXE_POSITION_RIGHT,
					spriteAxe.getPosition().y);
				spritePlayer.setPosition(1200, 720);
				// Update the branches
				updateBranches(score);
				//updateFPStext(fpsRate.asSeconds());
				// Set the log flying to the left
				spriteLog.setPosition(810, 720);
				logSpeedX = -5000;
				logActive = true;
				acceptInput = false;
				//Play a chop sound
				chop.play();
			}
			if (Keyboard::isKeyPressed(Keyboard::Left)) {
				playerSide = side::LEFT;
				score++;
				// Add to the amount of time remaining
				timeRemaining += (2 / score) + .15;
				spriteAxe.setPosition(AXE_POSITION_LEFT,
					spriteAxe.getPosition().y);
				spritePlayer.setPosition(580, 720);
				// Update the branches
				updateBranches(score);
				//updateFPStext(fpsRate.asSeconds());
				// Set the log flying to the left
				spriteLog.setPosition(810, 720);
				logSpeedX = 5000;
				logActive = true;
				acceptInput = false;
				//Play a chop sound
				chop.play();
			}

		}
		
		/*
		****************************************
		Update the scene
		****************************************
		*/

		if (!paused) {

			// Measure time
			Time dt = clock.restart();

			waitingTime += dt.asSeconds();
			//std::cout << waitingTime << "\n";
			if (waitingTime > fpsUpdateInterval) {
				waitingTime = 0.0f;
				updateFPStext(dt.asSeconds(), fpsText);
			}

			// Subtract from the amount of time remaining
			timeRemaining -= dt.asSeconds();
			// size up the time bar
			timeBar.setSize(Vector2f(timeBarWidthPerSecond *
				timeRemaining, timeBarHeight));

			if (timeRemaining <= 0.0f) {

				// Pause the game
				paused = true;
				// Change the message shown to the player
				messageText.setString("\t\tOut of time!! \nPress Enter to Restart");
				//Reposition the text based on its new size
				FloatRect textRect = messageText.getLocalBounds();
				messageText.setOrigin(textRect.left +
					textRect.width / 2.0f,
					textRect.top +
					textRect.height / 2.0f);
				messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);
				outOfTime.play();
			}

			// Setup the bee
			if (!beeActive) {
				// How fast is the bee
				srand((int)time(0));
				beeSpeed = (rand() % 200) + 200;

				// How high is the bee
				srand((int)time(0));
				float height = (rand() % 500) + 500;
				spriteBee.setPosition(2000, height);
				beeActive = true;
			}
			// Move the bee
			else {
				spriteBee.setPosition(
					spriteBee.getPosition().x -
					(beeSpeed * dt.asSeconds()),
					spriteBee.getPosition().y);

				// Hast the bee reached the left-hand edge of the screen
				if (spriteBee.getPosition().x < -100) {
					// Set it up reade to be a whole new bee on next frame
					beeActive = false;
				}
			}

			for (int i = 0; i < NUM_CLOUDS; i++) {
				if (!cloudsActive[i]) {
					// How fast is the cloud
					srand((int)time(0) * ( i  * 10));
					cloudsSpeed[i] = (rand() % 200 + 50);

					// How high is the cloud
					srand((int)time(0) * (i * 10));
					float height = (rand() % 150 + (i * 50));
					spriteClouds[i].setPosition(-200, height);
					cloudsActive[i] = true;
				}
				else {
					spriteClouds[i].setPosition(
						spriteClouds[i].getPosition().x +
						(cloudsSpeed[i] * dt.asSeconds()),
						spriteClouds[i].getPosition().y);
					// Has the cloud reached the right hand edge of the screen?
					if (spriteClouds[i].getPosition().x > 1920)
					{
						// Set it up ready to be a whole new cloud next frame
						cloudsActive[i] = false;
					}
				}
			}

			// Update the score text

			std::stringstream ss;
			ss << "Score = " << score;
			scoreText.setString(ss.str());



			//ss2 << "FPS = " << int(1 / dt.asSeconds());

			//fpsText.setString(ss2.str());

			// update the branch sprites
			for (int i = 0; i < NUM_BRANCHES; i++)
			{
				float height = i * 150;
				if (branchPositions[i] == side::LEFT)
				{
					// Move the sprite to the left side
					branches[i].setPosition(610, height);
					// Flip the sprite round the other way
					branches[i].setRotation(180);
				}
				else if (branchPositions[i] == side::RIGHT)
				{
					// Move the sprite to the right side
					branches[i].setPosition(1330, height);
					// Set the sprite rotation to normal
					branches[i].setRotation(0);
				}
				else
				{
					// Hide the branch
					branches[i].setPosition(3000, height);
				}
			}

			// Handle a flying log 
			if (logActive)
			{
				spriteLog.setPosition(
					spriteLog.getPosition().x +
					(logSpeedX * dt.asSeconds()),

					spriteLog.getPosition().y +
					(logSpeedY * dt.asSeconds()));
				// Has the log reached the right hand edge?
				if (spriteLog.getPosition().x < -100 ||
					spriteLog.getPosition().x > 2000)
				{
					// Set it up ready to be a whole new log next frame
					logActive = false;
					spriteLog.setPosition(810, 720);
				}
			}

			// has the player been squished by a branch?
			if (branchPositions[5] == playerSide)
			{
				// death
				paused = true;
				acceptInput = false;

				// Draw the gravestone
				spriteRIP.setPosition(525, 760);
				// hide the player
				spritePlayer.setPosition(2000, 660);
				spriteAxe.setPosition(2000, 830);
				// Change the text of the message
				messageText.setString("\t\tSQUISHED!! \nPress Enter to Restart");
				// Center it on the screen
				FloatRect textRect = messageText.getLocalBounds();
				messageText.setOrigin(textRect.left +
					textRect.width / 2.0f,
					textRect.top + textRect.height / 2.0f);
				messageText.setPosition(1920 / 2.0f,
					1080 / 2.0f);
				//Play death sound
				death.play();
			}
			

		}

		/*
		****************************************
		Draw the scene
		****************************************
		*/

		// Clear everything from the last frame
		window.clear();

		// Draw sprite backgroud
		window.draw(spriteBackground);

		//Draw the clouds
		for (int i = 0; i < NUM_CLOUDS; i++) {
			window.draw(spriteClouds[i]);
		}

		// Draw the branches
		for (int i = 0; i < NUM_BRANCHES; i++) {
			window.draw(branches[i]);
		}

		//Draw the tree
		window.draw(spriteTree);

		//Draw the player
		window.draw(spritePlayer);

		window.draw(spriteAxe);

		// Draw the flying log
		window.draw(spriteLog);
		// Draw the gravestone
		window.draw(spriteRIP);

		//Draw the bee
		window.draw(spriteBee);

		// Draw texts
		window.draw(scoreText);
		window.draw(fpsText);

		// Draw the timebar
		window.draw(timeBar);

		if (paused) {
			window.draw(messageText);
		}

		// Show everything we just drew
		window.display();


		}

	return 0;
}

// Ejecutar programa: Ctrl + F5 o menú Depurar > Iniciar sin depurar
// Depurar programa: F5 o menú Depurar > Iniciar depuración

// Sugerencias para primeros pasos: 1. Use la ventana del Explorador de soluciones para agregar y administrar archivos
//   2. Use la ventana de Team Explorer para conectar con el control de código fuente
//   3. Use la ventana de salida para ver la salida de compilación y otros mensajes
//   4. Use la ventana Lista de errores para ver los errores
//   5. Vaya a Proyecto > Agregar nuevo elemento para crear nuevos archivos de código, o a Proyecto > Agregar elemento existente para agregar archivos de código existentes al proyecto
//   6. En el futuro, para volver a abrir este proyecto, vaya a Archivo > Abrir > Proyecto y seleccione el archivo .sln

void updateBranches(int seed)
{
	// Move all the branches down one place
	for (int j = NUM_BRANCHES - 1; j > 0; j--) {
		branchPositions[j] = branchPositions[j - 1];
	}

	// Spawn a new branch at position 0
	// LEFT, RIGHT or NONE
	srand((int)time(0) + seed);
	int r = (rand() % 5);
	switch (r) {
	case 0:
		branchPositions[0] = side::LEFT;
		break;
	case 1:
		branchPositions[0] = side::RIGHT;
		break;
	default:
		branchPositions[0] = side::NONE;
		break;
	}
}

void resetBranches() {
	for (int j = NUM_BRANCHES - 1; j > 0; j--) {
		branchPositions[j] = side::NONE;
	}
}

void updateFPStext(float dt, Text &text) {
	std::stringstream ss;
	ss << "FPS = " << int(1 / dt);
	text.setString(ss.str());
}