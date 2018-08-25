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
		bool listen;

		bool onEntered();

		void clear();
	};
}

#endif