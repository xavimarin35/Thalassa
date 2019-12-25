#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1Gui.h"
#include "j1Audio.h"
#include "j1Window.h"
#include "j1Scene1.h"
#include "j1Fonts.h"

#include "Brofiler/Brofiler.h"

j1Gui::j1Gui() : j1Module()
{
	name.create("gui");
}

j1Gui::~j1Gui() {}

bool j1Gui::Awake(pugi::xml_node& config)
{
	bool ret = true;

	return ret;
}

bool j1Gui::Start()
{
	gui_texture = App->tex->Load(gui_texture_name.data());

	font1 = App->font->Load("fonts/Pixeled.ttf", 5);

	return true;
}

bool j1Gui::PreUpdate()
{
	if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
		debug != debug;

	return true;
}

bool j1Gui::PostUpdate()
{
	return true;
}

bool j1Gui::CleanUp()
{
	App->tex->UnLoad(gui_texture);

	return true;
}

const SDL_Texture* j1Gui::GetSprites() const
{
	return gui_texture;
}