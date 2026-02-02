
#include "AABB.h"

#include "Graphics.h"

#include <limits>

//#include "core/string/ustring.h"
//#include "core/variant/variant.h"

using namespace BoundsVolume;

namespace aabb_math
{
	inline constexpr double INF = std::numeric_limits<double>::infinity();
	inline constexpr double NaN = std::numeric_limits<double>::quiet_NaN();
	#define HASH_MURMUR3_SEED 0x7F07C65

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

	float fposmodp(float p_x, float p_y) {
		float value = fmod(p_x, p_y);
		if (value < 0) {
			value += p_y;
		}
		value += 0.0f;
		return value;
	}

	uint32_t hash_murmur3_one_32(uint32_t p_in, uint32_t p_seed = HASH_MURMUR3_SEED) {
		p_in *= 0xcc9e2d51;
		p_in = (p_in << 15) | (p_in >> 17);
		p_in *= 0x1b873593;

		p_seed ^= p_in;
		p_seed = (p_seed << 13) | (p_seed >> 19);
		p_seed = p_seed * 5 + 0xe6546b64;

		return p_seed;
	}

	uint32_t hash_murmur3_one_float(float p_in, uint32_t p_seed = HASH_MURMUR3_SEED) {
		union {
			float f;
			uint32_t i;
		} u;

		// Normalize +/- 0.0 and NaN values so they hash the same.
		if (p_in == 0.0f) {
			u.f = 0.0;
		}
		else if (std::isnan(p_in)) {
			u.f = NaN;
		}
		else {
			u.f = p_in;
		}

		return hash_murmur3_one_32(u.i, p_seed);
	}

	uint32_t hash_fmix32(uint32_t h) {
		h ^= h >> 16;
		h *= 0x85ebca6b;
		h ^= h >> 13;
		h *= 0xc2b2ae35;
		h ^= h >> 16;

		return h;
	}
}

float AABB::get_volume() const {
	return size.x * size.y * size.z;
}

void AABB::merge_with(const AABB &p_aabb) {
#ifdef MATH_CHECKS
	if (unlikely(size.x < 0 || size.y < 0 || size.z < 0 || p_aabb.size.x < 0 || p_aabb.size.y < 0 || p_aabb.size.z < 0)) {
		assert("AABB size is negative, this is not supported. Use AABB.abs() to get an AABB with a positive size.");
	}
#endif
	glm::vec3 beg_1, beg_2;
	glm::vec3 end_1, end_2;
	glm::vec3 min, max;

	beg_1 = position;
	beg_2 = p_aabb.position;
	end_1 = size + beg_1;
	end_2 = p_aabb.size + beg_2;

	min.x = (beg_1.x < beg_2.x) ? beg_1.x : beg_2.x;
	min.y = (beg_1.y < beg_2.y) ? beg_1.y : beg_2.y;
	min.z = (beg_1.z < beg_2.z) ? beg_1.z : beg_2.z;

	max.x = (end_1.x > end_2.x) ? end_1.x : end_2.x;
	max.y = (end_1.y > end_2.y) ? end_1.y : end_2.y;
	max.z = (end_1.z > end_2.z) ? end_1.z : end_2.z;

	position = min;
	size = max - min;
}

bool AABB::is_equal_approx(const AABB &p_aabb) const {
	return aabb_math::is_equal_approx(position, p_aabb.position) && aabb_math::is_equal_approx(size, p_aabb.size);
}

bool AABB::is_same(const AABB &p_aabb) const {
	return aabb_math::is_same(position, p_aabb.position) && aabb_math::is_same(size, p_aabb.size);
}

bool AABB::is_finite() const {
	return aabb_math::is_finite(position) && aabb_math::is_finite(size);
}

