#include "Animation.hpp"
#include "SFML/Graphics.hpp"
#include "../Game/GameTimer.hpp"
#include "Sprite.hpp"

bit::Animation::Animation()
    : frames(), reverse(false)
{
}

void bit::Animation::load(int frameCount, int frameWidth, int frameHeight, int xOffset, int yOffset, float fps, Direction direction)
{
	this->minFps = 0;
    this->maxFps = 60;
	this->frameWidth = frameWidth;
    this->frameHeight = frameHeight;
	this->framesPerSecond = fps > maxFps ? maxFps : fps < minFps ? minFps : fps;
	this->frameTimer = GameTimer(1 / framesPerSecond);
	this->currentFrame = 0;

    for (int i = 0; i < frameCount; i++)
    {
		switch(direction)
		{
			case Direction::Horizontal:
                frames.push_back(sf::IntRect(xOffset + (frameWidth * i), yOffset, frameWidth, frameHeight));
				break;
			case Direction::Vertical:
				frames.push_back(sf::IntRect(xOffset, yOffset + (frameHeight * i), frameWidth, frameHeight));
				break;
		}
    }
}

void bit::Animation::update(sf::RenderWindow &window, sf::Time &gameTime)
{
	if(frameTimer.update(gameTime))
	{
        if(!reverse)
        {
		    currentFrame = (currentFrame + 1) % frames.size();
        }
        else
        {
		    currentFrame--;
            if(currentFrame < 0)
                currentFrame = frames.size() - 1;
        }
	}

    // Apply current frame to us
    if(currentFrame > -1)
    {
        width = frames[currentFrame].width;
        height = frames[currentFrame].height;
        left = frames[currentFrame].left;
        top = frames[currentFrame].top;
    }
}

void bit::Animation::changeFrame(unsigned int index)
{
    if (frames.size() > index)
    {
        currentFrame = index;
    }
}

void bit::Animation::setFps(int fps)
{
	framesPerSecond = fps > maxFps ? maxFps : fps < minFps ? minFps : fps;
	frameTimer.setNewDuration(framesPerSecond);
}

void bit::Animation::reset()
{
	currentFrame = 0;
}

bit::Animation bit::Animation::fromSprite(Sprite* sprite, int frames, float fps, Direction direction)
{
    int frameWidth = direction == Direction::Horizontal ? sprite->width / frames : sprite->width;
    int frameHeight = direction == Direction::Horizontal ? sprite->height : sprite->height / frames;

    Animation a;
    a.load(frames, frameWidth, frameHeight, sprite->left, sprite->top, fps, direction);

    return a;
}