#pragma once

#include "Triangulation.h"
#include <QObject>

class PolygonMesh: public QObject
{
    Q_OBJECT

public:
    explicit PolygonMesh(QObject * _parent = nullptr) :
        QObject(_parent)
    {
    }

    void setVertices(const QList<QPointF> & _vertices)
    {
        m_vertices = _vertices;
        m_triangulation.calculate(_vertices);
    }

    const QList<QPointF> & getVertices() const
    {
        return m_vertices;
    }

    const QPointF & operator[](qsizetype _idx) const
    {
        return m_vertices[_idx];
    }

    qsizetype getVertexCount() const
    {
        return m_vertices.size();
    }

    const Triangulation & getTriangulation() const
    {
        return m_triangulation;
    }

private:
    QList<QPointF> m_vertices;
    Triangulation m_triangulation;
};

QList<QPointF> calculateInnerPolygon(const QList<QPointF> & _polygon, float _padding);
