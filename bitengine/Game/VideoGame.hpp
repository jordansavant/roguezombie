#pragma once
#ifndef BIT_VIDEOGAME_H
#define BIT_VIDEOGAME_H

#include "SFML/Graphics.hpp"
#include "Game.hpp"
#include "../System/PlatformDefinitions.hpp"

#if defined(__OS_WIN__)
#if defined(_DEBUG)
#include "vld.h"
#endif
#endif

namespace bit
{
    class GameComponent;
    class InputManager;
    class SoundManager;
    class StateStack;
    class MusicManager;
    class SpriteLoader;

	class VideoGame : public Game
	{
	public:
 		VideoGame(std::string gameTitle, int width, int height, bool fullscreen);

        virtual ~VideoGame();

        InputManager* inputManager;
        SoundManager* soundManager;
        MusicManager* musicManager;
        SpriteLoader* spriteLoader;

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
        bool verticalSync;

		void run();

		virtual void update(sf::Time &gameTime);

		virtual void draw(sf::RenderWindow &window, sf::Time &gameTime);

        virtual void registerStates();

        void quit();

        void setVerticalSync(bool vSync);

        void configureOpenGL();

        static void changeResolution(int width, int height, bool change = true);

        static void changeFullscreen(bool fullscreen);

        static void depthTestBegin();

        static void depthTestEnd();

	protected:
        static bool graphicsChange;
	};
}

#endif