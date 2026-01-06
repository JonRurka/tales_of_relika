#include "plane.h"

//#include "core/math/math_funcs.h"
//#include "core/variant/variant.h"

using namespace BoundsVolume;

namespace plane_math
{
	glm::vec3 vec3_cross(glm::vec3 v1, glm::vec3 v2)
	{
		return glm::cross(v1, v2);
	}

	bool is_zero_approx(float p_value)
	{
		return abs(p_value) < (float)CMP_EPSILON;
	}

	bool is_equal_approx(float p_left, float p_right) {
		// Check for exact equality first, required to handle "infinity" values.
		if (p_left == p_right) {
			return true;
		}
		// Then check for approximate equality.
		float tolerance = (float)CMP_EPSILON * abs(p_left);
		if (tolerance < (float)CMP_EPSILON) {
			tolerance = (float)CMP_EPSILON;
		}
		return abs(p_left - p_right) < tolerance;
	}

	bool is_equal_approx(const glm::vec3& s, const glm::vec3& p_v)
	{
		return is_equal_approx(s.x, p_v.x) && is_equal_approx(s.y, p_v.y) && is_equal_approx(s.z, p_v.z);
	}

	bool is_same(float p_left, float p_right) {
		return (p_left == p_right) || (std::isnan(p_left) && std::isnan(p_right));
	}

	bool is_same(const glm::vec3& s, const glm::vec3& p_v) {
		return is_same(s.x, p_v.x) && is_same(s.y, p_v.y) && is_same(s.z, p_v.z);
	}

	bool is_finite(const glm::vec3& s) {
		return std::isfinite(s.x) && std::isfinite(s.y) && std::isfinite(s.z);
	}
}


void Plane::set_normal(const glm::vec3& p_normal) {
	normal = p_normal;
}

void Plane::normalize() {
	float l = glm::length(normal);
	if (l == 0) {
		*this = Plane(0, 0, 0, 0);
		return;
	}
	normal /= l;
	d /= l;
}

Plane Plane::normalized() const {
	Plane p = *this;
	p.normalize();
	return p;
}

glm::vec3 Plane::get_any_perpendicular_normal() const {
	static const glm::vec3 p1 = glm::vec3(1, 0, 0);
	static const glm::vec3 p2 = glm::vec3(0, 1, 0);
	glm::vec3 p;

	if (glm::abs(glm::dot(normal, p1)) > 0.99f) { // if too similar to p1
		p = p2; // use p2
	} else {
		p = p1; // use p1
	}

	p -= normal * glm::dot(normal, p);
	p = glm::normalize(p);

	return p;
}

/* intersections */

bool Plane::intersect_3(const Plane &p_plane1, const Plane &p_plane2, glm::vec3* r_result) const {
	const Plane &p_plane0 = *this;
	glm::vec3 normal0 = p_plane0.normal;
	glm::vec3 normal1 = p_plane1.normal;
	glm::vec3 normal2 = p_plane2.normal;

	float denom = glm::dot(glm::cross(normal0, normal1), normal2);

	if (plane_math::is_zero_approx(denom)) {
		return false;
	}

	if (r_result) {
		*r_result = ((plane_math::vec3_cross(normal1, normal2) * p_plane0.d) +
							(plane_math::vec3_cross(normal2, normal0) * p_plane1.d) +
							(plane_math::vec3_cross(normal0, normal1) * p_plane2.d)) /
				denom;
	}

	return true;
}

bool Plane::intersects_ray(const glm::vec3& p_from, const glm::vec3& p_dir, glm::vec3* p_intersection) const {
	glm::vec3 segment = p_dir;
	float den = glm::dot(normal, segment);

	if (plane_math::is_zero_approx(den)) {
		return false;
	}

	float dist = (glm::dot(normal, p_from) - d) / den;

	if (dist > (float)CMP_EPSILON) { //this is a ray, before the emitting pos (p_from) doesn't exist

		return false;
	}

	dist = -dist;
	*p_intersection = p_from + segment * dist;

	return true;
}