AABB AABB::intersection(const AABB &p_aabb) const {
#ifdef MATH_CHECKS
	if (unlikely(size.x < 0 || size.y < 0 || size.z < 0 || p_aabb.size.x < 0 || p_aabb.size.y < 0 || p_aabb.size.z < 0)) {
		assert("AABB size is negative, this is not supported. Use AABB.abs() to get an AABB with a positive size.");
	}
#endif
	glm::vec3 src_min = position;
	glm::vec3 src_max = position + size;
	glm::vec3 dst_min = p_aabb.position;
	glm::vec3 dst_max = p_aabb.position + p_aabb.size;

	glm::vec3 min, max;

	if (src_min.x > dst_max.x || src_max.x < dst_min.x) {
		return AABB();
	} else {
		min.x = (src_min.x > dst_min.x) ? src_min.x : dst_min.x;
		max.x = (src_max.x < dst_max.x) ? src_max.x : dst_max.x;
	}

	if (src_min.y > dst_max.y || src_max.y < dst_min.y) {
		return AABB();
	} else {
		min.y = (src_min.y > dst_min.y) ? src_min.y : dst_min.y;
		max.y = (src_max.y < dst_max.y) ? src_max.y : dst_max.y;
	}

	if (src_min.z > dst_max.z || src_max.z < dst_min.z) {
		return AABB();
	} else {
		min.z = (src_min.z > dst_min.z) ? src_min.z : dst_min.z;
		max.z = (src_max.z < dst_max.z) ? src_max.z : dst_max.z;
	}

	return AABB(min, max - min);
}

// Note that this routine returns the BACKTRACKED (i.e. behind the ray origin)
// intersection point + normal if INSIDE the AABB.
// The caller can therefore decide when INSIDE whether to use the
// backtracked intersection, or use p_from as the intersection, and
// carry on progressing without e.g. reflecting against the normal.
bool AABB::find_intersects_ray(const glm::vec3& p_from, const glm::vec3& p_dir, bool &r_inside, glm::vec3* r_intersection_point, glm::vec3* r_normal) const {
#ifdef MATH_CHECKS
	if (unlikely(size.x < 0 || size.y < 0 || size.z < 0)) {
		ERR_PRINT("AABB size is negative, this is not supported. Use AABB.abs() to get an AABB with a positive size.");
	}
#endif
	glm::vec3 end = position + size;
	float tmin = -1e20;
	float tmax = 1e20;
	int axis = 0;

	// Make sure r_inside is always initialized,
	// to prevent reading uninitialized data in the client code.
	r_inside = false;

	for (int i = 0; i < 3; i++) {
		if (p_dir[i] == 0) {
			if ((p_from[i] < position[i]) || (p_from[i] > end[i])) {
				return false;
			}
		} else { // ray not parallel to planes in this direction
			float t1 = (position[i] - p_from[i]) / p_dir[i];
			float t2 = (end[i] - p_from[i]) / p_dir[i];

			if (t1 > t2) {
				std::swap(t1, t2);
			}
			if (t1 >= tmin) {
				tmin = t1;
				axis = i;
			}
			if (t2 < tmax) {
				if (t2 < 0) {
					return false;
				}
				tmax = t2;
			}
			if (tmin > tmax) {
				return false;
			}
		}
	}

	// Did the ray start from inside the box?
	// In which case the intersection returned is the point of entry
	// (behind the ray start) or the calling routine can use the ray origin as intersection point.
	r_inside = tmin < 0;

	if (r_intersection_point) {
		*r_intersection_point = p_from + p_dir * tmin;

		// Prevent float error by making sure the point is exactly
		// on the AABB border on the relevant axis.
		(*r_intersection_point)[axis] = (p_dir[axis] >= 0) ? position[axis] : end[axis];
	}
	if (r_normal) {
		*r_normal = glm::vec3();
		(*r_normal)[axis] = (p_dir[axis] >= 0) ? -1 : 1;
	}

	return true;
}

