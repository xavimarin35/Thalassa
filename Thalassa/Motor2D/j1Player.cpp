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
#include "j1Particle.h"
#include "j1Window.h"
#include "SDL/include/SDL.h"
#include "p2Point.h"
#include "j1Demon.h"
#include "j1Transitions.h"
#include "j1TransitionsManager.h"

#include "Brofiler/Brofiler.h"

#define SDL_TICKS_PASSED(A, B)  ((Sint32)((B) - (A)) <= 0)

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

bool j1Player::PreUpdate() 
{
	BROFILER_CATEGORY("Player_PreUpdate", Profiler::Color::Salmon)

	return true;
}

bool j1Player::Update(float dt) 
{
	BROFILER_CATEGORY("Player_Update", Profiler::Color::Indigo)

	if (playerCreated)
	{
		if (godMode) 
		{
			jetpackActive = false;
			animation = &godAnim;
			GodMode(dt);
		}

		else if (!isDead)
		{
			animation = &idle;

			if(lifePoints <= 0)
				isDead = true;

			if (playerCanMove)
			{
				PlayerMovement(dt);
			}
			
			if (isJumping) Jumping();

			if (jetpackActive) JetPack();

			if (onFloor)
				speed.y = 0;

			// Falling
			if (!isJumping && !godMode && !ColDown)
			{
				if (!onFloor && App->scene1->scene_timer.Read() > 2400) {
					position.y += speed.y * dt;
					speed.y += gravity * dt;
					animation = &jump;
				}
			}			
		}

		// Dead
		if (isDead)
		{
			animation = &deathAnim;

			if (!playedFx)
				App->audio->PlayFx(App->audio->deathFx);
			playedFx = true;

			position.y += 17.0f * dt;

			App->scene1->death = true;
			isJumping = true;

			if (collider != nullptr)
				collider->to_delete = true;
			collider = nullptr;
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

	BlitEntity(animation->GetCurrentFrame(dt), flip, position.x, position.y);

	return true;
}

bool j1Player::PostUpdate() 
{
	BROFILER_CATEGORY("Player_PostUpdate", Profiler::Color::Orange)

	ColRight = ColLeft = ColDown = ColUp = onFloor = false;

	return true;
}

bool j1Player::Load(pugi::xml_node& data) {

	position.x = data.child("scene1").child("playerPos").attribute("x").as_float();
	position.y = data.child("scene1").child("playerPos").attribute("y").as_float();

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

	if (collider != nullptr) 
	{
		collider->to_delete = true;
		collider = nullptr;
	}

	return true;
}

void j1Player::OnCollision(Collider * c1, Collider * c2)
{
	BROFILER_CATEGORY("PlayerOnCollision", Profiler::Color::PaleVioletRed)

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

		if (c2->type == COLLIDER_DEATH || c2->type == COLLIDER_ENEMY_SHOT || c2->type == COLLIDER_ENEMY)
		{
			isDead = true;
		}
			   		
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

void j1Player::PlayerMovement(float dt)
{
	BROFILER_CATEGORY("PlayerMovement", Profiler::Color::Lavender)

	if (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT) {
		if (!ColRight)
		{
			position.x += speed.x * dt;
			animation = &run;
			flip = true;
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_REPEAT) {
		if (!ColLeft)
		{
			position.x -= speed.x * dt;
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

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == j1KeyState::KEY_DOWN) 
	{
		jetpackActive = true;
		playedFx2 = false;
		onFloor = false;
		jetForce = jetForce_xml;
		if (!playedFx2) {
			App->audio->PlayFx(App->audio->jetpackFx);
			playedFx2 = true;
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == j1KeyState::KEY_UP) 
	{
		jetpackActive = false;
		jumpForce = 0.0f;
		speed.y = speed_y;
	}

	if (App->input->GetMouseButtonDown(3) == KEY_DOWN)
	{
		ChangeWeapon();
	}

	if (App->input->GetMouseButtonDown(1) == KEY_DOWN)
	{
		iPoint mouse_pos;
		App->input->GetMousePosition(mouse_pos.x, mouse_pos.y);
		Shooting(mouse_pos.x, mouse_pos.y, dt);
	}

	if (jetPackLife < 196)
		jetPackLife += 0.025;
	else if (jetPackLife >= 196)
		jetPackLife = 196;

	if (jetPackLife < 0)
		jetpackActive = false;
}

void j1Player::Jumping() {

	animation = &jump;

	if (!onFloor && !jetpackActive) {
		position.y -= jumpForce * App->GetDT();
		jumpForce -= gravity * App->GetDT();
	}
}

void j1Player::JetPack() {

	animation = &jetpack;

	jetPackLife -= 3;

	if (!onFloor) {
		position.y -= jetForce * App->GetDT();
	}
}

void j1Player::Die() 
{
	App->scene1->done_anim = false;
	playedFx = false;
	isDead = false;
	playerCanMove = false;
	jetpackActive = false;
	App->scene1->death = false;
	App->entity_manager->DestroyAllEntities();
	App->scene1->ReSpawnEntities();

	position.y += speed.y * App->GetDT();
	speed.y += gravity * App->GetDT();
	
	/*CleanUp();*/
	Start();
	
}

void j1Player::Shooting(float x, float y, float dt)
{
	BROFILER_CATEGORY("PlayerShot", Profiler::Color::Purple)

	fPoint margin;
	margin = margin_particles;

	fPoint edge;
	edge.x = x - (position.x + margin.x) - (App->render->camera.x / (int)App->win->scale);
	edge.y = (position.y + margin.y) - y + (App->render->camera.y / (int)App->win->scale);

	double angle = -(atan2(edge.y, edge.x));

	fPoint speed_particle;
	fPoint p_speed = speed_particles;

	speed_particle.x = p_speed.x * cos(angle);
	speed_particle.y = p_speed.y * sin(angle);

	double angleInDeg = angle * 180 / PI;

	switch (currentType) 
	{
	case PARTICLE_TYPE::BASIC_SHOOT:
		App->particles->basicShoot.speed = { speed_particle.x * App->GetDT(), speed_particle.y * App->GetDT() };
		App->particles->AddParticle(App->particles->basicShoot, position.x + margin.x, position.y + margin.y, dt, COLLIDER_SHOT, 0, angleInDeg);
		App->audio->PlayFx(App->audio->shotFx);
		break;
	case PARTICLE_TYPE::REMOTE_SHOOT:
		App->particles->remoteShoot.speed = { speed_particle.x * App->GetDT(), speed_particle.y * App->GetDT() };
		App->particles->AddParticle(App->particles->remoteShoot, position.x + margin.x, position.y + margin.y, dt, COLLIDER_SHOT, 0, angleInDeg);
		App->audio->PlayFx(App->audio->shotFx);
		break;
	}

}

void j1Player::ChangeWeapon()
{
	if (currentType == PARTICLE_TYPE::BASIC_SHOOT)
	{
		currentType = PARTICLE_TYPE::REMOTE_SHOOT;
	}
	else if (currentType == PARTICLE_TYPE::REMOTE_SHOOT)
	{
		currentType = PARTICLE_TYPE::BASIC_SHOOT;
	}
}

void j1Player::GodMode(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT) {
		position.x += godSpeed * dt;
		flip = true;
	}

	if (App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_REPEAT) {
		position.x -= godSpeed * dt;
		flip = false;
	}

	if (App->input->GetKey(SDL_SCANCODE_W) == j1KeyState::KEY_REPEAT) {
		position.y -= godSpeed * dt;
	}

	if (App->input->GetKey(SDL_SCANCODE_S) == j1KeyState::KEY_REPEAT) {
		position.y += godSpeed * dt;
	}
}

void j1Player::LoadInfo()
{
	BROFILER_CATEGORY("LoadPlayerInfo", Profiler::Color::DarkBlue)

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
	speed_y = nodePlayer.child("speed").attribute("y").as_float();
	gravity = nodePlayer.child("gravity").attribute("value").as_float();
	flip = nodePlayer.child("flip").attribute("value").as_bool();
	playerCreated = nodePlayer.child("created").attribute("value").as_bool();
	jetForce_xml = nodePlayer.child("jetForce").attribute("value").as_float();
	jumpForce_xml = nodePlayer.child("jumpForce").attribute("value").as_float();
	hitbox = { nodePlayer.child("hitbox").attribute("x").as_int(), nodePlayer.child("hitbox").attribute("y").as_int() };
	lifePoints = nodePlayer.child("life").attribute("value").as_int();

	pugi::xml_node nodeParticles;
	nodeParticles = config.child("particles");

	margin_particles = { nodeParticles.child("margin1").attribute("x").as_float(),nodeParticles.child("margin1").attribute("y").as_float() };
	speed_particles = { nodeParticles.child("p_speed1").attribute("x").as_float(),nodeParticles.child("p_speed1").attribute("y").as_float() };
}
