#ifndef ALGEBRA
#define ALGEBRA
#include <SFML/Graphics.hpp>
#include <math.h>

float dot(const sf::Vector2f &a, const sf::Vector2f &b) {
  return a.x * b.x + a.y * b.y;
}

float distance(const sf::Vector2f &a, const sf::Vector2f &b) {
  sf::Vector2f diff = a - b;
  return hypotf(diff.x, diff.y);
}

sf::Vector2f rotateToShape(const sf::Vector2f &v, const sf::Transformable &t) {
  sf::Vector2f diffRotated =
      t.getPosition() - t.getTransform().transformPoint(t.getOrigin() + v);
  return diffRotated;
}

#endif