bool AABB::intersects_segment(const glm::vec3& p_from, const glm::vec3& p_to, glm::vec3* r_intersection_point, glm::vec3* r_normal) const {
#ifdef MATH_CHECKS
	if (unlikely(size.x < 0 || size.y < 0 || size.z < 0)) {
		ERR_PRINT("AABB size is negative, this is not supported. Use AABB.abs() to get an AABB with a positive size.");
	}
#endif
	float min = 0, max = 1;
	int axis = 0;
	float sign = 0;

	for (int i = 0; i < 3; i++) {
		float seg_from = p_from[i];
		float seg_to = p_to[i];
		float box_begin = position[i];
		float box_end = box_begin + size[i];
		float cmin, cmax;
		float csign;

		if (seg_from < seg_to) {
			if (seg_from > box_end || seg_to < box_begin) {
				return false;
			}
			float length = seg_to - seg_from;
			cmin = (seg_from < box_begin) ? ((box_begin - seg_from) / length) : 0;
			cmax = (seg_to > box_end) ? ((box_end - seg_from) / length) : 1;
			csign = -1.0;

		} else {
			if (seg_to > box_end || seg_from < box_begin) {
				return false;
			}
			float length = seg_to - seg_from;
			cmin = (seg_from > box_end) ? (box_end - seg_from) / length : 0;
			cmax = (seg_to < box_begin) ? (box_begin - seg_from) / length : 1;
			csign = 1.0;
		}

		if (cmin > min) {
			min = cmin;
			axis = i;
			sign = csign;
		}
		if (cmax < max) {
			max = cmax;
		}
		if (max < min) {
			return false;
		}
	}

	glm::vec3 rel = p_to - p_from;

	if (r_normal) {
		glm::vec3 normal;
		normal[axis] = sign;
		*r_normal = normal;
	}

	if (r_intersection_point) {
		*r_intersection_point = p_from + rel * min;
	}

	return true;
}

bool AABB::intersects_plane(const Plane &p_plane) const {
	glm::vec3 points[8] = {
		glm::vec3(position.x, position.y, position.z),
		glm::vec3(position.x, position.y, position.z + size.z),
		glm::vec3(position.x, position.y + size.y, position.z),
		glm::vec3(position.x, position.y + size.y, position.z + size.z),
		glm::vec3(position.x + size.x, position.y, position.z),
		glm::vec3(position.x + size.x, position.y, position.z + size.z),
		glm::vec3(position.x + size.x, position.y + size.y, position.z),
		glm::vec3(position.x + size.x, position.y + size.y, position.z + size.z),
	};

	bool over = false;
	bool under = false;

	for (int i = 0; i < 8; i++) {
		if (p_plane.distance_to(points[i]) > 0) {
			over = true;
		} else {
			under = true;
		}
	}

	return under && over;
}

AABB AABB::abs() const {
	return AABB(position + glm::min(size, glm::vec3(0)), glm::abs(size));
}

glm::vec3 AABB::get_longest_axis() const {
	glm::vec3 axis(1, 0, 0);
	float max_size = size.x;

	if (size.y > max_size) {
		axis = glm::vec3(0, 1, 0);
		max_size = size.y;
	}

	if (size.z > max_size) {
		axis = glm::vec3(0, 0, 1);
	}

	return axis;
}

int AABB::get_longest_axis_index() const {
	int axis = 0;
	float max_size = size.x;

	if (size.y > max_size) {
		axis = 1;
		max_size = size.y;
	}

	if (size.z > max_size) {
		axis = 2;
	}

	return axis;
}

glm::vec3 AABB::get_shortest_axis() const {
	glm::vec3 axis(1, 0, 0);
	float min_size = size.x;

	if (size.y < min_size) {
		axis = glm::vec3(0, 1, 0);
		min_size = size.y;
	}

	if (size.z < min_size) {
		axis = glm::vec3(0, 0, 1);
	}

	return axis;
}

int AABB::get_shortest_axis_index() const {
	int axis = 0;
	float min_size = size.x;

	if (size.y < min_size) {
		axis = 1;
		min_size = size.y;
	}

	if (size.z < min_size) {
		axis = 2;
	}

	return axis;
}

AABB AABB::merge(const AABB &p_with) const {
	AABB aabb = *this;
	aabb.merge_with(p_with);
	return aabb;
}

AABB AABB::expand(const glm::vec3& p_vector) const {
	AABB aabb = *this;
	aabb.expand_to(p_vector);
	return aabb;
}

AABB AABB::grow(float p_by) const {
	AABB aabb = *this;
	aabb.grow_by(p_by);
	return aabb;
}

