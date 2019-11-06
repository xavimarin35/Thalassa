#ifndef __j1PATHFINDING_H__
#define __j1PATHFINDING_H__

#include "j1Module.h"
#include "p2Point.h"
#include "p2DynArray.h"
#include "p2List.h"

#define DEFAULT_PATH_LENGTH 50
#define INVALID_WALK_CODE 255

struct PathList;

enum PATH_MOVEMENT 
{
	NO_MOVE = -1,
	UP,
	DOWN,
	RIGHT,
	LEFT,
	UP_RIGHT,
	UP_LEFT,
	DOWN_RIGHT,
	DOWN_LEFT
};

class j1Pathfinding : public j1Module
{

public:

	j1Pathfinding();

	// Destructor
	~j1Pathfinding();

	// Called before quitting
	bool CleanUp();

	// Sets up the walkability map
	void SetMap(uint width, uint height, uchar* data);

	// Main function to request a path from A to B
	p2DynArray<iPoint>* CreatePath(iPoint& origin, iPoint& destination);

	// To request all tiles involved in the last generated path
	const p2DynArray<iPoint>* GetLastPath() const;

	// Utility: return true if pos is inside the map boundaries
	bool CheckBoundaries(const iPoint& pos) const;

	// Utility: returns true is the tile is walkable
	bool IsWalkable(const iPoint& pos) const;

	// Utility: return the walkability value of a tile
	uchar GetTileAt(const iPoint& pos) const;

	PATH_MOVEMENT CheckDirection(p2DynArray<iPoint>& path)const;
	PATH_MOVEMENT CheckDirectionGround(p2DynArray<iPoint>& path)const;

private:

	p2DynArray<iPoint>* _path = nullptr;

	uint width;
	uint height;

	uchar* map = nullptr;

	p2DynArray<iPoint> last_path;

};

struct PathNode
{
	PathNode();
	PathNode(int g, int h, const iPoint& pos, const PathNode* parent);
	PathNode(const PathNode& node);

	uint WalkableAdjacents(PathList& list) const;

	int Score() const;
	int CalculateF(const iPoint& destination);

	int g, h;
	iPoint pos;
	const PathNode* parent = nullptr;

};

struct PathList
{
	p2List_item<PathNode>* Find(const iPoint& point) const;
	p2List_item<PathNode>* GetNodeLowestScore() const;

	p2List<PathNode> node_list;
};

#endif