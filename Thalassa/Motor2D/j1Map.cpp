#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Player.h"
#include "j1Collisions.h"
#include "j1Scene1.h"
#include <math.h>

j1Map::j1Map() : j1Module(), map_loaded(false)
{
	name.create("map");
}

// Destructor
j1Map::~j1Map()
{}

// Called before render is available
bool j1Map::Awake(pugi::xml_node& config)
{
	LOG("Loading Map Parser");
	bool ret = true;

	folder.create(config.child("folder").child_value());

	return ret;
}

void j1Map::Draw()
{
	if (map_loaded == false)
		return;

	p2List_item<MapLayer*>* layer;

	int tile_num;
	for (layer = data.layers.start; layer != nullptr; layer = layer->next)
	{
		tile_num = 0;
		for (int y = 0; y < data.height; ++y)
		{
			for (int x = 0; x < data.width; ++x)
			{
				int tile_id = layer->data->data[tile_num];
				if (tile_id > 0)
				{
					TileSet* tileset = GetTilesetFromTileId(tile_id);
					if (tileset != nullptr)
					{
						SDL_Rect r = tileset->GetTileRect(tile_id);
						iPoint pos = MapToWorld(x, y);
						if (layer->data->name == "Decor")
							App->render->Blit(tileset->texture, pos.x, pos.y, &r, SDL_FLIP_NONE, 1.0F);
						else if (layer->data->name == "anim")
							App->render->Blit(tileset->texture, pos.x, pos.y, &tileset->tmxAnim->GetCurrentFrame(), SDL_FLIP_NONE, 1.0F); //animations layer
						else if(layer->data->name == "Capa de Patrones 1")
							App->render->Blit(tileset->texture, pos.x, pos.y, &r);  //playable layer
						else if (layer->data->name == "parallax")
							App->render->Blit(tileset->texture, pos.x, pos.y, &r, SDL_FLIP_NONE, 0.8F);
						else if (layer->data->name == "parallax2")
							App->render->Blit(tileset->texture, pos.x, pos.y, &r, SDL_FLIP_NONE, 0.5F);
						else if (layer->data->name == "parallax3")
							App->render->Blit(tileset->texture, pos.x, pos.y, &r, SDL_FLIP_NONE, 0.7F);
						else if (layer->data->name == "parallax4")
							App->render->Blit(tileset->texture, pos.x, pos.y, &r, SDL_FLIP_NONE, 0.3F);
						else if (layer->data->name == "bg")
							App->render->Blit(tileset->texture, pos.x, pos.y, &r, SDL_FLIP_NONE, 0.057F);
					}
				}

				tile_num++;
			}
		}
	}


}

TileSet* j1Map::GetTilesetFromTileId(int id) const
{
	p2List_item<TileSet*>* item = data.tilesets.start;
	TileSet* set = item->data;

	while (item != NULL)
	{
		if (id < item->data->firstgid)
		{
			set = item->prev->data;
			break;
		}
		set = item->data;
		item = item->next;
	}

	return set;
}

iPoint j1Map::MapToWorld(int x, int y) const
{
	iPoint ret(0, 0);

	if (data.type == MapTypes::MAPTYPE_ORTHOGONAL) {
		ret.x = x * data.tile_width;
		ret.y = y * data.tile_height;
	}
	else if (data.type == MapTypes::MAPTYPE_ISOMETRIC) {
		ret.x = (x - y) * (data.tile_width / 2);
		ret.y = (x + y) * (data.tile_height / 2);
	}

	return ret;
}

iPoint j1Map::WorldToMap(int x, int y) const
{
	iPoint ret(0, 0);

	if (data.type == MapTypes::MAPTYPE_ORTHOGONAL) {
		ret.x = x / data.tile_width;
		ret.y = y / data.tile_height;
	}

	else if (data.type == MapTypes::MAPTYPE_ISOMETRIC) {
		ret.x = (x / (data.tile_width / 2) + y / (data.tile_height / 2)) / 2;
		ret.y = (y / (data.tile_height / 2) - x / (data.tile_width / 2)) / 2;
	}
	return ret;
}

// Method that receives a tile id and returns its rectangle
SDL_Rect TileSet::GetTileRect(int id) const
{
	SDL_Rect rect;
	int relative_id = id - firstgid;

	rect.w = tile_width;
	rect.h = tile_height;
	rect.x = margin + ((rect.w + spacing) * (relative_id % num_tiles_width));
	rect.y = margin + ((rect.h + spacing) * (relative_id / num_tiles_width));

	return rect;
}

