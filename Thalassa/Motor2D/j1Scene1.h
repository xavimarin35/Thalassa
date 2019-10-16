#ifndef __j1SCENE1_H__
#define __j1SCENE1_H__

#include "j1Module.h"
#include "p2Point.h"
#include "p2Animation.h"
#include "j1Entity.h"
#include "j1EntityManager.h"

struct SDL_Texture;
//class Animation;

enum ACTIVE_SCENE {
	SCENE_TUTORIAL,
	SCENE_LEVEL1,
	SCENE_MIDLEVEL,

	SCENE_NONE
};

class j1Scene1 : public j1Module
{
public:

	j1Scene1();

	// Destructor
	virtual ~j1Scene1();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	bool Load(pugi::xml_node& data);
	bool Save(pugi::xml_node& data) const;

	void LoadNewLevel();

	void LoadSceneInfo();

public:

	bool death = false;
	bool win1 = false;

	bool tutorial_active = true;
	bool level1_active = false;
	bool midlevel_active = false;

	bool midlevel_completed = false;

	bool cameraMoving = true;
	bool lateralMove = false;



private:

	SDL_Texture* keys;

	int cameraLimitX;
	int cameraLimitY;
	int cameraPositionMoving;

	iPoint doorPosition;
	iPoint cameraLimit;

	iPoint obstacle1;
	iPoint obstacle2;

	int differenceY;

	SDL_Rect W, WPressed, A, APressed, D, DPressed, SPACE, SPACEPressed;
	iPoint posA, posD, posW1, posW2, posW3, posW4, posSPACE;
};

#endif // __j1SCENE1_H__