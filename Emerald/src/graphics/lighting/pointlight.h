#pragma once

class Pointlight {
public:
    glm::vec3 position;
    float radius;
    Color color;

    Pointlight() : position(glm::vec3(0, 0, 0)), radius(1), color(Color(1, 0, 0)) {}
    Pointlight(const glm::vec3& _position, float _radius, const Color& _color) : position(_position), radius(_radius), color(_color) {}
};