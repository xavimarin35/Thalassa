#include "j1Scene1.h"
#include "j1TransitionsManager.h"
#include "j1Transitions.h"
#include "j1Entity.h"
#include "j1EntityManager.h"
#include "j1Player.h"
#include "j1Render.h"
#include "FadeToColor.h"
#include "j1App.h"
#include "j1Window.h"
#include "p2Log.h"

// Similar code to FadeToBlack, but now we can control the color easily

FadeToColor::FadeToColor(j1Color color, float time) : j1Transitions(time) {

	this->color = color;

	uint w, h;
	App->win->GetWindowSize(w, h);
	screen = { 0, 0, (int)w, (int)h };
	SDL_SetRenderDrawBlendMode(App->render->renderer, SDL_BLENDMODE_BLEND);

}

FadeToColor::~FadeToColor()
{}

void FadeToColor::Start() {

	j1Transitions::Start();

	// Filling the screen with a color
	FillScreen(Interpolation(0, 255, percentage));
}

void FadeToColor::Change() {

	// FillScreen(255.0f);

	if (App->scene1->death) {
		App->entity_manager->player->Die();
	}

	j1Transitions::Change();
}

void FadeToColor::Exit() {

	// Color disappears
	FillScreen(Interpolation(255, 0, percentage));

	j1Transitions::Exit();
}

void FadeToColor::FillScreen(float percentage) {

	if (percentage < 0)
		percentage = 0;

	if (percentage > 255)
		percentage = 255;

	SDL_SetRenderDrawColor(App->render->renderer, color.r, color.g, color.b, percentage);
	SDL_RenderFillRect(App->render->renderer, &screen);
}