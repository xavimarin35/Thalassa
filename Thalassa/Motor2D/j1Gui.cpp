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
#include "j1MainMenu.h"

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
	if (App->scene1->settings_window != nullptr && App->scene1->settings_window->visible == true)
		App->scene1->settings_window->Draw(App->gui->settingsWindowScale);


	for (p2List_item<j1Button*>* item = App->scene1->scene1Buttons.start; item != nullptr; item = item->next) {
		if (item->data->parent == nullptr) continue;

		if (item->data->parent->visible == false)
			item->data->visible = false;
		else
			item->data->Draw(App->gui->buttonsScale);
	}
	for (p2List_item<j1Label*>* item = App->scene1->scene1Labels.start; item != nullptr; item = item->next) {
		if (item->data->parent == nullptr) continue;

		if (item->data->parent->visible == false)
			item->data->visible = false;
		else {
			if (item->data->text != "Settings" && item->data->text != "Save"  && item->data->text != "Quit")
				item->data->Draw(App->gui->buttonsScale);
			else
				item->data->Draw();
		}
	}
	for (p2List_item<j1Box*>* item = App->scene1->scene1Boxes.start; item != nullptr; item = item->next) {
		if (item->data->parent == nullptr) continue;

		if (item->data->parent->visible == false)
			item->data->visible = false;
		else
			item->data->Draw(App->gui->buttonsScale);
	}

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
		buttons->add(ret);
	}

	return ret;
}

j1Box* j1Gui::CreateBox(p2List<j1Box*>* boxes, UI_ELEMENTS type, int x, int y, SDL_Rect section, SDL_Texture* texture, j1UIElement* parent, uint minimum, uint maximum)
{
	j1Box* ret = nullptr;

	ret = new j1Box(type, x, y, section, texture, parent, minimum, maximum);

	if (ret != nullptr)
	{
		boxes->add(ret);
	}

	return ret;
}

void j1Gui::UpdateButtonState(p2List<j1Button*>* buttons)
{
	int x, y; App->input->GetMousePosition(x, y);

	for (p2List_item<j1Button*>* button = buttons->start; button != nullptr; button = button->next) {

		if (button->data->visible == false || button->data->bfunction == NO_FUNCTION) continue;

		if ((x - (App->render->camera.x / (int)App->win->scale)) <= button->data->position.x + button->data->situation.w
			&& ((x - (App->render->camera.x / (int)App->win->scale)) >= button->data->position.x)
			&& ((y - (App->render->camera.y / (int)App->win->scale)) <= button->data->position.y + button->data->situation.h)
			&& ((y - (App->render->camera.y / (int)App->win->scale)) >= button->data->position.y)) {

			//if (/*App->credits->active == false && */App->mainmenu->settings_window != nullptr && App->mainmenu->settings_window->visible
			//	&& button->data->bfunction != CLOSE_SETTINGS) continue;

			button->data->state = STATE::HOVERED;
			if (!button->data->hoverPlayed) {
				App->audio->PlayFx(App->audio->hoverSound);
				button->data->hoverPlayed = true;
			}

			if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
				button->data->state = STATE::CLICKED;
				if (!button->data->clickPlayed) {
					App->audio->PlayFx(App->audio->clickSound);
					button->data->clickPlayed = true;
				}
			}
			if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {
				button->data->state = STATE::RELEASED;
				button->data->clickPlayed = false;
			}
		}
		else {
			button->data->state = STATE::IDLE;
			button->data->hoverPlayed = false;
		}
	}
}

void j1Gui::UpdateWindow(j1Box* window, p2List<j1Button*>* buttons, p2List<j1Label*>* labels, p2List<j1Box*>* boxes)
{
	int x, y; App->input->GetMousePosition(x, y);

	if ((window != nullptr && window->visible == true)
		&& (x - (App->render->camera.x / (int)(App->win->scale)) <= window->position.x + window->section.w)
		&& (x - (App->render->camera.x / (int)(App->win->scale)) >= window->position.x)
		&& (y - (App->render->camera.y / (int)(App->win->scale)) <= window->position.y + window->section.h)
		&& (y - (App->render->camera.y / (int)(App->win->scale)) >= window->position.y))
	{
		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
			window->clicked = true;

		else if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
			window->clicked = false;
	}

	if (window != nullptr)
	{
		if (buttons != nullptr)
		{
			for (p2List_item<j1Button*>* item = buttons->start; item != nullptr; item->next)
			{
				if (item->data->state == CLICKED && item->data->parent == window)
					window->clicked = false;
			}
		}		

		if (window->clicked)
		{
			int x, y; App->input->GetMousePosition(x, y);

			if (window->distanceCalculated == false) {
				window->mouseDistance = { x - window->position.x, y - window->position.y };
				window->distanceCalculated = true;
			}

			// Updating the positions of the window and its elements
			window->position = { x - window->mouseDistance.x, y - window->mouseDistance.y };

			if (buttons != nullptr) {
				for (p2List_item<j1Button*>* item = buttons->start; item != nullptr; ++item) {
					if (item->data->parent == window) {
						item->data->position.x = window->position.x + item->data->initialPosition.x;
						item->data->position.y = window->position.y + item->data->initialPosition.y;
					}
				}
			}

			if (labels != nullptr) {
				for (p2List_item<j1Label*>* item = labels->start; item != nullptr; ++item) {
					if (item->data->parent == window) {
						item->data->position.x = window->position.x + item->data->initialPosition.x;
						item->data->position.y = window->position.y + item->data->initialPosition.y;
					}
				}
			}
		}
		else
			window->distanceCalculated = false;
	}
}