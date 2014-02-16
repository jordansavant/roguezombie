#pragma once
#ifndef BIT_GAME_H
#define BIT_GAME_H

#include "SFML/Graphics.hpp"

namespace bit
{
    class GameComponent;
    class InputManager;
    class SoundManager;
    class StateStack;
    class MusicManager;
    class SpriteLoader;

	class Game
	{
	public:
 		Game(std::string gameTitle, int width, int height, bool fullscreen);

        virtual ~Game();

        static InputManager* inputManager;
        static StateStack* stateStack;
        static SoundManager* soundManager;
        static MusicManager* musicManager;
        static SpriteLoader* spriteLoader;
        static Game* instance;
        static bool isPaused;
        static bool isInFocus;
        static sf::Vector2i currentResolution;
        static sf::Vector2i targetResolution;
        static float currentResolutionRatio;
        static float currentResolutionRatioX;
        static float currentResolutionRatioY;
        static bool isFullscreen;
        static float targetFpsInterval;

        std::string title;
		sf::RenderWindow* renderWindow;
        sf::ContextSettings settings;
		sf::Clock clock;
		float FPS;
		std::vector<GameComponent*> gameComponents;
        bool verticalSync;

		void run();

		virtual void update(sf::RenderWindow &window, sf::Time &gameTime);

		virtual void draw(sf::RenderWindow &window, sf::Time &gameTime);

        virtual void registerStates();

        void quit();

        void setVerticalSync(bool vSync);

        void configureOpenGL();

        static void changeResolution(int width, int height, bool change = true);

        static void changeFullscreen(bool fullscreen);

        static void depthTestBegin();

        static void depthTestEnd();

	private:
        static bool graphicsChange;
		float actualTime;
		float dt;
		float timer;
        bool running;
	};
}

#endif