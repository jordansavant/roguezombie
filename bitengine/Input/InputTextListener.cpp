#include "InputTextListener.hpp"

bit::InputTextListener::InputTextListener()
	: input(), isEntered(false), isChanged(false), listen(true)
{
}

void bit::InputTextListener::clear()
{
	input = "";
}

bool bit::InputTextListener::onEntered()
{
	if (isEntered)
	{
		isEntered = false;
		return true;
	}
	return false;
}

bool bit::InputTextListener::onChanged()
{
	if (isChanged)
	{
		isChanged = false;
		return true;
	}
	return false;
}