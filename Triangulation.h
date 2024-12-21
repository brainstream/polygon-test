#pragma once

#include <QList>
#include <QPointF>
#include <vector>
#include <cstdint>

class Triangulation final
{
    struct Mesh;

public:
    Triangulation();
    ~Triangulation();

    void calculate(const QList<QPointF> & _points);
    void reset();
    bool isInternalEdge(uint32_t _v1, uint32_t _v2) const;

    size_t getVertexCount() const
    {
        return m_vertices.size();
    }

    const std::vector<uint32_t> & getVertices() const
    {
        return m_vertices;
    }

private:
    std::vector<uint32_t> m_vertices;
    Mesh * mp_mesh;
};


