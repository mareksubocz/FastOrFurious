#ifndef ALGEBRA
#define ALGEBRA
#include <SFML/Graphics.hpp>
#include <math.h>

template<typename F>
F rad2deg(F angle) {
    return angle * 180.0 / M_PI;
}

template<typename F>
F sign(F val){
  if (val > 0){
    return 1;
  }
  else if (val == 0){
    return 0;
  }
  return -1;
}

template<typename F>
F sign(F val, F eps){
  if (val-eps > 0){
    return 1;
  }
  else if (val+eps < 0){
    return -1;
  }
  return 0;
}

template<typename F>
F clip(F val, F low, F high){
  if (val > high){
    return high;
  }
  else if (val < low){
    return low;
  }
  return val;
}

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


float vectorRotation(sf::Vector2f &v) {
  if (v.y >= 0 and v.x >= 0) {
    return rad2deg(atan(v.x / v.y));
  } // quadrant 1
  else if (v.y < 0 and v.x >= 0) {
    return 180 - rad2deg(atan(v.x / -v.y));
  } // quadrant 2
  else if (v.y < 0 and v.x < 0) {
    return 180 + rad2deg(atan(-v.x / -v.y));
  } // quadrant 3
  else {
    return 360 - rad2deg(atan(-v.x / v.y));
  } // quadrant 4
}


#endif
