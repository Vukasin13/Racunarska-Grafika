#pragma once
#include <vector>
#include <cmath>

struct Point {
    float x, y;
};

// Funkcija vraca distancu izmedju dve tacke
float calculateDistance(Point p1, Point p2);

// Funkcija prima niz tacaka i vraca ukupnu duzinu
float calculateTotalDistance(const std::vector<Point>& points);