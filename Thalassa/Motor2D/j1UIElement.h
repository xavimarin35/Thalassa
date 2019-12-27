#ifndef __USERINTERFACEELEMENT_H__
#define __USERINTERFACEELEMENT_H__

#include "j1Gui.h"
#include "p2Point.h"
#include "SDL/include/SDL_rect.h"

struct SDL_Texture;
class j1UIElement {

public:
	//Constructor
	j1UIElement(UI_ELEMENTS type, int x, int y, SDL_Texture* sprites = nullptr, j1UIElement* parent = nullptr) :
		type(type), position(x, y), initialPosition(x, y), sprites(sprites), parent(parent) {}

	~j1UIElement() {}

	// Blits the element on the screen
	virtual void Draw(float scale, int x = 0, int y = 0, bool use_camera = true) {}

public:
	UI_ELEMENTS type;
	bool visible = true;

	iPoint initialPosition;
	iPoint position;

	SDL_Texture* sprites = nullptr;
	j1UIElement* parent = nullptr;
};

#endif // __USERINTERFACEELEMENT_H__