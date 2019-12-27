#ifndef __j1MAINMENU_H__
#define __j1MAINMENU_H__

#include "j1Module.h"
#include "p2Point.h"
#include "p2Defs.h"
#include "p2List.h"

struct SDL_Texture;
struct SDL_Rect;
class j1Button;
class j1Label;
class j1Box;

enum SCENE_TYPE
{
	MENU = 0, 
	SCENE, 
	CREDITS
};

class j1MainMenu : public j1Module
{
public:

	j1MainMenu();
	virtual ~j1MainMenu();

	bool Awake(pugi::xml_node&);
	bool Start();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

	void ChangeScene(SCENE_TYPE next);

	bool startScene1 = false;
	bool startGame = false;
	bool startCredits = false;

	p2List<j1Button*> buttons_menu;
	p2List<j1Label*> labels_menu;
	p2List<j1Box*> boxes_menu;

public:

	j1Box* settings_window = nullptr;

private:

	SDL_Texture* texture = nullptr;
	SDL_Texture* gui_texture = nullptr;

};

#endif // !__j1MAINMENU_H___
