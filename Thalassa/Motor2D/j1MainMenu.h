#ifndef __j1MAINMENU_H__
#define __j1MAINMENU_H__

#include "j1Module.h"
#include "p2Point.h"
#include "p2Defs.h"

struct SDL_Texture;
struct SDL_Rect;

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

private:

	SDL_Texture* texture = nullptr;

};

#endif // !__j1MAINMENU_H___
