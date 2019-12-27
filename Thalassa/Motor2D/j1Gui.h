#ifndef __j1GUI_H__
#define __j1GUI_H__

#include "j1Module.h"
#include "p2List.h"
#include <string>
#include "p2Point.h"
#include "p2Defs.h"
#include "SDL/include/SDL_rect.h"

#define CURSOR_WIDTH 2

struct SDL_Texture;
struct SDL_Rect;
struct _TTF_Font;
struct SDL_Color;
class j1Button;
class j1Box;
class j1Label;
class j1UIElement;

enum UI_ELEMENTS
{
	BUTTON,
	INPUT_TEXT,
	LABEL,
	IMAGE,
	BOX,
	CHECKBOX
};

enum BUTTON_FUNCTION
{
	NO_FUNCTION = 0,
	PLAY,
	EXIT
};

class j1Gui : public j1Module
{
public:

	j1Gui();
	virtual ~j1Gui();

	bool Awake(pugi::xml_node&);
	bool Start();
	bool PreUpdate();
	bool PostUpdate();
	bool CleanUp();

	j1Button* CreateButton(p2List<j1Button*>* buttons, UI_ELEMENTS type, int x, int y, SDL_Rect idle, SDL_Rect hovered, SDL_Rect clicked, SDL_Texture* text = nullptr, BUTTON_FUNCTION function = NO_FUNCTION, j1UIElement* parent = nullptr);
	void UpdateButtonState(p2List<j1Button*>* buttons, float scale = 1);

	const SDL_Texture* GetSprites() const;

public:

	// Pixeled.ttf font
	_TTF_Font* font1 = nullptr;
	_TTF_Font* font2 = nullptr;
	_TTF_Font* font3 = nullptr;

	bool debug = false;

	// Colors
	SDL_Color beige;
	SDL_Color brown;
	SDL_Color grey;
	SDL_Color white;

private:

	SDL_Texture* gui_texture = nullptr;
	std::string gui_texture_name;

	bool loadedAudios = false;
};

#endif // !__j1GUI_H__
