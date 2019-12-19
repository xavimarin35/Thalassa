#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "j1UIElement.h"

enum UI_ELEMENTS;
struct SDL_Texture;
struct SDL_Rect;
enum BUTTON_FUNCTION;

enum STATE {
	IDLE = 0,
	HOVERED,
	CLICKED,
	RELEASED
};

class j1Button : public j1UIElement {
public:

	// Constructor
	// To create a button we need several parameters: its type, a texture, the three sections of the atlas the button will use and the function of the button
	j1Button(UI_ELEMENTS type, int x, int y, SDL_Rect idle, SDL_Rect hovered, SDL_Rect clicked, SDL_Texture* text = nullptr, BUTTON_FUNCTION function = BUTTON_FUNCTION::NO_FUNCTION, j1UIElement* parent = nullptr);

	// Destructor
	virtual ~j1Button();

	// Called before quitting
	bool CleanUp();

	// Blits the element on the screen
	void Draw(float scale = 1.0f, int x = 0, int y = 0, bool use_camera = true) override;

public:
	STATE state = IDLE;
	BUTTON_FUNCTION bfunction;

	// Visual situation of the button
	SDL_Rect situation;

	SDL_Rect idle;
	SDL_Rect hovered;
	SDL_Rect clicked;

	bool hoverPlayed = false;
	bool clickPlayed = false;
};

#endif // __BUTTON_H__