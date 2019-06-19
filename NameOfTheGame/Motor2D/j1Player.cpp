#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Player.h"

j1Player::j1Player() {}

j1Player::~j1Player() {}

// Load assets
bool j1Player::Start() {
	return true;
}

//Call modules before each loop iteration
bool j1Player::PreUpdate() {

	return true;
}

// Call modules on each loop iteration
bool j1Player::Update() {

	return true;
}

// Call modules after each loop iteration
bool j1Player::PostUpdate() {

	return true;
}

// Load game state
bool j1Player::Load(pugi::xml_node& data) {

	position.x = data.child("position").attribute("x").as_int();
	position.y = data.child("position").attribute("y").as_int();

	return true;
}

// Save game state
bool j1Player::Save(pugi::xml_node& data) const {

	pugi::xml_node player = data.append_child("position");

	player.append_attribute("x") = position.x;
	player.append_attribute("y") = position.y;

	return true;
}

// Called before quitting
bool j1Player::CleanUp() {
	LOG("Freeing the player");

	return true;
}