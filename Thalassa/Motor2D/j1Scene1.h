#ifndef __j1SCENE1_H__
#define __j1SCENE1_H__

#include "j1Module.h"
#include "p2Point.h"
#include "p2Animation.h"
#include "j1Entity.h"
#include "j1EntityManager.h"
#include "j1Timer.h"

struct SDL_Texture;
class Animation;

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

	void LevelChangeLogic();

	void LoadTutorial();
	void LoadLevel1();
	void LoadMidLevel();

	void CameraMovement(float dt);
	void BlitKeys();

	void LoadNewLevel();

	void LoadSceneInfo();

	void SpawnTutorialEntities();
	void SpawnLevel1Entities();
	void SpawnMidLevelEntities();

	ENTITY_TYPE RandomItem();

public:

	bool death = false;
	bool win1 = false;

	bool tutorial_active = true;
	bool level1_active = false;
	bool midlevel_active = false;

	bool midlevel_completed = false;

	bool cameraMoving = true;
	bool lateralMove = false;

	bool cullingView = false;

	j1Timer scene_timer;

private:
	
	Animation mouseBlinking;

	SDL_Texture* keys;
	SDL_Texture* mouse;

	int cameraLimitX;
	int cameraLimitY;
	int cameraPositionMoving;

	iPoint doorPosition;
	iPoint cameraLimit;

	iPoint obstacle1;
	iPoint obstacle2;

	int differenceY;

	SDL_Rect W, WPressed, A, APressed, D, DPressed, SPACE, SPACEPressed, mouseLeft, mouseRight, mouseIdle;
	iPoint posA, posD, posW1, posW2, posW3, posW4, posSPACE, posMLeft, posMRight, posMIdle;
};

#endif // __j1SCENE1_H__