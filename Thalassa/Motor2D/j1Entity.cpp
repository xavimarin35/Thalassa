#include "j1Entity.h"

j1Entity::j1Entity(int x, int y, ENTITY_TYPE type)
{
}

bool j1Entity::Start()
{
	return true;
}

bool j1Entity::Update(float dt)
{
	return true;
}

bool j1Entity::PostUpdate()
{
	return true;
}

bool j1Entity::PreUpdate()
{
	return true;
}

bool j1Entity::CleanUp()
{
	return true;
}

bool j1Entity::Load(pugi::xml_node &)
{
	return true;
}

bool j1Entity::Save(pugi::xml_node &)
{
	return true;
}