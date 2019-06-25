#include "j1EntityManager.h"
#include "j1Player.h"

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
	return true;
}

bool j1EntityManager::PreUpdate()
{
	return true;
}

bool j1EntityManager::Update(float dt)
{
	return true;
}

bool j1EntityManager::PostUpdate()
{
	return true;
}

bool j1EntityManager::CleanUp()
{
	entityList.clear();
	player = nullptr;

	return true;
}

void j1EntityManager::OnCollision(Collider * c1, Collider * c2)
{

}

j1Entity* j1EntityManager::EntityFactory(ENTITY_TYPE type, int x, int y)
{
	j1Entity* ret = nullptr;

	switch (type) 
	{
	case PLAYER:
		ret = new j1Player(x, y, type);

		if (ret != nullptr)
			entityList.add(ret);
		break;
	}

	return ret;
}

void j1EntityManager::CreateEntity(ENTITY_TYPE type)
{
	switch (type) {
		case PLAYER:
			player = (j1Player*)EntityFactory(PLAYER);
		break;
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
