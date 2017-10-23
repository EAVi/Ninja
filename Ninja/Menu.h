#ifndef MENU_H
#define MENU_H

#include "TextWriter.h"

enum MenuType : Uint8
{
	kMainMenu = 0,
	kMainOptions,
	kPauseMenu,
	kPauseOptions,
	kGameOver,
	kCutscene,
	kInGame,
	kTotalMenus = kCutscene, //cutscene and ingame don't really count as menus
};

//list of all possible options a button could ever need to do
enum ButtonOption : Uint8
{
	//Might give or take Zones by the time the game is complete,
	//therefore I didn't hardcode these numbers
	kNoOption = 0,
	kSetZone1,//warp to Zone 1
	kSetZone2,//warp to Zone 2
	kSetZone3,//warp to Zone 3
	kSetZone4,//warp to Zone 4
	kSetZone5,//warp to Zone 5
	kSetZone6,//warp to Zone 6
	kSetZone7,//warp to Zone 7
	kSetZone8,//warp to Zone 8
	kSetMainMenu,//go to main menu
	kSetMainOptions,//go to options menu (from pause)
	kSetQuit,//quit the game
	kSetPauseMenu,//go to pause menu
	kSetPauseOption,//go to options menu (from pause)
	kRestartZone,//go to the beginning of the zone (used in Game Over screen)
	kUnPause,//unpause the game
	kMusicDecrease,//decrease the music volume
	kMusicIncrease,//increase the music volume
	kSFXDecrease,//decrease the SFX volume
	kSFXIncrease,//increase the SFX volume
	kToggleFullscreen,//toggle full screen
};

//button structure, with a string, an option, and a position on the screen
//as well as an optional active and inactive color
struct Button
{
	std::string text; //what the button will say
	ButtonOption option;
	SDL_Point position;
	std::string prefixA;//prefix for an active string (eg. txt::Red + txt::HLightOn)
	std::string prefixB;//prefix for an inactive string
};

class Menu
{
public:
	//default constructor
	Menu();

	//overloaded constructor
	Menu(MenuType m, std::string s, SDL_Point p = {0,0});

	//adds a button structure to the vector
	void addButton(Button a);

	//overload using the member variables
	void addButton(std::string s, ButtonOption o, SDL_Point p, std::string preA, std::string preB);

	//later versions of the Menu object will use backgrounds
	static std::vector<LTexture> mBackground;
	void renderMenu();
	ButtonOption handleEvent(SDL_Event & e);
private:
	MenuType mMenuType;
	Uint8 mCurrentButton;
	std::string mTitle;
	SDL_Point mTitlePosition;
	std::vector<Button> mButton;
	ButtonOption mPressYes();
	//void mPressReturn();
	ButtonOption mPressNext();
	ButtonOption mPressPrev();

};

#endif