#ifndef __J1ENTITYMANAGER_H__
#define __J1ENTITYMANAGER_H__

#include "j1Module.h"
#include "p2Defs.h"
#include "p2List.h"
#include "p2Point.h"
#include "j1Entity.h"

#define MAX_ENTITIES 50

class j1Entity;
class j1Player;
class j1Chest;
class j1MovingObstacle;
class j1Door;
struct SDL_Texture;

enum ENTITY_TYPE
{
	PLAYER,
	CHEST,
	OBSTACLE,
	DOOR,
	NONE
};

struct EntityInfo
{
	ENTITY_TYPE type = ENTITY_TYPE::NONE;
	fPoint position;
};

class j1EntityManager : public j1Module
{
public:

	j1EntityManager();
	~j1EntityManager();

	bool Awake(pugi::xml_node&);
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	j1Entity* EntityFactory(ENTITY_TYPE type, int x = 0, int y = 0);
	void CreateEntity(ENTITY_TYPE type, int x = 0, int y = 0);
	void AddEnemy(int x, int y, ENTITY_TYPE type);
	void SpawnEnemy(const EntityInfo& info);

	void OnCollision(Collider* c1, Collider* c2);

public:

	p2List<j1Entity*>	entityList;
	j1Player*			player = nullptr;
	j1Chest*			chest = nullptr;
	j1MovingObstacle*	obstacle = nullptr;
	j1Door*				door = nullptr;

private:

	EntityInfo			queue[MAX_ENTITIES];

};

#endif 
