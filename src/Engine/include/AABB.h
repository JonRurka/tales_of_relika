#pragma once

//#include "core/math/plane.h"
//#include "core/math/vector3.h"
//#include "core/templates/hashfuncs.h"

#include "Plane.h"
#include <glm/glm.hpp>

#include "BoundingVolume.h"
#include "Logger.h"


/**
 * AABB (Axis Aligned Bounding Box)
 * This is implemented by a point (position) and the box size.
 */

namespace BoundsVolume
{

	class Variant;

#ifndef _FORCE_INLINE_
#define _FORCE_INLINE_
#endif

	struct AABB : public BoundingVolume {
		glm::vec3 position{glm::vec3(0)};
		glm::vec3 size{glm::vec3(0)};

		float get_volume() const;
		_FORCE_INLINE_ bool has_volume() const {
			return size.x > 0.0f && size.y > 0.0f && size.z > 0.0f;
		}

		_FORCE_INLINE_ bool has_surface() const {
			return size.x > 0.0f || size.y > 0.0f || size.z > 0.0f;
		}

		const glm::vec3& get_position() const { return position; }
		void set_position(const glm::vec3& p_pos) { position = p_pos; }
		const glm::vec3& get_size() const { return size; }
		void set_size(const glm::vec3& p_size) { size = p_size; }

		constexpr bool operator==(const AABB& p_rval) const {
			return position == p_rval.position && size == p_rval.size;
		}
		constexpr bool operator!=(const AABB& p_rval) const {
			return position != p_rval.position || size != p_rval.size;
		}

		bool is_equal_approx(const AABB& p_aabb) const;
		bool is_same(const AABB& p_aabb) const;
		bool is_finite() const;
		_FORCE_INLINE_ bool intersects(const AABB& p_aabb) const; /// Both AABBs overlap
		_FORCE_INLINE_ bool intersects_inclusive(const AABB& p_aabb) const; /// Both AABBs (or their faces) overlap
		_FORCE_INLINE_ bool encloses(const AABB& p_aabb) const; /// p_aabb is completely inside this

		AABB merge(const AABB& p_with) const;
		void merge_with(const AABB& p_aabb); ///merge with another AABB
		AABB intersection(const AABB& p_aabb) const; ///get box where two intersect, empty if no intersection occurs
		_FORCE_INLINE_ bool smits_intersect_ray(const glm::vec3& p_from, const glm::vec3& p_dir, float p_t0, float p_t1) const;

		bool intersects_segment(const glm::vec3& p_from, const glm::vec3& p_to, glm::vec3* r_intersection_point = nullptr, glm::vec3* r_normal = nullptr) const;
		bool intersects_ray(const glm::vec3& p_from, const glm::vec3& p_dir) const {
			bool inside;
			return find_intersects_ray(p_from, p_dir, inside);
		}
		bool find_intersects_ray(const glm::vec3& p_from, const glm::vec3& p_dir, bool& r_inside, glm::vec3* r_intersection_point = nullptr, glm::vec3* r_normal = nullptr) const;

		_FORCE_INLINE_ bool intersects_convex_shape(const Plane* p_planes, int p_plane_count, const glm::vec3* p_points, int p_point_count) const;
		_FORCE_INLINE_ bool inside_convex_shape(const Plane* p_planes, int p_plane_count) const;
		bool intersects_plane(const Plane& p_plane) const;

		_FORCE_INLINE_ bool has_point(const glm::vec3& p_point) const;
		_FORCE_INLINE_ glm::vec3 get_support(const glm::vec3& p_direction) const;

		glm::vec3 get_longest_axis() const;
		int get_longest_axis_index() const;
		_FORCE_INLINE_ float get_longest_axis_size() const;

		glm::vec3 get_shortest_axis() const;
		int get_shortest_axis_index() const;
		_FORCE_INLINE_ float get_shortest_axis_size() const;

		AABB grow(float p_by) const;
		_FORCE_INLINE_ void grow_by(float p_amount);

		void get_edge(int p_edge, glm::vec3& r_from, glm::vec3& r_to) const;
		_FORCE_INLINE_ glm::vec3 get_endpoint(int p_point) const;

		AABB expand(const glm::vec3& p_vector) const;
		_FORCE_INLINE_ void project_range_in_plane(const Plane& p_plane, float& r_min, float& r_max) const;
		_FORCE_INLINE_ void expand_to(const glm::vec3& p_vector); /** expand to contain a point if necessary */

		_FORCE_INLINE_ AABB abs() const; /* {
			return AABB(position + size.minf(0), size.abs());
		}*/

		//Variant intersects_segment_bind(const glm::vec3& p_from, const glm::vec3& p_to) const;
		//Variant intersects_ray_bind(const glm::vec3& p_from, const glm::vec3& p_dir) const;

		_FORCE_INLINE_ void quantize(float p_unit);
		_FORCE_INLINE_ AABB quantized(float p_unit) const;

		_FORCE_INLINE_ void set_end(const glm::vec3& p_end) {
			size = p_end - position;
		}

		_FORCE_INLINE_ glm::vec3 get_end() const {
			return position + size;
		}

		_FORCE_INLINE_ glm::vec3 get_center() const {
			return position + (size * 0.5f);
		}

		uint32_t hash() const;

		//explicit operator String() const;

		AABB() = default;
		constexpr AABB(const glm::vec3& p_pos, const glm::vec3& p_size) :
			position(p_pos),
			size(p_size) {
		}

		void DebugDraw(glm::vec3 color, float duration);

		bool isOnOrForwardPlane(const Plane& plane) const override;

		bool isOnFrustum(const Frustum& camFrustum) const override;

	private:
		inline static const std::string LOG_LOC{ "AABB" };
	};

	
}


//template <>
//struct is_zero_constructible<AABB> : std::true_type {};
