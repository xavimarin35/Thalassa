#include "j1EntityManager.h"
#include "j1Player.h"
#include "j1Chest.h"
#include "j1MovingObstacle.h"
#include "j1Door.h"

j1EntityManager::j1EntityManager()
{
}

j1EntityManager::~j1EntityManager()
{
}

bool j1EntityManager::Awake(pugi::xml_node &)
{
	return true;
}

bool j1EntityManager::Start()
{
	for (p2List_item<j1Entity*>* it = entityList.start; it != nullptr; it = it->next)
	{
		it->data->Start();
	}

	return true;
}

bool j1EntityManager::PreUpdate()
{
	for (int i = 0; i < MAX_ENTITIES; ++i)
	{
		if (queue[i].type != ENTITY_TYPE::NONE) 
		{
			SpawnEnemy(queue[i]);
			queue[i].type = ENTITY_TYPE::NONE;
		}
	}

	return true;
}

bool j1EntityManager::Update(float dt)
{
	for (p2List_item<j1Entity*>* it = entityList.start; it != nullptr; it = it->next)
	{
		it->data->Update(dt);
	}
	return true;
}

bool j1EntityManager::PostUpdate()
{
	for (p2List_item<j1Entity*>* it = entityList.start; it != nullptr; it = it->next)
	{
		it->data->PostUpdate();
	}

	return true;
}

bool j1EntityManager::CleanUp()
{
	bool ret = true;

	for (p2List_item<j1Entity*>* it = entityList.end; it != NULL; it = it->prev)
	{
		ret = it->data->CleanUp();
	}

	entityList.clear();
	player = nullptr;

	return ret;
}

void j1EntityManager::OnCollision(Collider * c1, Collider * c2)
{
	for (p2List_item<j1Entity*>* it = entityList.start; it != nullptr; it = it->next) 
	{
		if (it->data->collider == c1)
		{
			it->data->OnCollision(c1, c2);
			it->data->OnCollision(c2, c1);
			break;
		}
	}
}

j1Entity* j1EntityManager::EntityFactory(ENTITY_TYPE type, int x, int y)
{
	j1Entity* ret = nullptr;

	switch (type) 
	{
	case ENTITY_TYPE::PLAYER:
		ret = new j1Player(x, y, type);

		if (ret != nullptr)
			entityList.add(ret);
		break;

	case ENTITY_TYPE::CHEST:
		ret = new j1Chest(x, y, type);

		if (ret != nullptr)
			entityList.add(ret);
		break;

	case ENTITY_TYPE::DOOR:
		ret = new j1Door(x, y, type);

		if (ret != nullptr)
			entityList.add(ret);
		break;
	}

	return ret;
}

void j1EntityManager::CreateEntity(ENTITY_TYPE type, int x, int y)
{
	switch (type) {
		case ENTITY_TYPE::PLAYER:
			player = (j1Player*)EntityFactory(ENTITY_TYPE::PLAYER, x, y);
			break;

		case ENTITY_TYPE::CHEST:
			chest = (j1Chest*)EntityFactory(ENTITY_TYPE::CHEST, x, y);
			break;

		case ENTITY_TYPE::DOOR:
			door = (j1Door*)EntityFactory(ENTITY_TYPE::DOOR, x, y);
			break;

	}
}

void j1EntityManager::AddEnemy(int x, int y, ENTITY_TYPE type)
{
	for (int i = 0; i < MAX_ENTITIES; ++i)
	{
		if (queue[i].type == ENTITY_TYPE::NONE)
		{
			queue[i].type = type;
			queue[i].position.x = x;
			queue[i].position.y = y;
			break;
		}
	}
}

void j1EntityManager::SpawnEnemy(const EntityInfo & info)
{
	for (uint i = 0; i < MAX_ENTITIES; ++i)
	{
		if (queue[i].type != ENTITY_TYPE::NONE)
		{
			j1Entity* ret;

			if (queue[i].type == ENTITY_TYPE::OBSTACLE)
				ret = new j1MovingObstacle(info.position.x, info.position.y, info.type);

			entityList.add(ret);
			ret->Start();

			break;
		}
	}
}

bool j1EntityManager::Load(pugi::xml_node &)
{
	return true;
}

bool j1EntityManager::Save(pugi::xml_node &) const
{
	return true;
}
