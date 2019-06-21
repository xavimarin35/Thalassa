#include "j1EntityManager.h"

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
	return true;
}

bool j1EntityManager::Load(pugi::xml_node &)
{
	return true;
}

bool j1EntityManager::Save(pugi::xml_node &) const
{
	return true;
}

j1Entity* j1EntityManager::EntityFactory(ENTITY_TYPE type, int x, int y)
{
	return nullptr;
}

void j1EntityManager::OnCollision(Collider * c1, Collider * c2)
{
}

void j1EntityManager::CreatePlayer()
{
}

void j1EntityManager::DestroyEntities()
{
}
