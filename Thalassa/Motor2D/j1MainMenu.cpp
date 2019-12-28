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
	LoadConfig();

	gui_texture = App->tex->Load("gui/buttons.png");

	App->map->Load("MainMenu.tmx");

	SDL_Rect idle1 = { button1_idle.x, button1_idle.y, button1_idle.w, button1_idle.h };
	SDL_Rect hover1 = { button1_hover.x, button1_hover.y, button1_hover.w, button1_hover.h };
	SDL_Rect click1 = { button1_click.x, button1_click.y, button1_click.w, button1_click.h };

	SDL_Rect idle2 = { button2_idle.x, button2_idle.y, button2_idle.w, button2_idle.h };
	SDL_Rect hover2 = { button2_hover.x, button2_hover.y, button2_hover.w, button2_hover.h };
	SDL_Rect click2 = { button2_click.x, button2_click.y, button2_click.w, button2_click.h };

	SDL_Rect idle3 = { button3_idle.x, button3_idle.y, button3_idle.w, button3_idle.h };
	SDL_Rect hover3 = { button3_hover.x, button3_hover.y, button3_hover.w, button3_hover.h };
	SDL_Rect click3 = { button3_click.x, button3_click.y, button3_click.w, button3_click.h };

	App->gui->CreateButton(&buttons_menu, BUTTON, 40, 83, idle1, hover1, click1, gui_texture, PLAY);
	App->gui->CreateButton(&buttons_menu, BUTTON, 70, 130, idle2, hover2, click2, gui_texture, CONTINUE_LOADED);
	App->gui->CreateButton(&buttons_menu, BUTTON, 30, 180, idle3, hover3, click3, gui_texture, EXIT);

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
		LoadScene1();
	}

	App->map->Draw(0);

	// Always blit GUI after the "App->map->Draw"
	// Blitting buttons and labels

	for (p2List_item<j1Button*>* item = buttons_menu.start; item != nullptr; item = item->next) 
	{
		if (item->data->parent != nullptr) continue;
		item->data->Draw(App->gui->buttonsScale);
	}

	for (p2List_item<j1Label*>* item = labels_menu.start; item != nullptr; item = item->next) 
	{
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

void j1MainMenu::LoadConfig()
{
	pugi::xml_document config_file;
	config_file.load_file("config.xml");

	pugi::xml_node config;
	config = config_file.child("config");

	pugi::xml_node nodeButtons;
	nodeButtons = config.child("buttons");

	button1_idle = { nodeButtons.child("button1_idle").attribute("x").as_int(),nodeButtons.child("button1_idle").attribute("y").as_int(), nodeButtons.child("button1_idle").attribute("w").as_int(), nodeButtons.child("button1_idle").attribute("h").as_int() };
	button1_hover = { nodeButtons.child("button1_hover").attribute("x").as_int(),nodeButtons.child("button1_hover").attribute("y").as_int(), nodeButtons.child("button1_hover").attribute("w").as_int(), nodeButtons.child("button1_hover").attribute("h").as_int() };
	button1_click = { nodeButtons.child("button1_click").attribute("x").as_int(),nodeButtons.child("button1_click").attribute("y").as_int(), nodeButtons.child("button1_click").attribute("w").as_int(), nodeButtons.child("button1_click").attribute("h").as_int() };

	button2_idle = { nodeButtons.child("button2_idle").attribute("x").as_int(),nodeButtons.child("button2_idle").attribute("y").as_int(), nodeButtons.child("button2_idle").attribute("w").as_int(), nodeButtons.child("button2_idle").attribute("h").as_int() };
	button2_hover = { nodeButtons.child("button2_hover").attribute("x").as_int(),nodeButtons.child("button2_hover").attribute("y").as_int(), nodeButtons.child("button2_hover").attribute("w").as_int(), nodeButtons.child("button2_hover").attribute("h").as_int() };
	button2_click = { nodeButtons.child("button2_click").attribute("x").as_int(),nodeButtons.child("button2_click").attribute("y").as_int(), nodeButtons.child("button2_click").attribute("w").as_int(), nodeButtons.child("button2_click").attribute("h").as_int() };

	button3_idle = { nodeButtons.child("button3_idle").attribute("x").as_int(),nodeButtons.child("button3_idle").attribute("y").as_int(), nodeButtons.child("button3_idle").attribute("w").as_int(), nodeButtons.child("button3_idle").attribute("h").as_int() };
	button3_hover = { nodeButtons.child("button3_hover").attribute("x").as_int(),nodeButtons.child("button3_hover").attribute("y").as_int(), nodeButtons.child("button3_hover").attribute("w").as_int(), nodeButtons.child("button3_hover").attribute("h").as_int() };
	button3_click = { nodeButtons.child("button3_click").attribute("x").as_int(),nodeButtons.child("button3_click").attribute("y").as_int(), nodeButtons.child("button3_click").attribute("w").as_int(), nodeButtons.child("button3_click").attribute("h").as_int() };
}

void j1MainMenu::LoadScene1()
{
	CleanUp();
	App->scene1->scene1_active = true;
	App->scene1->tutorial_active = true;
	App->scene1->Start();
	App->particles->Start();
	App->entity_manager->Start();
	App->entity_manager->player->Start();
	App->entity_manager->bat->Start();
}