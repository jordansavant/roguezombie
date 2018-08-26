#include "DevTerminal.hpp"
#include "Hud.hpp"
#include "../StateGamePlay.hpp"
#include "../RogueZombieGame.hpp"
#include "../../bitengine/Input.hpp"
#include "../../bitengine/System.hpp"

DevTerminal::DevTerminal(Hud* _hud)
	: Frame(_hud, 10, -30, _hud->targetWidth - 20, 30, bit::Element::AnchorType::TopLeft, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), false),
	blinker(1), blink(false)
{
	scaleStyle = ScaleStyle::PowerOfTwo;

	inputTextListener = hud->state->rogueZombieGame->inputManager->getInputTextListener();
	inputTextListener->ignoreCharacters = "`";

	input = new bit::Label(0, 0, 0, 0, bit::Element::AnchorType::TopLeft);
	input->setSfFontSize(Hud::font_noteSize);
	input->setSfFont(hud->journalFont);
	input->normalColor = sf::Color(0, 255, 0);
	input->setSfFontString(" > ");
	input->paddingBottom = 10;
	addChild(input);
}

void DevTerminal::update(sf::RenderWindow &window, sf::Time &gameTime)
{
	Frame::update(window, gameTime);

	if (blinker.update(gameTime))
	{
		blink = !blink;
	}

	if (inputTextListener->onEntered())
	{
		// Do something with the command
		processCommand(inputTextListener->input);
		inputTextListener->clear();
	}

	if (inputTextListener->onChanged())
	{
		blink = false;
		blinker.reset();
	}

	input->setSfFontString(" > " + inputTextListener->input + (blink ? "_" : ""));
}

void DevTerminal::hide()
{
	inputTextListener->listen = false;
	canHaveFocus = false;
	clearEffects();
	relativePosition.y = 0;
	immediateEffect(new bit::MoveEffect(300, bit::Easing::OutQuart, 0, -targetHeight));
	immediateEffect(new bit::FadeEffect(150, 0));
}

void DevTerminal::show()
{
	inputTextListener->listen = true;
	canHaveFocus = true;
	clearEffects();
	relativePosition.y = -targetHeight;
	immediateEffect(new bit::MoveEffect(300, bit::Easing::OutQuart, 0, targetHeight));
	immediateEffect(new bit::FadeEffect(300, 1));
}

void DevTerminal::processCommand(std::string command)
{
	std::vector<std::string> args = bit::String::split(command, ' ');

	if (args.size() > 0)
	{
		std::string op = args[0];

		if (op == "killall")
		{
			Command cmd;
			cmd.type = Command::Type::Dev_KillAll;
			hud->state->issueCommand(cmd);
		}
	}
}