void AABB::get_edge(int p_edge, glm::vec3& r_from, glm::vec3& r_to) const {
	assert(p_edge < 12);
	switch (p_edge) {
		case 0: {
			r_from = glm::vec3(position.x + size.x, position.y, position.z);
			r_to = glm::vec3(position.x, position.y, position.z);
		} break;
		case 1: {
			r_from = glm::vec3(position.x + size.x, position.y, position.z + size.z);
			r_to = glm::vec3(position.x + size.x, position.y, position.z);
		} break;
		case 2: {
			r_from = glm::vec3(position.x, position.y, position.z + size.z);
			r_to = glm::vec3(position.x + size.x, position.y, position.z + size.z);

		} break;
		case 3: {
			r_from = glm::vec3(position.x, position.y, position.z);
			r_to = glm::vec3(position.x, position.y, position.z + size.z);

		} break;
		case 4: {
			r_from = glm::vec3(position.x, position.y + size.y, position.z);
			r_to = glm::vec3(position.x + size.x, position.y + size.y, position.z);
		} break;
		case 5: {
			r_from = glm::vec3(position.x + size.x, position.y + size.y, position.z);
			r_to = glm::vec3(position.x + size.x, position.y + size.y, position.z + size.z);
		} break;
		case 6: {
			r_from = glm::vec3(position.x + size.x, position.y + size.y, position.z + size.z);
			r_to = glm::vec3(position.x, position.y + size.y, position.z + size.z);

		} break;
		case 7: {
			r_from = glm::vec3(position.x, position.y + size.y, position.z + size.z);
			r_to = glm::vec3(position.x, position.y + size.y, position.z);

		} break;
		case 8: {
			r_from = glm::vec3(position.x, position.y, position.z + size.z);
			r_to = glm::vec3(position.x, position.y + size.y, position.z + size.z);

		} break;
		case 9: {
			r_from = glm::vec3(position.x, position.y, position.z);
			r_to = glm::vec3(position.x, position.y + size.y, position.z);

		} break;
		case 10: {
			r_from = glm::vec3(position.x + size.x, position.y, position.z);
			r_to = glm::vec3(position.x + size.x, position.y + size.y, position.z);

		} break;
		case 11: {
			r_from = glm::vec3(position.x + size.x, position.y, position.z + size.z);
			r_to = glm::vec3(position.x + size.x, position.y + size.y, position.z + size.z);

		} break;
	}
}

/*Variant AABB::intersects_segment_bind(const Vector3& p_from, const Vector3& p_to) const {
	Vector3 inters;
	if (intersects_segment(p_from, p_to, &inters)) {
		return inters;
	}
	return Variant();
}

Variant AABB::intersects_ray_bind(const Vector3 &p_from, const Vector3 &p_dir) const {
	Vector3 inters;
	bool inside = false;

	if (find_intersects_ray(p_from, p_dir, inside, &inters)) {
		// When inside the intersection point may be BEHIND the ray,
		// so for general use we return the ray origin.
		if (inside) {
			return p_from;
		}

		return inters;
	}
	return Variant();
}*/

void AABB::quantize(float p_unit) {
	size += position;

	position.x -= aabb_math::fposmodp(position.x, p_unit);
	position.y -= aabb_math::fposmodp(position.y, p_unit);
	position.z -= aabb_math::fposmodp(position.z, p_unit);

	size.x -= aabb_math::fposmodp(size.x, p_unit);
	size.y -= aabb_math::fposmodp(size.y, p_unit);
	size.z -= aabb_math::fposmodp(size.z, p_unit);

	size.x += p_unit;
	size.y += p_unit;
	size.z += p_unit;

	size -= position;
}

AABB AABB::quantized(float p_unit) const {
	AABB ret = *this;
	ret.quantize(p_unit);
	return ret;
}

inline uint32_t AABB::hash() const
{
	uint32_t h = aabb_math::hash_murmur3_one_float(position.x);
	h = aabb_math::hash_murmur3_one_float(position.y, h);
	h = aabb_math::hash_murmur3_one_float(position.z, h);
	h = aabb_math::hash_murmur3_one_float(size.x, h);
	h = aabb_math::hash_murmur3_one_float(size.y, h);
	h = aabb_math::hash_murmur3_one_float(size.z, h);
	return aabb_math::hash_fmix32(h);
}

