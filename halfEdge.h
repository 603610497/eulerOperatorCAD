#pragma once
#include <list>
#include <tuple>
#include <vector>

struct Solid;
struct Face;
struct Loop;
struct HalfEdge;
struct Edge;
struct Vertex;

struct point
{
	float x, y, z;
};

struct Vertex
{
	point coordinate;
};

struct Edge
{
	HalfEdge* halfEdge0 = nullptr;
	HalfEdge* halfEdge1 = nullptr;
};

struct HalfEdge
{
	Loop* parentLoop = nullptr;
	Edge* parentEdge = nullptr;
	Vertex* vertex = nullptr;

	HalfEdge* next = nullptr;
	HalfEdge* pre = nullptr;

	HalfEdge* brother() {
		if (this == this->parentEdge->halfEdge0)
			return this->parentEdge->halfEdge1;
		else
			return this->parentEdge->halfEdge0;
	};
};

struct Loop
{
	Face* parentFace = nullptr;
	HalfEdge* leadingHalfEdge = nullptr;
	HalfEdge* find(Vertex* vertex);
};

struct Face
{
	Solid* parentSolid = nullptr;
	Loop* outerLoop = nullptr;
	std::list<Loop*> innerLoops;
};

struct Solid
{
	std::list<Face*>	faces;
	std::list<Edge*>	edges;
	std::list<Vertex*>	vertices;
};

std::tuple<Vertex*, Face*, Solid*> mvfs(const point& coord);
std::tuple<Edge*, Vertex*> mev(Vertex* vertex0, Loop* loop, point coord);
std::tuple<Edge*, Face*> mef(Loop* loop, Vertex* vertex0, Vertex* vertex1);
std::tuple<Loop*> kemr(Vertex* vertex0, Vertex* vertex1, Loop* loop0);
void kfmrh(Loop* outerLoop, Loop* innerLoop);
void sweepFace(Face* face, point dir);
void findCorner(Loop* loop,std::vector<point>& point);