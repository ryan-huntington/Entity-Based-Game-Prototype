#include "Vec2.h"
#include <cmath>

Vec2::Vec2()
{
}

Vec2::Vec2(float xin, float yin)
{
	x = xin;
	y = yin;
}

bool Vec2::operator==(const Vec2& rhs) const
{
	return (x == rhs.x && y == rhs.y);
}

bool Vec2::operator!=(const Vec2& rhs) const
{
	return false;
}

Vec2 Vec2::operator+(const Vec2& rhs) const
{
	return Vec2(x + rhs.x, y + rhs.y);
}

Vec2 Vec2::operator-(const Vec2& rhs) const
{
	return Vec2(x - rhs.x, y - rhs.y);
}

Vec2 Vec2::operator/(const float val) const
{
	return Vec2(x/val, y/val);
}

Vec2 Vec2::operator*(const float val) const
{
	return Vec2(x*val, y*val);
}

Vec2 Vec2::operator+=(const Vec2& rhs) const
{
	return Vec2(x + rhs.x, y + rhs.y);
}

Vec2 Vec2::operator-=(const Vec2& rhs) const
{
	return Vec2(x - rhs.x, y - rhs.y);
}

Vec2 Vec2::operator*=(const float val) const
{
	return Vec2(x * val, y * val);
}

Vec2 Vec2::operator/=(const float val) const
{
	return Vec2(x / val, y / val);
}

float Vec2::dist(const Vec2& rhs) const
{
	float dx = rhs.x - this->x;
	float dy = rhs.y - this->y;
	return std::sqrt(dx * dx + dy * dy);
}

Vec2 Vec2::normalize() const {
	float len = sqrt(x * x + y * y);
	if (len == 0) return Vec2(0, 0);
	return Vec2(x / len, y / len);
}

