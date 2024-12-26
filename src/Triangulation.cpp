#include "Triangulation.h"
#include "mapbox/earcut.hpp"

namespace mapbox {
namespace util {

template <>
struct nth<0, QPointF> {
    inline static auto get(const QPointF &t) {
        return t.x();
    };
};
template <>
struct nth<1, QPointF> {
    inline static auto get(const QPointF &t) {
        return t.y();
    };
};

} // namespace util
} // namespace mapbox

namespace {

struct Edge
{
    uint32_t v1;
    uint32_t v2;
    uint32_t triangle_count;
};

} // namespace

struct Triangulation::Mesh
{
    std::vector<Edge> edges;

    Edge * findEdge(uint32_t _v1, uint32_t _v2)
    {
        for(Edge & edge : edges)
        {
            if((edge.v1 == _v1 && edge.v2 == _v2) || (edge.v1 == _v2 && edge.v2 == _v1))
                return &edge;
        }
        return nullptr;
    }

    void incrementTriangleCout(uint32_t _v1, uint32_t _v2)
    {
        Edge * edge = findEdge(_v1, _v2);
        if(edge)
            ++edge->triangle_count;
        else
            edges.push_back(Edge { .v1 = _v1, .v2 = _v2, .triangle_count = 1 });
    }
};

Triangulation::Triangulation() :
    mp_mesh(new Mesh)
{
}

Triangulation::~Triangulation()
{
    delete mp_mesh;
}

void Triangulation::calculate(const QList<QPointF> & _points)
{
    reset();
    if(_points.size() < 3)
        return;

    m_vertices = mapbox::earcut(QList<QList<QPointF>>{ _points });
    mp_mesh->edges.clear();
    for(size_t i = 0; i < m_vertices.size(); i += 3)
    {
        mp_mesh->incrementTriangleCout(m_vertices[i], m_vertices[i + 1]);
        mp_mesh->incrementTriangleCout(m_vertices[i], m_vertices[i + 2]);
        mp_mesh->incrementTriangleCout(m_vertices[i + 1], m_vertices[i + 2]);
    }
}

void Triangulation::reset()
{
    m_vertices.clear();
    mp_mesh->edges.clear();
}

bool Triangulation::isInternalEdge(uint32_t _v1, uint32_t _v2) const
{
    Edge * edge = mp_mesh->findEdge(_v1, _v2);
    return edge && edge->triangle_count > 1;
}
