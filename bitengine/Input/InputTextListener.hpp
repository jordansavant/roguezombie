#pragma once
#ifndef BIT_INPUTTEXTLISTENER_H
#define BIT_INPUTTEXTLISTENER_H

#include <string>

namespace bit
{
	class InputTextListener
	{
	public:

		InputTextListener();

		std::string input;
		bool isEntered;
		bool isChanged;
		bool listen;
		std::string ignoreCharacters;

		bool onEntered();

		bool onChanged();

		void clear();
	};
}

#endif