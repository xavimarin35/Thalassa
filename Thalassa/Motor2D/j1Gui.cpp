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
#include "j1Button.h"
#include "j1Label.h"
#include "j1Box.h"

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
	gui_texture = App->tex->Load("gui/buttons.png");

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

j1Button* j1Gui::CreateButton(p2List<j1Button*>* buttons, UI_ELEMENTS type, int x, int y, SDL_Rect idle, SDL_Rect hovered, SDL_Rect clicked, SDL_Texture* text, BUTTON_FUNCTION function, j1UIElement* parent)
{
	j1Button* ret = nullptr;

	ret = new j1Button(type, x, y, idle, hovered, clicked, text, function, parent);

	if (ret != nullptr)
	{
		//buttons->add(ret);
	}

	return ret;
}

void j1Gui::UpdateButtonState(p2List<j1Button*>* buttons, float scale)
{
	int x, y; App->input->GetMousePosition(x, y);

	for (p2List_item<j1Button*>* item = buttons->start; item != buttons->end; ++item)
	{
		if ((*item).data->visible == false || (*item).data->bfunction == NO_FUNCTION) continue;

		if (x - (App->render->camera.x / (int)(App->win->GetScale())) <= (*item).data->position.x + (*item).data->situation.w * scale
			&& x - (App->render->camera.x / (int)(App->win->GetScale())) >= (*item).data->position.x
			&& y - (App->render->camera.y / (int)(App->win->GetScale())) <= (*item).data->position.y + (*item).data->situation.h * scale
			&& y - (App->render->camera.y / (int)(App->win->GetScale())) >= (*item).data->position.y)
		{
			(*item).data->state = STATE::HOVERED;

			if (!(*item).data->hoverPlayed)
			{
				App->audio->PlayFx(App->audio->hoverSound);
				(*item).data->hoverPlayed = true;
			}

			if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
				(*item).data->state = STATE::CLICKED;
				if (!(*item).data->clickPlayed) {
					App->audio->PlayFx(App->audio->clickSound);
					(*item).data->clickPlayed = true;
				}
			}
			if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {
				(*item).data->state = STATE::RELEASED;
				(*item).data->clickPlayed = false;
			}
		}

		else {
			(*item).data->state = STATE::IDLE;
			(*item).data->hoverPlayed = false;
		}
	}
}