#ifndef COORDINATEUTILS_HPP
#define COORDINATEUTILS_HPP

#include <QPointF>

namespace GraphUtils {
inline constexpr int gridSize = 50;
inline constexpr int gridTickStep = 10;

inline constexpr double scaleFactor()
{
    return static_cast<double>(gridSize) / gridTickStep;
}

// Grid (Math) -> Scene (Qt)
inline QPointF mapGridToScenePos(QPointF gridPos)
{
    return QPointF{gridPos.x() * scaleFactor(), -gridPos.y() * scaleFactor()};
}

// Scene (Qt) -> Grid (Math)
inline QPointF mapSceneToGridPos(QPointF scenePos)
{
    return QPointF{scenePos.x() / scaleFactor(), -scenePos.y() / scaleFactor()};
}
} // namespace GraphUtils

#endif // COORDINATEUTILS_HPP