#include "j1Button.h"
#include "j1Textures.h"
#include "j1App.h"
#include "p2Log.h"
#include "j1Input.h"
#include "j1Render.h"

j1Button::j1Button(UI_ELEMENTS type, int x, int y, SDL_Rect idle, SDL_Rect hovered, SDL_Rect clicked, SDL_Texture* text, BUTTON_FUNCTION function, j1UIElement* parent) :
	j1UIElement(UI_ELEMENTS::BUTTON, x, y, text, parent), idle(idle), hovered(hovered), clicked(clicked), bfunction(function) {}

j1Button::~j1Button() {}

void j1Button::Draw(float scale, int x, int y, bool use_camera)
{
	if (!use_camera)
		App->render->Blit(sprites, (int)position.x + x, (int)position.y + y, &situation, SDL_FLIP_NONE, false, scale);
	else
		App->render->Blit(sprites, (int)position.x + x, (int)position.y + y, &situation, SDL_FLIP_NONE, true, scale);
}

bool j1Button::CleanUp()
{
	LOG("Unloading button textures");
	App->tex->UnLoad(sprites);


	return true;
}