bool j1Map::CleanUp()
{
	LOG("Unloading map");

	// Remove all tilesets
	p2List_item<TileSet*>* item;
	item = data.tilesets.start;

	while (item != NULL)
	{
		RELEASE(item->data);
		item = item->next;
	}
	data.tilesets.clear();

	// Remove all layers
	p2List_item<MapLayer*>* item2;
	item2 = data.layers.start;

	while (item2 != NULL)
	{
		RELEASE(item2->data);
		item2 = item2->next;
	}
	data.layers.clear();

	// Clean up the pugui tree
	map_file.reset();

	return true;
}

// Load new map
bool j1Map::Load(const char* file_name)
{
	bool ret = true;
	p2SString tmp("%s%s", folder.GetString(), file_name);

	pugi::xml_parse_result result = map_file.load_file(tmp.GetString());

	if (result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}

	// Load general info ----------------------------------------------
	if (ret == true)
	{
		ret = LoadMap();
	}

	// Load all tilesets info ----------------------------------------------
	pugi::xml_node tileset;
	for (tileset = map_file.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
	{
		TileSet* set = new TileSet();

		if (ret == true)
		{
			ret = LoadTilesetDetails(tileset, set);
		}

		if (ret == true)
		{
			ret = LoadTilesetImage(tileset, set);
		}

		data.tilesets.add(set);
	}

	// Load layer info ----------------------------------------------
	pugi::xml_node layer;
	for (layer = map_file.child("map").child("layer"); layer && ret; layer = layer.next_sibling("layer"))
	{
		MapLayer* lay = new MapLayer();

		ret = LoadLayer(layer, lay);

		if (ret == true)
			data.layers.add(lay);
	}

	if (ret == true)
	{
		LOG("Successfully parsed map XML file: %s", file_name);
		LOG("width: %d height: %d", data.width, data.height);
		LOG("tile_width: %d tile_height: %d", data.tile_width, data.tile_height);

		p2List_item<TileSet*>* item = data.tilesets.start;
		while (item != NULL)
		{
			TileSet* s = item->data;
			LOG("Tileset ----");
			LOG("name: %s firstgid: %d", s->name.GetString(), s->firstgid);
			LOG("tile width: %d tile height: %d", s->tile_width, s->tile_height);
			LOG("spacing: %d margin: %d", s->spacing, s->margin);
			item = item->next;
		}

		p2List_item<MapLayer*>* item_layer = data.layers.start;
		while (item_layer != NULL)
		{
			MapLayer* l = item_layer->data;
			LOG("Layer ----");
			LOG("name: %s", l->name.GetString());
			LOG("tile width: %d tile height: %d", l->width, l->height);
			item_layer = item_layer->next;
		}
	}

	LoadColliders();

	map_loaded = ret;

	return ret;
}

// Load map general properties
bool j1Map::LoadMap()
{
	bool ret = true;
	pugi::xml_node map = map_file.child("map");

	if (map == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		data.width = map.attribute("width").as_int();
		data.height = map.attribute("height").as_int();
		data.tile_width = map.attribute("tilewidth").as_int();
		data.tile_height = map.attribute("tileheight").as_int();
		p2SString bg_color(map.attribute("backgroundcolor").as_string());

		data.background_color.r = 0;
		data.background_color.g = 0;
		data.background_color.b = 0;
		data.background_color.a = 0;

		if (bg_color.Length() > 0)
		{
			p2SString red, green, blue;
			bg_color.SubString(1, 2, red);
			bg_color.SubString(3, 4, green);
			bg_color.SubString(5, 6, blue);

			int v = 0;

			sscanf_s(red.GetString(), "%x", &v);
			if (v >= 0 && v <= 255) data.background_color.r = v;

			sscanf_s(green.GetString(), "%x", &v);
			if (v >= 0 && v <= 255) data.background_color.g = v;

			sscanf_s(blue.GetString(), "%x", &v);
			if (v >= 0 && v <= 255) data.background_color.b = v;
		}

		p2SString orientation(map.attribute("orientation").as_string());

		if (orientation == "orthogonal")
		{
			data.type = MAPTYPE_ORTHOGONAL;
		}
		else if (orientation == "isometric")
		{
			data.type = MAPTYPE_ISOMETRIC;
		}
		else if (orientation == "staggered")
		{
			data.type = MAPTYPE_STAGGERED;
		}
		else
		{
			data.type = MAPTYPE_UNKNOWN;
		}
	}

	return ret;
}

bool j1Map::LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	set->name.create(tileset_node.attribute("name").as_string());
	set->firstgid = tileset_node.attribute("firstgid").as_int();
	set->tile_width = tileset_node.attribute("tilewidth").as_int();
	set->tile_height = tileset_node.attribute("tileheight").as_int();
	set->margin = tileset_node.attribute("margin").as_int();
	set->spacing = tileset_node.attribute("spacing").as_int();
	pugi::xml_node offset = tileset_node.child("tileoffset");

	if (offset != NULL)
	{
		set->offset_x = offset.attribute("x").as_int();
		set->offset_y = offset.attribute("y").as_int();
	}
	else
	{
		set->offset_x = 0;
		set->offset_y = 0;
	}

	return ret;
}

