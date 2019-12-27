#include "j1MainMenu.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Scene1.h"
#include "j1EntityManager.h"
#include "j1Player.h"
#include "j1Bat.h"
#include "j1Platform.h"
#include "j1TransitionsManager.h"
#include "j1Collisions.h"
#include "p2Animation.h"
#include "SDL_mixer/include/SDL_mixer.h"
#include "j1Pathfinding.h"
#include "SDL/include/SDL_mouse.h"
#include "j1Button.h"
#include "j1UIElement.h"
#include "j1Label.h"
#include "j1Box.h"
#include "j1Gui.h"

#include "Brofiler/Brofiler.h"

j1MainMenu::j1MainMenu() 
{
	name.create("mainMenu");
}

j1MainMenu::~j1MainMenu() {}

bool j1MainMenu::Awake(pugi::xml_node &)
{
	return true;
}

bool j1MainMenu::Start()
{
	gui_texture = App->tex->Load("gui/buttons.png");

	App->map->Load("MainMenu.tmx");

	SDL_Rect idle = { 0,0,117,32 };

	App->gui->CreateButton(&buttons_menu, BUTTON, 100, 100, idle, idle, idle, gui_texture, PLAY);

	return true;
}

bool j1MainMenu::Update(float dt)
{

	App->gui->UpdateButtonState(&buttons_menu);

	for (p2List_item<j1Button*>* item = buttons_menu.start; item != nullptr; item = item->next) {
		if (item->data->visible) {
			switch (item->data->state)
			{
			case IDLE:
				item->data->situation = item->data->idle;
				break;

			case HOVERED:
				item->data->situation = item->data->hovered;
				break;

			case RELEASED:
				item->data->situation = item->data->idle;
				if (item->data->bfunction == PLAY) {
					/*function button*/
				}
				else if (item->data->bfunction == LOAD_GAME) {
					/*function button*/
				}
				else if (item->data->bfunction == EXIT) {
					/*function button*/
				}
				else
					if ((item->data->bfunction == SETTINGS && !settings_window->visible)
						|| (item->data->bfunction == CLOSE_SETTINGS && settings_window->visible)) {
						settings_window->visible = !settings_window->visible;
						settings_window->position = App->gui->settingsPosition;

						for (p2List_item<j1Button*>* item = buttons_menu.start; item != nullptr; item = item->next) {
							if (item->data->parent == settings_window) {
								item->data->visible = !item->data->visible;
								item->data->position.x = settings_window->position.x + item->data->initialPosition.x;
								item->data->position.y = settings_window->position.y + item->data->initialPosition.y;
							}
						}
						for (p2List_item<j1Label*>* item = labels_menu.start; item != nullptr; item = item->next) {
							if (item->data->parent == settings_window) {
								item->data->visible = !item->data->visible;
								item->data->position.x = settings_window->position.x + item->data->initialPosition.x;
								item->data->position.y = settings_window->position.y + item->data->initialPosition.y;
							}
						}
						for (p2List_item<j1Box*>* item = boxes_menu.start; item != nullptr; item = item->next) {
							if (item->data->parent == settings_window) {
								item->data->visible = !item->data->visible;
								item->data->position.x = settings_window->position.x + item->data->initialPosition.x;
								item->data->position.y = settings_window->position.y + item->data->initialPosition.y;

								item->data->minimum = item->data->originalMinimum + settings_window->position.x;
								item->data->maximum = item->data->originalMaximum + settings_window->position.x;
							}
						}
					}
					else if (item->data->bfunction == OPEN_CREDITS) {
						/*function button*/
					}
				break;

			case CLICKED:
				item->data->situation = item->data->clicked;
				break;
			}
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_F7) == KEY_DOWN)
	{
		CleanUp();
		App->scene1->scene1_active = true;
		App->scene1->tutorial_active = true;
		App->scene1->Start();
		App->entity_manager->player->Start();
		App->entity_manager->bat->Start();
	}

	App->map->Draw(0);

	// ------------------------ Always blit GUI after the "App->map->Draw"
	// Blitting buttons and labels

	for (p2List_item<j1Button*>* item = buttons_menu.start; item != nullptr; item = item->next) {
		if (item->data->parent != nullptr) continue;
		item->data->Draw(App->gui->buttonsScale);
	}
	for (p2List_item<j1Label*>* item = labels_menu.start; item != nullptr; item = item->next) {
		if (item->data->parent != nullptr) continue;
		if (item->data->visible) item->data->Draw();
	}



	return true;
}

bool j1MainMenu::PostUpdate()
{
	return true;
}

bool j1MainMenu::CleanUp()
{
	App->tex->UnLoad(gui_texture);
	App->map->CleanUp();
	App->tex->CleanUp();

	for (p2List_item<j1Button*>* item = buttons_menu.start; item != nullptr; item = item->next) {
		item->data->CleanUp();
		buttons_menu.del(item);
	}

	for (p2List_item<j1Label*>* item = labels_menu.start; item != nullptr; item = item->next) {
		item->data->CleanUp();
		labels_menu.del(item);
	}

	for (p2List_item<j1Box*>* item = boxes_menu.start; item != nullptr; item = item->next) {
		item->data->CleanUp();
		boxes_menu.del(item);
	}


	return true;
}

void j1MainMenu::ChangeScene(SCENE_TYPE next)
{
	if (!startScene1)
	{
		this->active = false;
		startGame = false;
		startCredits = false;

		CleanUp();
		if (next == SCENE_TYPE::CREDITS)
		{
			/*App->credits->active = true;
			App->credits->Start();*/
		}
		else {
			if (next == SCENE_TYPE::SCENE)
			{
				App->scene1->active = true;
				App->scene1->Start();
			}

			App->entity_manager->active = true;
		}
	}
	
	
}
