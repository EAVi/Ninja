#include "Menu.h"

extern TextWriter gWriter;

using namespace std;

Menu::Menu()
	: Menu(kMainMenu,std::string())//call the overloaded constructor, initialize with empty string
{}

Menu::Menu(MenuType m, std::string s)
{
	mMenuType = m;
	mTitle = s;
	mCurrentButton = 0;
}


void Menu::addButton(Button a)
{
	mButton.push_back(a);
}

void Menu::addButton(std::string s, ButtonOption o, SDL_Point p, std::string preA, std::string preB)
{
	addButton({ s,o,p,preA,preB });
}

void Menu::renderMenu()
{
	gWriter(Large, mTitlePosition.x, mTitlePosition.y) << mTitle;

	for (int i = 0, j = mButton.size(); i < j; ++i)
	{
		//set the text options prefixes depending on if the button is active
		if (mCurrentButton == i)
			gWriter(Small) << mButton[i].prefixA;
		else
			gWriter(Small) << mButton[i].prefixB;

		//draw the text to the position
		gWriter(Small,mButton[i].position.x,mButton[i].position.y) << mButton[i].text;

		//clears the buffer for each, so the next string can be at a different position
		gWriter.ClearBuffer();
	}
}

ButtonOption Menu::handleEvent(SDL_Event & e)
{
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)//keyboard press
	{
		switch (e.key.keysym.sym)
		{
		case SDLK_d: return mPressNext(); break;
		case SDLK_a: return mPressPrev();  break;
		case SDLK_SPACE: return mPressYes(); break;
		case SDLK_m: return mPressYes(); break;
		case SDLK_n: return mPressYes(); break;
		}
	}
	
	else if (e.type == SDL_CONTROLLERBUTTONDOWN)//controller press
	{
		switch (e.cbutton.button)
		{
		case SDL_CONTROLLER_BUTTON_A: return mPressYes(); break;
		case SDL_CONTROLLER_BUTTON_X: return mPressYes(); break;
		case SDL_CONTROLLER_BUTTON_B: return mPressYes(); break;
		case SDL_CONTROLLER_BUTTON_DPAD_LEFT: return mPressPrev(); break;
		case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: return mPressNext(); break;
		}

	}
}

ButtonOption Menu::mPressYes()
{
	return mButton[mCurrentButton].option;
}

ButtonOption Menu::mPressNext()
{
	return kNoOption;
}

ButtonOption Menu::mPressPrev()
{
	return kNoOption;
}


