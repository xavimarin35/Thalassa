#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Player.h"
#include "j1Scene1.h"
#include "j1Collisions.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Input.h"
#include "j1Audio.h"


j1Player::j1Player(int x, int y, ENTITY_TYPE type) : j1Entity(x, y, ENTITY_TYPE::PLAYER) 
{
	animation = NULL;

	// Loading all animations from xml
	idle.LoadAnimations("idle");
	jetpack.LoadAnimations("jetpack");
	godAnim.LoadAnimations("godmode");
	run.LoadAnimations("run");
	jump.LoadAnimations("jump");
	deathAnim.LoadAnimations("death");
}

j1Player::~j1Player() {}

bool j1Player::Start() {

	sprites = App->tex->Load("textures/Character_Spritesheet.png");

	// Load values from config.xml
	LoadInfo();

	collider = App->collisions->AddCollider({ (int)position.x, (int)position.y, hitbox.x, hitbox.y }, COLLIDER_PLAYER, App->entity_manager);

	return true;
}

bool j1Player::PreUpdate() {

	return true;
}

bool j1Player::Update(float dt) {

	if (playerCreated)
	{
		animation = &idle;

		if (godMode) 
		{
			jetpackActive = false;
			animation = &godAnim;
			if (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT) {
				position.x += godSpeed;
				flip = true;
			}

			if (App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_REPEAT) {
				position.x -= godSpeed;
				flip = false;
			}

			if (App->input->GetKey(SDL_SCANCODE_W) == j1KeyState::KEY_REPEAT) {
				position.y -= godSpeed;
			}

			if (App->input->GetKey(SDL_SCANCODE_S) == j1KeyState::KEY_REPEAT) {
				position.y += godSpeed;
			}
		}
		else if (!isDead)
		{
			if (playerCanMove) 
			{
				if (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT) {
					if (!ColRight)
					{
						position.x += speed.x;
						animation = &run;
						flip = true;
					}
				}

				if (App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_REPEAT) {
					if (!ColLeft)
					{
						position.x -= speed.x;
						animation = &run;
						flip = false;
					}
				}

				if (App->input->GetKey(SDL_SCANCODE_W) == j1KeyState::KEY_DOWN && doubleJump != 0) {
					isJumping = true;
					onFloor = false;
					jumpForce = jumpForce_xml;
					doubleJump -= 1;
					changedFloor = false;

					App->audio->PlayFx(App->audio->jumpFx);
				}

				if (App->input->GetKey(SDL_SCANCODE_SPACE) == j1KeyState::KEY_DOWN) {
					jetpackActive = true;
					onFloor = false;
					jetForce = jetForce_xml;
				}

				if (App->input->GetKey(SDL_SCANCODE_SPACE) == j1KeyState::KEY_UP) {
					jetpackActive = false;
					jumpForce = 0.0f;
					speed.y = 0.7f;
				}

			}

			if (isJumping) Jumping();

			if (jetpackActive) JetPack();

			if (onFloor)
				speed.y = 0;

			// Falling
			if (!isJumping && !godMode && !ColDown) 
			{
				if (!onFloor) {
					position.y += speed.y;
					speed.y += gravity;
					animation = &jump;
				}
			}
			
		}
		// Dead
		else {
			if (lifes > 0) {
				App->scene1->death = true;
			}
		}

		if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
		{
			godMode = !godMode;
			
			if (godMode) 
			{
				collider->type = COLLIDER_GOD;
			}
			else if(!godMode)
			{
				collider->type = COLLIDER_PLAYER;
			}
		}
	}

	if (collider != nullptr)
		collider->SetPos(position.x, position.y);

	BlitEntity(animation->GetCurrentFrame(), flip, position.x, position.y);

	return true;
}

bool j1Player::PostUpdate() {

	ColRight = false;
	ColLeft = false;
	ColDown = false;
	ColUp = false;
	onFloor = false;

	return true;
}

bool j1Player::Load(pugi::xml_node& data) {

	position.x = data.child("scene1").child("playerPos").attribute("x").as_float();
	position.y = data.child("sceen1").child("playerPos").attribute("y").as_float();

	return true;
}

bool j1Player::Save(pugi::xml_node& data) const {

	pugi::xml_node player = data.append_child("playerPos");

	player.append_attribute("x") = position.x;
	player.append_attribute("y") = position.y;

	return true;
}

bool j1Player::CleanUp() {
	LOG("Freeing the player");

	App->tex->UnLoad(sprites);

	if (collider != nullptr) {
		collider->to_delete = true;
		collider = nullptr;
	}

	return true;
}

