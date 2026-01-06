#pragma once

#include <glm/glm.hpp>
#include <type_traits>

// Borrowed from https://github.com/godotengine/godot/blob/master/core/math/plane.h

#define CMP_EPSILON 0.00001

namespace BoundsVolume
{

	enum ClockDirection {
		CLOCKWISE,
		COUNTERCLOCKWISE
	};

	class Variant;

	struct Plane {
		static const Plane PLANE_YZ;
		static const Plane PLANE_XZ;
		static const Plane PLANE_XY;

		glm::vec3 normal;
		float d = 0;

		void set_normal(const glm::vec3& p_normal);
		glm::vec3 get_normal() const { return normal; }

		void normalize();
		Plane normalized() const;

		/* Plane-Point operations */

		glm::vec3 get_center() const { return normal * d; }
		glm::vec3 get_any_perpendicular_normal() const;

		bool is_point_over(const glm::vec3& p_point) const; ///< Point is over plane
		float distance_to(const glm::vec3& p_point) const;
		bool has_point(const glm::vec3& p_point, float p_tolerance = CMP_EPSILON) const;

		/* intersections */

		bool intersect_3(const Plane& p_plane1, const Plane& p_plane2, glm::vec3* r_result = nullptr) const;
		bool intersects_ray(const glm::vec3& p_from, const glm::vec3& p_dir, glm::vec3* p_intersection) const;
		bool intersects_segment(const glm::vec3& p_begin, const glm::vec3& p_end, glm::vec3* p_intersection) const;

		// For Variant bindings.
		//Variant intersect_3_bind(const Plane &p_plane1, const Plane &p_plane2) const;
		//Variant intersects_ray_bind(const glm::vec3& p_from, const glm::vec3& p_dir) const;
		//Variant intersects_segment_bind(const glm::vec3& p_begin, const glm::vec3& p_end) const;

		glm::vec3 project(const glm::vec3& p_point) const {
			return p_point - normal * distance_to(p_point);
		}

		/* misc */

		constexpr Plane operator-() const { return Plane(-normal, -d); }
		bool is_equal_approx(const Plane& p_plane) const;
		bool is_same(const Plane& p_plane) const;
		bool is_equal_approx_any_side(const Plane& p_plane) const;
		bool is_finite() const;

		constexpr bool operator==(const Plane& p_plane) const;
		constexpr bool operator!=(const Plane& p_plane) const;
		//explicit operator String() const;

		Plane() = default;
		constexpr Plane(double p_a, double p_b, double p_c, double p_d) :
			normal(p_a, p_b, p_c),
			d(p_d) {
		}

		constexpr Plane(const glm::vec3& p_normal, float p_d = 0.0);
		Plane(const glm::vec3& p_normal, const glm::vec3& p_point);
		Plane(const glm::vec3& p_point1, const glm::vec3& p_point2, const glm::vec3& p_point3, ClockDirection p_dir = CLOCKWISE);
	};

	
}

//template <>
//struct is_zero_constructible<Plane> : std::true_type {};