bool Plane::intersects_segment(const glm::vec3& p_begin, const glm::vec3& p_end, glm::vec3* p_intersection) const {
	glm::vec3 segment = p_begin - p_end;
	float den = glm::dot(normal, segment);

	if (plane_math::is_zero_approx(den)) {
		return false;
	}

	float dist = (glm::dot(normal, p_begin) - d) / den;

	if (dist < (float)-CMP_EPSILON || dist > (1.0f + (float)CMP_EPSILON)) {
		return false;
	}

	dist = -dist;
	*p_intersection = p_begin + segment * dist;

	return true;
}

/*Variant Plane::intersect_3_bind(const Plane& p_plane1, const Plane& p_plane2) const {
	Vector3 inters;
	if (intersect_3(p_plane1, p_plane2, &inters)) {
		return inters;
	} else {
		return Variant();
	}
}

Variant Plane::intersects_ray_bind(const Vector3 &p_from, const Vector3 &p_dir) const {
	Vector3 inters;
	if (intersects_ray(p_from, p_dir, &inters)) {
		return inters;
	} else {
		return Variant();
	}
}

Variant Plane::intersects_segment_bind(const Vector3 &p_begin, const Vector3 &p_end) const {
	Vector3 inters;
	if (intersects_segment(p_begin, p_end, &inters)) {
		return inters;
	} else {
		return Variant();
	}
}*/

/* misc */

bool Plane::is_equal_approx_any_side(const Plane &p_plane) const {
	return (plane_math::is_equal_approx(normal, p_plane.normal) && plane_math::is_equal_approx(d, p_plane.d)) || (plane_math::is_equal_approx(normal, -p_plane.normal) && plane_math::is_equal_approx(d, -p_plane.d));
}

bool Plane::is_equal_approx(const Plane &p_plane) const {
	return plane_math::is_equal_approx(normal, p_plane.normal) && plane_math::is_equal_approx(d, p_plane.d);
}

bool Plane::is_same(const Plane &p_plane) const {
	return plane_math::is_same(normal, p_plane.normal) && plane_math::is_same(d, p_plane.d);
}

bool Plane::is_finite() const {
	return plane_math::is_finite(normal) && std::isfinite(d);
}

inline constexpr Plane Plane::PLANE_YZ = { 1, 0, 0, 0 };
inline constexpr Plane Plane::PLANE_XZ = { 0, 1, 0, 0 };
inline constexpr Plane Plane::PLANE_XY = { 0, 0, 1, 0 };

bool Plane::is_point_over(const glm::vec3& p_point) const {
	return (glm::dot(normal, p_point) > d);
}

float Plane::distance_to(const glm::vec3& p_point) const {
	return (glm::dot(normal, p_point) - d);
}

bool Plane::has_point(const glm::vec3& p_point, float p_tolerance) const {
	float dist = glm::dot(normal, p_point) - d;
	dist = glm::abs(dist);
	return (dist <= p_tolerance);
}

constexpr Plane::Plane(const glm::vec3& p_normal, float p_d) :
	normal(p_normal),
	d(p_d) {
}

Plane::Plane(const glm::vec3& p_normal, const glm::vec3& p_point) :
	normal(p_normal),
	d(glm::dot(p_normal, p_point)) {
}

Plane::Plane(const glm::vec3& p_point1, const glm::vec3& p_point2, const glm::vec3& p_point3, ClockDirection p_dir) {
	if (p_dir == CLOCKWISE) {
		normal = glm::cross((p_point1 - p_point3), p_point1 - p_point2);
	}
	else {
		normal = glm::cross((p_point1 - p_point2), p_point1 - p_point3);
	}

	normal = glm::normalize(normal);
	d = glm::dot(normal, p_point1);
}

constexpr bool Plane::operator==(const Plane& p_plane) const {
	return normal == p_plane.normal && d == p_plane.d;
}

constexpr bool Plane::operator!=(const Plane& p_plane) const {
	return normal != p_plane.normal || d != p_plane.d;
}

/*Plane::operator String() const {
	return "[N: " + normal.operator String() + ", D: " + String::num_real(d, false) + "]";
}*/
