#include "halfEdge.h"

std::tuple<Vertex*, Face*, Solid*> mvfs(const point& coord)
{
	auto solid = new Solid;
	auto face = new Face;
	auto loop = new Loop;
	auto vertex = new Vertex;

	solid->faces.push_back(face);
	face->parentSolid = solid;

	face->outerLoop = loop;
	loop->parentFace = face;

	vertex->coordinate = coord;
	solid->vertices.push_back(vertex);
	return std::make_tuple(vertex, face, solid);
}

std::tuple<Edge*, Vertex*> mev(Vertex* vertex0, Loop* loop, point coord)
{
	auto vertex1 = new Vertex;
	auto edge = new Edge;
	auto halfEdge0 = new HalfEdge;
	auto halfEdge1 = new HalfEdge;

	vertex1->coordinate = coord;
	halfEdge0->vertex = vertex0;
	halfEdge1->vertex = vertex1;

	halfEdge0->next = halfEdge1;
	halfEdge1->pre = halfEdge0;

	halfEdge0->parentEdge = edge;
	halfEdge1->parentEdge = edge;
	halfEdge0->parentLoop = loop;
	halfEdge1->parentLoop = loop;

	edge->halfEdge0 = halfEdge0;
	edge->halfEdge1 = halfEdge1;

	if (loop->leadingHalfEdge == nullptr) {
		loop->leadingHalfEdge = halfEdge0;
		halfEdge0->pre = halfEdge1;
		halfEdge1->next = halfEdge0;
	}
	else {
		auto tmp = loop->find(vertex0);
		halfEdge0->pre = tmp->pre;
		halfEdge1->next = tmp;
		halfEdge0->pre->next = halfEdge0;
		halfEdge1->next->pre = halfEdge1;
	}
	auto solid = loop->parentFace->parentSolid;
	solid->edges.push_back(edge);
	solid->vertices.push_back(vertex1);
	return std::make_tuple(edge, vertex1);
}

std::tuple<Edge*, Face*> mef(Loop* loop0, Vertex* vertex0, Vertex* vertex1)
{
	auto solid = loop0->parentFace->parentSolid;
	auto face = new Face;
	auto loop1 = new Loop;
	auto edge = new Edge;
	auto halfEdge0 = new HalfEdge;
	auto halfEdge1 = new HalfEdge;

	solid->faces.push_back(face);
	face->parentSolid = loop0->parentFace->parentSolid;
	face->outerLoop = loop1;

	solid->edges.push_back(edge);
	edge->halfEdge0 = halfEdge0;
	edge->halfEdge1 = halfEdge1;

	loop1->parentFace = face;

	auto tmphe0 = loop0->find(vertex0);
	auto tmphe1 = loop0->find(vertex1);

	halfEdge0->next = tmphe1;
	halfEdge0->pre = tmphe0->pre;
	halfEdge1->next = tmphe0;
	halfEdge1->pre = tmphe1->pre;

	tmphe0->pre->next = halfEdge0;
	tmphe1->pre->next = halfEdge1;
	tmphe0->pre = halfEdge1;
	tmphe1->pre = halfEdge0;

	halfEdge0->parentLoop = loop0;
	halfEdge0->parentEdge = edge;
	halfEdge1->parentLoop = loop1;
	halfEdge1->parentEdge = edge;
	halfEdge0->vertex = vertex0;
	halfEdge1->vertex = vertex1;

	loop1->leadingHalfEdge = halfEdge1;
	loop0->leadingHalfEdge = halfEdge0;
	return std::make_tuple(edge, face);
}

std::tuple<Loop*> kemr(Vertex* vertex0, Vertex* vertex1, Loop* loop0)
{
	auto solid = loop0->parentFace->parentSolid;
	auto loop1 = new Loop;

	HalfEdge* halfEdge0 = loop0->leadingHalfEdge;
	HalfEdge* halfEdge1 = halfEdge0->next;
	while (halfEdge0->vertex != vertex0 || halfEdge1->vertex != vertex1) {
		halfEdge0 = halfEdge1;
		halfEdge1 = halfEdge0->next;
	}

	loop0->leadingHalfEdge = halfEdge1->next;
	halfEdge0->pre->next = halfEdge1->next;
	halfEdge1->next->pre = halfEdge0->pre;

	loop1->parentFace = loop0->parentFace;
	loop1->parentFace->innerLoops.push_back(loop1);

	auto edge = halfEdge0->parentEdge;
	solid->edges.remove(edge);
	delete edge;

	delete halfEdge0;
	delete halfEdge1;

	return std::make_tuple(loop1);
}