inline bool AABB::intersects(const AABB& p_aabb) const {
#ifdef MATH_CHECKS
	if (unlikely(size.x < 0 || size.y < 0 || size.z < 0 || p_aabb.size.x < 0 || p_aabb.size.y < 0 || p_aabb.size.z < 0)) {
		ERR_PRINT("AABB size is negative, this is not supported. Use AABB.abs() to get an AABB with a positive size.");
	}
#endif
	if (position.x >= (p_aabb.position.x + p_aabb.size.x)) {
		return false;
	}
	if ((position.x + size.x) <= p_aabb.position.x) {
		return false;
	}
	if (position.y >= (p_aabb.position.y + p_aabb.size.y)) {
		return false;
	}
	if ((position.y + size.y) <= p_aabb.position.y) {
		return false;
	}
	if (position.z >= (p_aabb.position.z + p_aabb.size.z)) {
		return false;
	}
	if ((position.z + size.z) <= p_aabb.position.z) {
		return false;
	}

	return true;
}

inline bool AABB::intersects_inclusive(const AABB& p_aabb) const {
#ifdef MATH_CHECKS
	if (unlikely(size.x < 0 || size.y < 0 || size.z < 0 || p_aabb.size.x < 0 || p_aabb.size.y < 0 || p_aabb.size.z < 0)) {
		ERR_PRINT("AABB size is negative, this is not supported. Use AABB.abs() to get an AABB with a positive size.");
	}
#endif
	if (position.x > (p_aabb.position.x + p_aabb.size.x)) {
		return false;
	}
	if ((position.x + size.x) < p_aabb.position.x) {
		return false;
	}
	if (position.y > (p_aabb.position.y + p_aabb.size.y)) {
		return false;
	}
	if ((position.y + size.y) < p_aabb.position.y) {
		return false;
	}
	if (position.z > (p_aabb.position.z + p_aabb.size.z)) {
		return false;
	}
	if ((position.z + size.z) < p_aabb.position.z) {
		return false;
	}

	return true;
}

inline bool AABB::encloses(const AABB& p_aabb) const {
#ifdef MATH_CHECKS
	if (unlikely(size.x < 0 || size.y < 0 || size.z < 0 || p_aabb.size.x < 0 || p_aabb.size.y < 0 || p_aabb.size.z < 0)) {
		ERR_PRINT("AABB size is negative, this is not supported. Use AABB.abs() to get an AABB with a positive size.");
	}
#endif
	glm::vec3 src_min = position;
	glm::vec3 src_max = position + size;
	glm::vec3 dst_min = p_aabb.position;
	glm::vec3 dst_max = p_aabb.position + p_aabb.size;

	return (
		(src_min.x <= dst_min.x) &&
		(src_max.x >= dst_max.x) &&
		(src_min.y <= dst_min.y) &&
		(src_max.y >= dst_max.y) &&
		(src_min.z <= dst_min.z) &&
		(src_max.z >= dst_max.z));
}

glm::vec3 AABB::get_support(const glm::vec3& p_direction) const {
	glm::vec3 support = position;
	if (p_direction.x > 0.0f) {
		support.x += size.x;
	}
	if (p_direction.y > 0.0f) {
		support.y += size.y;
	}
	if (p_direction.z > 0.0f) {
		support.z += size.z;
	}
	return support;
}

glm::vec3 AABB::get_endpoint(int p_point) const {
	assert(p_point < 8);
	switch (p_point) {
	case 0:
		return glm::vec3(position.x, position.y, position.z);
	case 1:
		return glm::vec3(position.x, position.y, position.z + size.z);
	case 2:
		return glm::vec3(position.x, position.y + size.y, position.z);
	case 3:
		return glm::vec3(position.x, position.y + size.y, position.z + size.z);
	case 4:
		return glm::vec3(position.x + size.x, position.y, position.z);
	case 5:
		return glm::vec3(position.x + size.x, position.y, position.z + size.z);
	case 6:
		return glm::vec3(position.x + size.x, position.y + size.y, position.z);
	case 7:
		return glm::vec3(position.x + size.x, position.y + size.y, position.z + size.z);
	}

	return glm::vec3();
}

