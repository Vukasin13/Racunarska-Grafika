#include "../Header/MathUtils.h"

float calculateDistance(Point p1, Point p2) {
    float dx = p1.x - p2.x;
    float dy = p1.y - p2.y;
    return std::sqrt(dx * dx + dy * dy) * 5000.0f;
}

float calculateTotalDistance(const std::vector<Point>& points) {
    float totalDist = 0.0f;
    if (points.size() < 2) return 0.0f;

    for (size_t i = 0; i < points.size() - 1; ++i) {
        totalDist += calculateDistance(points[i], points[i + 1]);
    }
    return totalDist;
}