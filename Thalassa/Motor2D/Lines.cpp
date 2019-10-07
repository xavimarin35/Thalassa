#include "j1Render.h"
#include "Lines.h"
#include "j1Scene1.h"
#include "j1App.h"
#include "j1Window.h"

Lines::Lines(j1Color color, float time) : j1Transitions(time) {
	this->color = color;

	App->win->GetWindowSize(w, h);
	screen = { -(int)w, 0, (int)w, (int)h };
	SDL_SetRenderDrawBlendMode(App->render->renderer, SDL_BLENDMODE_BLEND);

	initial_x_left = App->render->camera.x;
	initial_x_right = App->render->camera.x + (int)w;


	for (int i = 0; i < 11; i++) {
		// All lines have window width as width and height/10 as height
		lines[i].h = ((int)h / 10);

		lines[i].w = (int)w;

		// 5 lines are placed at the left of the screen
		if (i % 2 == 0)
			lines[i].x = initial_x_left;

		// 5 lines are placed at the right of the screen
		else
			lines[i].x = initial_x_right;
	}

	// Each one is placed h += h/10 than the previous one
	for (int i = 0; i < 11; i++) {
		lines[i].y = height;
		height += h / 10;
	}

}

Lines::~Lines()
{}

void Lines::Start() {

	j1Transitions::Start();

	for (int i = 0; i < 11; i++) {
		if (i % 2 == 0)
			lines[i].x = Interpolation(-(int)w, 0, percentage);
		else
			lines[i].x = Interpolation((int)w, 0, percentage);
	}

	SDL_SetRenderDrawColor(App->render->renderer, color.r, color.g, color.b, 255);
	for (int i = 0; i < 11; i++)
		SDL_RenderFillRect(App->render->renderer, &lines[i]);
}

void Lines::Change() {

	SDL_SetRenderDrawColor(App->render->renderer, color.r, color.g, color.b, 255);
	SDL_RenderFillRect(App->render->renderer, &screen);

	j1Transitions::Change();
}

void Lines::Exit() {

	for (int i = 0; i < 11; i++) {
		if (i % 2 == 0)
			lines[i].x = Interpolation(0, (int)w, percentage);
		else
			lines[i].x = Interpolation(0, -(int)w, percentage);
	}

	SDL_SetRenderDrawColor(App->render->renderer, color.r, color.g, color.b, 255);
	for (int i = 0; i < 11; i++)
		SDL_RenderFillRect(App->render->renderer, &lines[i]);

	j1Transitions::Exit();
}