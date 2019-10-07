#ifndef __j1SCENE1_H__
#define __j1SCENE1_H__

#include "j1Module.h"

struct SDL_Texture;

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

	void LoadNewLevel();

public:

	bool death = false;
	bool win1 = false;

	bool tutorial_active = true;
	bool level1_active = false;
	bool midlevel_active = false;

	bool midlevel_completed = false;

	bool cameraMoving = true;
	bool lateralMove = false;

	int cameraLimitX;
	int cameraLimitY;
};

#endif // __j1SCENE1_H__