bool AABB::intersects_convex_shape(const Plane* p_planes, int p_plane_count, const glm::vec3* p_points, int p_point_count) const {
	glm::vec3 half_extents = size * 0.5f;
	glm::vec3 ofs = position + half_extents;

	for (int i = 0; i < p_plane_count; i++) {
		const Plane& p = p_planes[i];
		glm::vec3 point(
			(p.normal.x > 0) ? -half_extents.x : half_extents.x,
			(p.normal.y > 0) ? -half_extents.y : half_extents.y,
			(p.normal.z > 0) ? -half_extents.z : half_extents.z);
		point += ofs;
		if (p.is_point_over(point)) {
			return false;
		}
	}

	// Make sure all points in the shape aren't fully separated from the AABB on
	// each axis.
	int bad_point_counts_positive[3] = { 0 };
	int bad_point_counts_negative[3] = { 0 };

	for (int k = 0; k < 3; k++) {
		for (int i = 0; i < p_point_count; i++) {
			if (p_points[i][k] > ofs[k] + half_extents[k]) {
				bad_point_counts_positive[k]++;
			}
			if (p_points[i][k] < ofs[k] - half_extents[k]) {
				bad_point_counts_negative[k]++;
			}
		}

		if (bad_point_counts_negative[k] == p_point_count) {
			return false;
		}
		if (bad_point_counts_positive[k] == p_point_count) {
			return false;
		}
	}

	return true;
}

bool AABB::inside_convex_shape(const Plane* p_planes, int p_plane_count) const {
	glm::vec3 half_extents = size * 0.5f;
	glm::vec3 ofs = position + half_extents;

	for (int i = 0; i < p_plane_count; i++) {
		const Plane& p = p_planes[i];
		glm::vec3 point(
			(p.normal.x < 0) ? -half_extents.x : half_extents.x,
			(p.normal.y < 0) ? -half_extents.y : half_extents.y,
			(p.normal.z < 0) ? -half_extents.z : half_extents.z);
		point += ofs;
		if (p.is_point_over(point)) {
			return false;
		}
	}

	return true;
}

bool AABB::has_point(const glm::vec3& p_point) const {
#ifdef MATH_CHECKS
	if (unlikely(size.x < 0 || size.y < 0 || size.z < 0)) {
		ERR_PRINT("AABB size is negative, this is not supported. Use AABB.abs() to get an AABB with a positive size.");
	}
#endif
	if (p_point.x < position.x) {
		return false;
	}
	if (p_point.y < position.y) {
		return false;
	}
	if (p_point.z < position.z) {
		return false;
	}
	if (p_point.x > position.x + size.x) {
		return false;
	}
	if (p_point.y > position.y + size.y) {
		return false;
	}
	if (p_point.z > position.z + size.z) {
		return false;
	}

	return true;
}

inline void AABB::expand_to(const glm::vec3& p_vector) {
#ifdef MATH_CHECKS
	if (unlikely(size.x < 0 || size.y < 0 || size.z < 0)) {
		ERR_PRINT("AABB size is negative, this is not supported. Use AABB.abs() to get an AABB with a positive size.");
	}
#endif
	glm::vec3 begin = position;
	glm::vec3 end = position + size;

	if (p_vector.x < begin.x) {
		begin.x = p_vector.x;
	}
	if (p_vector.y < begin.y) {
		begin.y = p_vector.y;
	}
	if (p_vector.z < begin.z) {
		begin.z = p_vector.z;
	}

	if (p_vector.x > end.x) {
		end.x = p_vector.x;
	}
	if (p_vector.y > end.y) {
		end.y = p_vector.y;
	}
	if (p_vector.z > end.z) {
		end.z = p_vector.z;
	}

	position = begin;
	size = end - begin;
}

void AABB::project_range_in_plane(const Plane& p_plane, float& r_min, float& r_max) const {
	glm::vec3 half_extents(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f);
	glm::vec3 center(position.x + half_extents.x, position.y + half_extents.y, position.z + half_extents.z);

	glm::vec3 abs_norm = glm::abs(p_plane.normal);
	float length = glm::dot(abs_norm, half_extents);
	float distance = p_plane.distance_to(center);
	r_min = distance - length;
	r_max = distance + length;
}

inline float AABB::get_longest_axis_size() const {
	float max_size = size.x;

	if (size.y > max_size) {
		max_size = size.y;
	}

	if (size.z > max_size) {
		max_size = size.z;
	}

	return max_size;
}