bool j1Map::LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	pugi::xml_node image = tileset_node.child("image");

	if (image == NULL)
	{
		LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
		ret = false;
	}
	else
	{
		set->texture = App->tex->Load(PATH(folder.GetString(), image.attribute("source").as_string()));
		int w, h;
		SDL_QueryTexture(set->texture, NULL, NULL, &w, &h);
		set->tex_width = image.attribute("width").as_int();

		if (set->tex_width <= 0)
		{
			set->tex_width = w;
		}

		set->tex_height = image.attribute("height").as_int();

		if (set->tex_height <= 0)
		{
			set->tex_height = h;
		}

		set->num_tiles_width = set->tex_width / set->tile_width;
		set->num_tiles_height = set->tex_height / set->tile_height;
	}

	// animations from tmx 
	if (tileset_node.child("tile").child("animation")) {

		set->tmxAnim = new Animation;

		pugi::xml_node tileAnim;
		pugi::xml_node durationAnim;

		// load convertor values from xml
		float convertor;

		if (App->scene1->tutorial_active)
			convertor = 0.00008f;
		else if (App->scene1->level1_active)
			convertor = 0.00002f;
		else if (App->scene1->midlevel_active)
			convertor = 0.00013f;


		for (tileAnim = tileset_node.child("tile").child("animation").child("frame"); tileAnim; tileAnim = tileAnim.next_sibling()) 
		{
			set->tmxAnim->PushBack(set->GetTileRect(tileAnim.attribute("tileid").as_int() + set->firstgid));
		}

		durationAnim = tileset_node.child("tile").child("animation").child("frame");
		set->tmxAnim->speed = durationAnim.attribute("duration").as_float() * convertor;
	}

	return ret;
}

bool j1Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
	bool ret = true;

	layer->name = node.attribute("name").as_string();
	layer->width = node.attribute("width").as_int();
	layer->height = node.attribute("height").as_int();
	pugi::xml_node layer_data = node.child("data");

	if (layer_data == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'layer/data' tag.");
		ret = false;
		RELEASE(layer);
	}
	else
	{
		layer->data = new uint[layer->width*layer->height];
		memset(layer->data, 0, layer->width*layer->height);

		int i = 0;
		for (pugi::xml_node tile = layer_data.child("tile"); tile; tile = tile.next_sibling("tile"))
		{
			layer->data[i++] = tile.attribute("gid").as_int(0);
		}
	}

	return ret;
}


uint MapLayer::Get(int x, int y) const
{
	return (y * width) + x;
}


bool j1Map::LoadColliders()
{
	bool ret = true;

	pugi::xml_node objectgroup;
	pugi::xml_node object;
	const char* name;

	for (objectgroup = map_file.child("map").child("objectgroup"); objectgroup && ret; objectgroup = objectgroup.next_sibling("objectgroup")) 
	{
		name = objectgroup.attribute("name").as_string();

		for (object = objectgroup.child("object"); object && ret; object = object.next_sibling("object")) 
		{
			if (strcmp(name, "wall_collider") == 0)
				App->collisions->AddCollider({ object.attribute("x").as_int(), object.attribute("y").as_int(), object.attribute("width").as_int(), object.attribute("height").as_int() }, COLLIDER_WALL);

			if (strcmp(name, "death_collider") == 0)
				App->collisions->AddCollider({ object.attribute("x").as_int(), object.attribute("y").as_int(), object.attribute("width").as_int(), object.attribute("height").as_int() }, COLLIDER_DEATH);

			if (strcmp(name, "chest_collider") == 0)
				App->collisions->AddCollider({ object.attribute("x").as_int(), object.attribute("y").as_int(), object.attribute("width").as_int(), object.attribute("height").as_int() }, COLLIDER_OPENCHEST);

			if (strcmp(name, "win_collider") == 0)
				App->collisions->AddCollider({ object.attribute("x").as_int(), object.attribute("y").as_int(), object.attribute("width").as_int(), object.attribute("height").as_int() }, COLLIDER_WIN);

			if (strcmp(name, "opendoor_collider") == 0)
				App->collisions->AddCollider({ object.attribute("x").as_int(), object.attribute("y").as_int(), object.attribute("width").as_int(), object.attribute("height").as_int() }, COLLIDER_OPENDOOR);

		}
	}

	return true;
}
