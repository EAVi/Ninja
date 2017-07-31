#include "UIDrawer.h"

using namespace std;

vector<LTexture*> UIDrawer::gUITextures = vector<LTexture*>();

enum UITextureNums : Uint8
{
	kHealthbar, kHealthbarGradient, kNinjaLifeIcon
};

namespace Healthbar
{
	SDL_Rect dims = { 27,14,64,5 };
	SDL_Rect life = { 28,21,8,8 };
}


UIDrawer::UIDrawer()
{
}

void UIDrawer::drawHealthbar(int & health, int & maxhealth, int & lives) const
{
	gUITextures[kHealthbar]->renderTexture(0, 0);
	int width = Healthbar::dims.w;
	int height = Healthbar::dims.h;
	
	if (health < maxhealth)
	{
		width = width * (health * 1.f / maxhealth);
	}
	
	SDL_Color blue = {0,0,255,255};
	SDL_Color red = { 255,0,0,255 };

	if (width <= Healthbar::dims.w >> 1 /*w divided by 2*/)
		gUITextures[kHealthbarGradient]->colorMod(red);
	else 
		gUITextures[kHealthbarGradient]->colorMod(blue);

	gUITextures[kHealthbarGradient]->renderTexture(
		Healthbar::dims.x,
		Healthbar::dims.y,
		NULL,
		0.0,
		NULL,
		SDL_FLIP_NONE,
		&width,
		&height
		);
	if (lives <= 5 && lives != 0)//shows only a certain amount of lives before it just shows "x6, x7, etc."
		for (int i = 0; i < lives; ++i)
		{
			int xpos = Healthbar::life.x + (Healthbar::life.w + 1) * i;
			
			gUITextures[kNinjaLifeIcon]->renderTexture(
				xpos,
				Healthbar::life.y
			);
		}

	else
	{
		gUITextures[kNinjaLifeIcon]->renderTexture(
			Healthbar::life.x,
			Healthbar::life.y
		);
		string lifecounter = "x";
		lifecounter += to_string(lives);
		gWriter.RenderString(lifecounter, Healthbar::life.x + Healthbar::life.w + 1, Healthbar::life.y, (textColors)8, (textCommand)1);
	}
}
