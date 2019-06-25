#include "j1Entity.h"
#include "j1Render.h"
#include "j1App.h"

j1Entity::j1Entity(int x, int y, ENTITY_TYPE type) : position(x,y), type(type)
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

void j1Entity::BlitEntity(int x, int y, SDL_Rect r, bool flip)
{
	if (flip == false)
		App->render->Blit(sprites, (int)position.x + x, (int)position.y + y, &r, SDL_FLIP_NONE);
	else
		App->render->Blit(sprites, (int)position.x + x, (int)position.y + y, &r, SDL_FLIP_HORIZONTAL);
}
