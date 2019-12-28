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
struct _TTF_Font;
struct SDL_Rect;
struct SDL_Color;
class j1UIElement;
class j1Button;
class j1Box;
class j1Label;


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
	CONTINUE_LOADED,
	GO_TO_MENU,
	OPEN_CREDITS,
	SAVE_GAME,
	LOAD_GAME,
	SETTINGS,
	CLOSE_SETTINGS,
	LINK,
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
	void UpdateButtonState(p2List<j1Button*>* buttons);

	j1Box* CreateBox(p2List<j1Box*>* boxes, UI_ELEMENTS type, int x, int y, SDL_Rect section, SDL_Texture* text = nullptr, j1UIElement* parent = nullptr, uint minimum = 0, uint maximum = 0);
	void UpdateWindow(j1Box* window, p2List<j1Button*>* buttons = nullptr, p2List<j1Label*>* labels = nullptr, p2List<j1Box*>* boxes = nullptr);

	j1Label* CreateLabel(p2List<j1Label*>* labels, UI_ELEMENTS type, int x, int y, _TTF_Font* font, const char* text, SDL_Color color = { 255, 255, 255, 255 }, j1UIElement* parent = nullptr);
	void UpdateSliders(p2List<j1Box*>* sliders);

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

	float buttonsScale;
	float settingsWindowScale;
	float logoScale;

	iPoint settingsPosition;

	uint lastSlider1X;
	uint lastSlider2X;
	uint slider1Y;
	uint slider2Y;

private:

	SDL_Texture* gui_texture = nullptr;
	std::string gui_texture_name;

	bool loadedAudios = false;
};

#endif // !__j1GUI_H__