void kfmrh(Loop* outerLoop, Loop* innerLoop)
{
	auto solid = outerLoop->parentFace->parentSolid;
	outerLoop->parentFace->innerLoops.push_back(innerLoop);
	delete innerLoop->parentFace;
	solid->faces.remove(innerLoop->parentFace);
	innerLoop->parentFace = outerLoop->parentFace;
}

point operator + (point c0, point c1)
{
	c0.x += c1.x;
	c0.y += c1.y;
	c0.z += c1.z;
	return c0;
}

void sweepFace(Face* face, point dir)
{
	Loop* loop = face->outerLoop;
	auto endVertex = loop->leadingHalfEdge->vertex;
	auto  hfedge = loop->leadingHalfEdge;
	auto initialVertex = std::get<1>(mev(hfedge->vertex, loop, hfedge->vertex->coordinate + dir));
	hfedge = hfedge->next;
	auto lastVertex = initialVertex;
	while (hfedge->vertex != endVertex) {
		auto nextVertex = std::get<1>(mev(hfedge->vertex, loop, hfedge->vertex->coordinate + dir));
		mef(loop, lastVertex, nextVertex);
		lastVertex = nextVertex;
		hfedge = hfedge->next;
	}
	mef(loop, lastVertex, initialVertex);

	if (face->innerLoops.size() > 0)
	{
		for (auto it = face->innerLoops.begin(); it != face->innerLoops.end(); it++)
		{
			Loop* loop = *it;
			auto endVertex = loop->leadingHalfEdge->vertex;
			auto  hfedge = loop->leadingHalfEdge;
			auto initialVertex = std::get<1>(mev(hfedge->vertex, loop, hfedge->vertex->coordinate + dir));
			hfedge = hfedge->next;
			auto lastVertex = initialVertex;
			while (hfedge->vertex != endVertex) {
				auto nextVertex = std::get<1>(mev(hfedge->vertex, loop, hfedge->vertex->coordinate + dir));
				mef(loop, lastVertex, nextVertex);
				lastVertex = nextVertex;
				hfedge = hfedge->next;
			}
			mef(loop, lastVertex, initialVertex);
		}
	}
}

void findCorner(Loop* loop,std::vector<point>& pnts)
{
	auto hf = loop->leadingHalfEdge;
	point p1 = hf->vertex->coordinate;
	hf = hf->next;
	point p2 = hf->vertex->coordinate;
	hf = hf->next;
	point p3 = hf->vertex->coordinate;
	hf = hf->next;
	point p4 = hf->vertex->coordinate;
	if (p1.x == p2.x)
	{
		if (p1.y > p2.y)
		{
			if (p1.x > p4.x)
			{
				pnts.push_back(p4);
				pnts.push_back(p3);
				pnts.push_back(p2);
				pnts.push_back(p1);
			}
			else
			{
				pnts.push_back(p1);
				pnts.push_back(p2);
				pnts.push_back(p3);
				pnts.push_back(p4);
			}
		}
		else
		{
			if (p1.x > p4.x)
			{
				pnts.push_back(p3);
				pnts.push_back(p4);
				pnts.push_back(p1);
				pnts.push_back(p2);
			}
			else
			{
				pnts.push_back(p2);
				pnts.push_back(p1);
				pnts.push_back(p4);
				pnts.push_back(p3);
			}
		}
	}
	else
	{
		if (p1.x < p2.x)
		{
			if (p1.y > p4.y)
			{
				pnts.push_back(p1);
				pnts.push_back(p4);
				pnts.push_back(p3);
				pnts.push_back(p2);
			}
			else
			{
				pnts.push_back(p4);
				pnts.push_back(p1);
				pnts.push_back(p2);
				pnts.push_back(p3);
			}
		}
		else
		{
			if (p1.y > p4.y)
			{
				pnts.push_back(p2);
				pnts.push_back(p3);
				pnts.push_back(p4);
				pnts.push_back(p1);
			}
			else
			{
				pnts.push_back(p3);
				pnts.push_back(p2);
				pnts.push_back(p1);
				pnts.push_back(p4);
			}
		}
	}
}

HalfEdge* Loop::find(Vertex* vertex)
{
	auto he = leadingHalfEdge;
	while (he) {
		if (he->vertex == vertex) return he;
		he = he->next;
		if (he == leadingHalfEdge) break;
	}
	return nullptr;
}
