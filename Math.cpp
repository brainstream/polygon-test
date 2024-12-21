#include "Math.h"

float calculateSignedArea(const QList<QPointF> & _points)
{
    if(_points.size() < 3)
    {
        return 0.0f;
    }
    float sum = _points[0].x() * (_points[1].y() - _points[_points.size() - 1].y()) +
                _points[_points.size() - 1].x() * (_points[0].y() - _points[_points.size() - 2].y());
    for(qsizetype i = 1; i < _points.size() - 1; ++i)
    {
        sum += _points[i].x() * (_points[i + 1].y() - _points[i - 1].y());
    }
    return 0.5f * sum;
}