void j1Player::OnCollision(Collider * c1, Collider * c2)
{
	if (c1->type == COLLIDER_PLAYER)
	{
		if (c2->type == COLLIDER_WALL)
		{
			// Right & Left Collisions
			if (c1->rect.y <= c2->rect.y + c2->rect.h && c1->rect.y + c1->rect.h - 5 >= c2->rect.y)
			{
				// right
				if (c1->rect.x + c1->rect.w >= c2->rect.x && c1->rect.x <= c2->rect.x)
				{
					ColRight = true;
					ColLeft = false;
				}
				// left
				else if (c1->rect.x <= c2->rect.x + c2->rect.w && c1->rect.x + c1->rect.w >= c2->rect.x + c2->rect.w)
				{
					ColLeft = true;
					ColRight = false;
				}
			}

			// Up & Down Collisions
			if (c1->rect.x + c1->rect.w >= c2->rect.x + 4 && c1->rect.x + 4 <= c2->rect.x + c2->rect.w)
			{
				// down
				if (c1->rect.y + c1->rect.h >= c2->rect.y && c1->rect.y < c2->rect.y) {

					onFloor = true;
					isJumping = false;
					jetpackActive = false;

					position.y = c2->rect.y - c1->rect.h + 1;

					speed.y = 0;
					doubleJump = 2;

					ColDown = true;
					ColUp = false;
					playerCanMove = true;
				}
				// up
				else if (c1->rect.y <= c2->rect.y + c2->rect.h && c1->rect.y > c2->rect.y) {
					
					onFloor = false;

					position.y = c2->rect.y + c2->rect.h;

					ColDown = false;
					ColUp = true;

					LOG("TOUCHING UP");
				}
			}
		}

		if (c2->type == COLLIDER_DEATH) {
			if (!playedFx) {
				App->audio->PlayFx(App->audio->deathFx);
				playedFx = true;
			}

			animation = &deathAnim;
			isDead = true;
		}

		if (c2->type == COLLIDER_OPENCHEST)
			openingChest = true;

		if (c2->type == COLLIDER_CHEST)
			itemPicked = true;

		
		if (c2->type == COLLIDER_WIN) {
			touchingWin = true;
			playerCanMove = false;
			c2->to_delete = true;
		}

		if (c2->type == COLLIDER_OPENDOOR) {
			doorOpened = true;
		}
	}
}

void j1Player::Jumping() {

	animation = &jump;

	if (!onFloor && !jetpackActive) {
		position.y -= jumpForce;
		jumpForce -= gravity;
	}
}

void j1Player::JetPack() {

	animation = &jetpack;

	if (!onFloor) {
		position.y -= jetForce;
	}
}

void j1Player::Die() {

	playedFx = false;
	isDead = false;
	playerCanMove = false;
	jetpackActive = false;
	App->scene1->death = false;

	position.y += speed.y;
	speed.y += gravity;

	CleanUp();
	Start();
}

void j1Player::LoadInfo()
{
	pugi::xml_document config_file;
	config_file.load_file("config.xml");

	pugi::xml_node config;
	config = config_file.child("config");

	pugi::xml_node nodePlayer;
	nodePlayer = config.child("player");

	if (App->scene1->tutorial_active) 
	{
		position.x = nodePlayer.child("posTuto").attribute("x").as_int();
		position.y = nodePlayer.child("posTuto").attribute("y").as_int();
	}
	else if (App->scene1->level1_active) 
	{
		if (App->scene1->midlevel_completed) 
		{
			position.x = nodePlayer.child("posLvl2").attribute("x").as_int();
			position.y = nodePlayer.child("posLvl2").attribute("y").as_int();
		}
		else 
		{
			position.x = nodePlayer.child("posLvl1").attribute("x").as_int();
			position.y = nodePlayer.child("posLvl1").attribute("y").as_int();
		}
	}
	else if (App->scene1->midlevel_active) 
	{
		position.x = nodePlayer.child("posMidLvl").attribute("x").as_int();
		position.y = nodePlayer.child("posMidLvl").attribute("y").as_int();
	}

	godSpeed = nodePlayer.child("godSpeed").attribute("value").as_float();
	speed.x = nodePlayer.child("speed").attribute("x").as_float();
	speed.y = nodePlayer.child("speed").attribute("y").as_float();
	gravity = nodePlayer.child("gravity").attribute("value").as_float();
	flip = nodePlayer.child("flip").attribute("value").as_bool();
	playerCreated = nodePlayer.child("created").attribute("value").as_bool();
	jetForce_xml = nodePlayer.child("jetForce").attribute("value").as_float();
	jumpForce_xml = nodePlayer.child("jumpForce").attribute("value").as_float();
	hitbox = { nodePlayer.child("hitbox").attribute("x").as_int(), nodePlayer.child("hitbox").attribute("y").as_int() };
}