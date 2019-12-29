#ifndef __j1SCENE1_H__
#define __j1SCENE1_H__

#include "j1Module.h"
#include "p2Point.h"
#include "p2Animation.h"
#include "j1Entity.h"
#include "j1EntityManager.h"
#include "j1Timer.h"
#include "p2DynArray.h"

struct SDL_Texture;
struct _TTF_Font;
class j1Box;
class Animation;
class j1Button;
class j1Label;


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
	void EndRun();

	void LoadSceneInfo();

	void PrintSettingsWindow();

	void SpawnTutorialEntities();
	void SpawnLevel1Entities();
	void SpawnMidLevelEntities();

	void SpawnTutorialCoins();
	void SpawnLevel1Coins();
	void SpawnMidLevelCoins();

	void ReSpawnEntities();
	void DrawJetLife();

	void ShowCursor(bool hide);
	
	void DrawPath(p2DynArray<iPoint>* path);

	ENTITY_TYPE RandomItem();

public:

	bool death = false;
	bool win1 = false;

	bool scene1_active = false;

	bool tutorial_active = false;
	bool level1_active = false;
	bool midlevel_active = false;

	bool midlevel_completed = false;

	bool cameraMoving = true;
	bool lateralMove = false;

	bool cullingView = false;

	j1Timer scene_timer;

	bool done_anim = false;

	int player_lifes = 3;

	j1Box* settings_window = nullptr;

	p2List<j1Button*> scene1Buttons;
	p2List<j1Label*> scene1Labels;
	p2List<j1Box*> scene1Boxes;

	std::string current_points;

	SDL_Rect continue_idle, continue_hover, continue_click;
	SDL_Rect save_idle, save_hover, save_click;
	SDL_Rect return_idle, return_hover, return_click;

private:

	j1Timer deathTimer;

	bool clicked = false;
	Animation* mouseBlinking;
	Animation blinkAnim;

	SDL_Texture* keys;
	SDL_Texture* mouse;
	SDL_Texture* debugPath;
	SDL_Texture* jetPack_tex;
	SDL_Texture* cursor_tex;
	SDL_Texture* settings_window_text;
	SDL_Texture* buttons_text;

	int cameraLimitX;
	int cameraLimitY;
	int cameraPositionMoving;
	int timer;

	fPoint window_pos;

	int score_player = 0;
	int score;

	bool saved_score = false;

	bool hide_cursor = true;

	iPoint doorPosition;
	iPoint cameraLimit;
	iPoint camera_speed;

	iPoint batPos1, batPos2, batPos3, batPos4;
	iPoint demonPos1, demonPos2;

	iPoint item1, item2, item3, item4, item5, item6, item7, item8;

	SDL_Rect cursor;
	fPoint mouse_position;

	iPoint obstacle1;
	iPoint obstacle2;

	int differenceY;

	SDL_Rect jetPackBar, jetPackLife;
	iPoint pos_bar, pos_bar2;

	SDL_Rect W, WPressed, A, APressed, D, DPressed, SPACE, SPACEPressed, mouseLeft, mouseRight, mouseIdle;
	iPoint posA, posD, posW1, posW2, posW3, posW4, posSPACE, posMLeft, posMRight, posMIdle;
};

#endif // __j1SCENE1_H__