inline float AABB::get_shortest_axis_size() const {
	float max_size = size.x;

	if (size.y < max_size) {
		max_size = size.y;
	}

	if (size.z < max_size) {
		max_size = size.z;
	}

	return max_size;
}

bool AABB::smits_intersect_ray(const glm::vec3& p_from, const glm::vec3& p_dir, float p_t0, float p_t1) const {
#ifdef MATH_CHECKS
	if (unlikely(size.x < 0 || size.y < 0 || size.z < 0)) {
		ERR_PRINT("AABB size is negative, this is not supported. Use AABB.abs() to get an AABB with a positive size.");
	}
#endif
	float divx = 1.0f / p_dir.x;
	float divy = 1.0f / p_dir.y;
	float divz = 1.0f / p_dir.z;

	glm::vec3 upbound = position + size;
	float tmin, tmax, tymin, tymax, tzmin, tzmax;
	if (p_dir.x >= 0) {
		tmin = (position.x - p_from.x) * divx;
		tmax = (upbound.x - p_from.x) * divx;
	}
	else {
		tmin = (upbound.x - p_from.x) * divx;
		tmax = (position.x - p_from.x) * divx;
	}
	if (p_dir.y >= 0) {
		tymin = (position.y - p_from.y) * divy;
		tymax = (upbound.y - p_from.y) * divy;
	}
	else {
		tymin = (upbound.y - p_from.y) * divy;
		tymax = (position.y - p_from.y) * divy;
	}
	if ((tmin > tymax) || (tymin > tmax)) {
		return false;
	}
	if (tymin > tmin) {
		tmin = tymin;
	}
	if (tymax < tmax) {
		tmax = tymax;
	}
	if (p_dir.z >= 0) {
		tzmin = (position.z - p_from.z) * divz;
		tzmax = (upbound.z - p_from.z) * divz;
	}
	else {
		tzmin = (upbound.z - p_from.z) * divz;
		tzmax = (position.z - p_from.z) * divz;
	}
	if ((tmin > tzmax) || (tzmin > tmax)) {
		return false;
	}
	if (tzmin > tmin) {
		tmin = tzmin;
	}
	if (tzmax < tmax) {
		tmax = tzmax;
	}
	return ((tmin < p_t1) && (tmax > p_t0));
}

void AABB::grow_by(float p_amount) {
	position.x -= p_amount;
	position.y -= p_amount;
	position.z -= p_amount;
	size.x += 2.0f * p_amount;
	size.y += 2.0f * p_amount;
	size.z += 2.0f * p_amount;
}


void AABB::DebugDraw(glm::vec3 color, float duration)
{
	for (int i = 0; i < 12; i++) {
		glm::vec3 from; 
		glm::vec3 to;
		get_edge(i, from, to);
		Graphics::DrawDebugLine(from, to, color, duration);
	}
}

bool AABB::isOnOrForwardPlane(const Plane& plane) const
{
	// Compute the projection interval radius of b onto L(t) = b.c + t * p.n
	const float r = size.x * std::abs(plane.normal.x) +
		size.y * std::abs(plane.normal.y) + size.z * std::abs(plane.normal.z);

	return -r <= plane.getSignedDistanceToPlane(position);
}

bool AABB::isOnFrustum(const Frustum& camFrustum) const
{
	bool is_on_left = isOnOrForwardPlane(camFrustum.leftFace);
	bool is_on_right = isOnOrForwardPlane(camFrustum.rightFace);
	bool is_on_top = isOnOrForwardPlane(camFrustum.topFace);
	bool is_on_bottom = isOnOrForwardPlane(camFrustum.bottomFace);
	bool is_on_near = isOnOrForwardPlane(camFrustum.nearFace);
	bool is_on_far = isOnOrForwardPlane(camFrustum.farFace);

	//Logger::LogDebug(LOG_POS("isOnFrustum"), "left: %d, right: %d \t top: %d, bottom: %d \t near: %d, far: %d",
	//	is_on_left, is_on_right, is_on_top, is_on_bottom, is_on_near, is_on_far);

	return (is_on_left &&
			is_on_right &&
			is_on_top &&
			is_on_bottom &&
			is_on_near &&
			is_on_far);
}

/*AABB::operator String() const {
	return "[P: " + String(position) + ", S: " + String(size) + "]";
}*/
