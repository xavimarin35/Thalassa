#include "j1Pathfinding.h"
#include "j1App.h"
#include "p2Defs.h"
#include "p2Log.h"

#include "Brofiler/Brofiler.h"

j1Pathfinding::j1Pathfinding() : j1Module(), map(NULL), last_path(DEFAULT_PATH_LENGTH), width(0), height(0)
{
	name.create("pathfinding");
}

j1Pathfinding::~j1Pathfinding()
{
	RELEASE_ARRAY(map);
}

bool j1Pathfinding::CleanUp()
{
	LOG("Freeing pathfinding library");

	last_path.Clear();
	RELEASE_ARRAY(map);

	return true;
}

void j1Pathfinding::SetMap(uint width, uint height, uchar* data)
{
	this->width = width;
	this->height = height;

	RELEASE_ARRAY(map);
	map = new uchar[width * height];
	memcpy(map, data, width * height);

}

bool j1Pathfinding::CheckBoundaries(const iPoint& pos) const
{
	return (pos.x >= 0 && pos.x <= (int)width &&
		pos.y >= 0 && pos.y <= (int)height);
}

bool j1Pathfinding::IsWalkable(const iPoint& pos) const
{
	uchar t = GetTileAt(pos);
	//return t != INVALID_WALK_CODE && t > 0;
	return true;
}

uchar j1Pathfinding::GetTileAt(const iPoint& pos) const
{
	if (CheckBoundaries(pos))
		return map[(pos.y*width) + pos.x];

	return INVALID_WALK_CODE;
}

PATH_MOVEMENT j1Pathfinding::CheckDirection(p2DynArray<iPoint>& path) const
{
	if (path.Count() >= 2)
	{
		iPoint tile = path[0];
		iPoint next_tile = path[1];

		int x_difference = next_tile.x - tile.x;
		int y_difference = next_tile.y - tile.y;

		if (x_difference == 1 && y_difference == 1) return DOWN_RIGHT;
		else if (x_difference == 1 && y_difference == -1) return UP_RIGHT;
		else if (x_difference == -1 && y_difference == 1) return DOWN_LEFT;
		else if (x_difference == -1 && y_difference == -1) return UP_LEFT;
		else if (x_difference == 1) return RIGHT;
		else if (x_difference == -1) return LEFT;
		else if (y_difference == 1)	return DOWN;
		else if (y_difference == -1) return UP;
	}

	else return NO_MOVE;
}

PATH_MOVEMENT j1Pathfinding::CheckDirectionGround(p2DynArray<iPoint>& path) const
{
	if (path.Count() >= 2)
	{
		iPoint tile = path[0];
		iPoint next_tile = path[1];

		int x_difference = next_tile.x - tile.x;
		int y_difference = next_tile.y - tile.y;

		if (x_difference == 1) return RIGHT;
		else if (x_difference == -1) return LEFT;
		else if (y_difference == 1)	return DOWN;
		else if (y_difference == -1) return UP;
	}

	else return NO_MOVE;
}

const p2DynArray<iPoint>* j1Pathfinding::GetLastPath() const
{
	return &last_path;
}

p2List_item<PathNode>* PathList::Find(const iPoint& point) const
{
	p2List_item<PathNode>* item = node_list.start;

	while (item)
	{
		if (item->data.pos == point)
		{
			return item;
		}

		item = item->next;
	}

	return NULL;
}

p2List_item<PathNode>* PathList::GetNodeLowestScore() const
{
	p2List_item<PathNode>* ret = NULL;
	int min = 65535;

	p2List_item<PathNode>* item = node_list.end;
	while (item)
	{
		if (item->data.Score() < min)
		{
			min = item->data.Score();
			ret = item;
		}
		item = item->prev;
	}
	return ret;
}

PathNode::PathNode() : g(-1), h(-1), pos(-1, -1), parent(NULL) {}

PathNode::PathNode(int g, int h, const iPoint& pos, const PathNode* parent) : g(g), h(h), pos(pos), parent(parent) {}

PathNode::PathNode(const PathNode& node) : g(node.g), h(node.h), pos(node.pos), parent(node.parent) {}

uint PathNode::WalkableAdjacents(PathList& list) const
{
	iPoint cell;
	uint last_cell = list.node_list.count();

	// UP RIGHT
	cell.create(pos.x + 1, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell))
		list.node_list.add(PathNode(-1, -1, cell, this));

	// UP LEFT
	cell.create(pos.x - 1, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell))
		list.node_list.add(PathNode(-1, -1, cell, this));

	// DOWN RIGHT
	cell.create(pos.x + 1, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell))
		list.node_list.add(PathNode(-1, -1, cell, this));

	// DOWN LEFT
	cell.create(pos.x - 1, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell))
		list.node_list.add(PathNode(-1, -1, cell, this));

	// UP
	cell.create(pos.x, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell))
		list.node_list.add(PathNode(-1, -1, cell, this));

	// DOWN
	cell.create(pos.x, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell))
		list.node_list.add(PathNode(-1, -1, cell, this));

	// RIGHT
	cell.create(pos.x + 1, pos.y);
	if (App->pathfinding->IsWalkable(cell))
		list.node_list.add(PathNode(-1, -1, cell, this));

	// LEFT
	cell.create(pos.x - 1, pos.y);
	if (App->pathfinding->IsWalkable(cell))
		list.node_list.add(PathNode(-1, -1, cell, this));

	return list.node_list.count();
}

int PathNode::Score() const
{
	return g + h;
}

int PathNode::CalculateF(const iPoint& destination)
{
	g = parent->g + 1;

	int x_distance = abs(pos.x - destination.x);
	int y_distance = abs(pos.y - destination.y);

	h = (x_distance + y_distance) * min(x_distance, y_distance);

	return g + h;
}

p2DynArray<iPoint>* j1Pathfinding::CreatePath(iPoint& origin, iPoint& destination)
{
	BROFILER_CATEGORY("PathfindingCreation", Profiler::Color::Yellow)

	last_path.Clear();

	// Making sure origin and destination are walkable
	if (IsWalkable(origin) && IsWalkable(destination))
	{
		// Creating list open and close
		PathList open, close;
		PathNode origin(0, origin.DistanceNoSqrt(destination), origin, nullptr);
		open.node_list.add(origin);

		while (open.node_list.count() > 0)
		{
			close.node_list.add(open.GetNodeLowestScore()->data);
			open.node_list.del(open.GetNodeLowestScore());
			
			if (close.node_list.end->data.pos != destination) {

				PathList adjacent;

				close.node_list.end->data.WalkableAdjacents(adjacent);

				for (p2List_item<PathNode>* iterator = adjacent.node_list.start; iterator != nullptr; iterator = iterator->next)
				{
					if (close.Find(iterator->data.pos))
						continue;

					else if (open.Find(iterator->data.pos))
					{
						PathNode tmp = open.Find(iterator->data.pos)->data;
						iterator->data.CalculateF(destination);

						if (tmp.g > iterator->data.g)
						{
							tmp.parent = iterator->data.parent;
						}
					}

					else
					{
						iterator->data.CalculateF(destination);
						open.node_list.add(iterator->data);
					}
				}
				adjacent.node_list.clear();
			}

			else
			{
				for (p2List_item<PathNode>* iterator = close.node_list.end; iterator->data.parent != nullptr; iterator = close.Find(iterator->data.parent->pos))
				{
					last_path.PushBack(iterator->data.pos);

					if (iterator->data.parent == nullptr)
						last_path.PushBack(close.node_list.start->data.pos);
				}

				last_path.Flip();

				return &last_path;
			}
		}
	}

	else return nullptr;
}