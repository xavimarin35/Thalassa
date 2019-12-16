#ifndef __j1HUD_H__
#define __j1HUD_H__

#include "j1Timer.h"
#include <string>
#include <list>
#include "PugiXml/src/pugixml.hpp"
#include "p2Animation.h"

class j1Button;
class j1Label;

struct _TTF_Font;
struct SDL_Texture;

class j1Hud
{
public:
	j1Hud();
	~j1Hud();

	bool Start();
	bool Update(float dt);
	bool CleanUp();

public:

	float life_points_max, life_points;
	char const *current_points;

private:
	SDL_Texture* hud_text = nullptr;

	std::list<j1Label*> labels_list;
	std::list<j1Button*> hud_buttons;

};

#endif // __j1HUD_H__