#ifndef __j1MAINMENU_H__
#define __j1MAINMENU_H__

#include "j1Module.h"
#include "p2Point.h"
#include "p2Defs.h"

struct SDL_Texture;
struct SDL_Rect;

class j1MainMenu : public j1Module
{
public:

	j1MainMenu();
	~j1MainMenu();

	bool Start();
	bool Update(float dt);
	bool PostUpdate();

private:

	SDL_Texture* texture = nullptr;

};

#endif // !__j1MAINMENU_H___
