#ifndef __j1LABEL_H__
#define __j1LABEL_H__

#include "j1UIElement.h"

struct _TTF_Font;

class j1Label : public j1UIElement {
public:
	// Constructor
	j1Label(UI_ELEMENTS type, int x, int y, _TTF_Font* font, const char* text, SDL_Color color = { 255, 255, 255, 255 }, j1UIElement* parent = nullptr);

	// Destructor
	virtual ~j1Label();

	// Called before quitting
	bool CleanUp();

	// Blits the element on the screen
	void Draw(float scale = 1.0f, int x = 0, int y = 0, bool use_camera = true);

public:
	_TTF_Font* font = nullptr;
	SDL_Color color;
	const char* text = nullptr;

private:
	SDL_Rect temp;
};

#endif // __j1LABEL_H__