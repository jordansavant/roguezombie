#pragma once
#ifndef ANIMATION_H
#define ANIMATION_H

#include "SFML/Graphics.hpp"
#include "../Game/GameTimer.hpp"
#include "Sprite.hpp"

namespace bit
{
	class Animation : public Sprite
	{
	public:

		Animation();

        enum Direction
		{
			Horizontal,
			Vertical
		};

		int frameWidth;
		int frameHeight;
		float framesPerSecond;
		int currentFrame;
        bool reverse;

		void load(int frameCount, int frameWidth, int frameHeight, int xOffset, int yOffset, float fps, Direction direction);

        virtual void update(sf::RenderWindow &window, sf::Time &gameTime);

        void changeFrame(unsigned int index);

		void setFps(int fps);

		void reset();

        static Animation fromSprite(Sprite* sprite, int frames, float fps, Direction direction = Direction::Horizontal);


	protected:

        std::vector<sf::IntRect> frames;
		GameTimer frameTimer;
		int minFps;
		int maxFps;
	};
}

#endif
