#ifndef VERTEX_ABSTRACT_H
#define VERTEX_ABSTRACT_H

#include <typeinfo>
#include <glm/common.hpp>

namespace SmartVertex
{
	template <typename... Ts>
	struct v_tup {};

	template <typename T, typename... Ts>
	struct v_tup<T, Ts...> : v_tup<Ts...>
	{
		v_tup(T t, Ts... ts) : v_tup<Ts...>(ts...), tail(t) {};

		T tail;
	};

	namespace detail
	{
		template <size_t, typename> struct tup_elem_type;

		template <typename T, typename... Ts>
		struct tup_elem_type<0, v_tup<T, Ts... >>
		{
			typedef T type;
		};

		template <size_t k, typename T, typename... Ts>
		struct tup_elem_type<k, v_tup<T, Ts...>>
		{
			typedef typename tup_elem_type<k - 1, v_tup<Ts...>>::type type;
		};
	}

	template <size_t k, typename... Ts>
	typename std::enable_if<k == 0, typename detail::tup_elem_type<0, v_tup<Ts...>>::type&>::type
	get(v_tup<Ts...>& t)
	{
		return t.tail;
	};

	template < size_t k, typename T, typename... Ts >
	typename std::enable_if< k != 0, typename detail::tup_elem_type< k, v_tup< T, Ts...> >::type& >::type
	get(v_tup<T, Ts...>& t)
	{
		v_tup<Ts...>& base = t;
		return get<k - 1>(base);
	};

	template <typename T, typename... Ts>
	constexpr size_t
	stride(v_tup<T, Ts...>& t)
	{
		return sizeof(t);
	};

	namespace detail
	{
		template <size_t k, typename T, typename... Ts>
		constexpr typename std::enable_if<k == 0, size_t>::type
		offset(v_tup<T, Ts...>& t)
		{
			return 0;
		};

		template <size_t k, typename T, typename... Ts>
		constexpr typename std::enable_if<k != 0, size_t>::type
		offset(v_tup<T, Ts...>& t)
		{
			return sizeof(T) + detail::offset<k - 1>(v_tup<Ts...>(t));
		};
	}

	template <bool reversed>
	struct offset_viewer
	{};

	template <>
	struct offset_viewer<false>
	{
		offset_viewer() = default;

		template <size_t k, typename... Ts>
		constexpr size_t get(v_tup<Ts...>& t)
		{
			return detail::offset<k>(t);
		};
	};

	namespace detail
	{
		template <size_t k, typename T, typename... Ts>
		constexpr typename std::enable_if<k == 0, size_t>::type
		offset_rev_(v_tup<T, Ts...>& t)
		{
			return sizeof(T);
		};

		template <size_t k, typename T, typename... Ts>
		constexpr typename std::enable_if<k != 0, size_t>::type
		offset_rev_(v_tup<T, Ts...>& t)
		{
			return sizeof(T) + detail::offset_rev_<k - 1>(v_tup<Ts...>(t));
		};

		template <size_t k, typename T, typename... Ts>
		constexpr typename std::enable_if<k == 0, size_t>::type
		offset_rev(v_tup<T, Ts...>& t)
		{
			return sizeof(v_tup<Ts...>);
		};

		template <size_t k, typename... Ts>
		constexpr typename std::enable_if<k != 0, size_t>::type
		offset_rev(v_tup<Ts...>& t)
		{
			return sizeof(v_tup<Ts...>) - detail::offset_rev_<k>(t);
		};
	}

	template <>
	struct offset_viewer<true>
	{
		offset_viewer() = default;

		template <size_t k, typename... Ts>
		constexpr size_t get(v_tup<Ts...>& t)
		{
			return detail::offset_rev<k>(t);
		};
	};
}

namespace SmartVertex
{
	/// Vertex which has only a position vector.
	using SimpleVertex = v_tup<glm::vec3>;
	/// Vertex which has position and normal vectors.
	using NormalVertex = v_tup<glm::vec3, glm::vec3>;
	/// Vertex which has position, normal, and tangent vectors.
	using NTVertex = v_tup<glm::vec3, glm::vec3, glm::vec3>;
	/// Vertex which has position, normal, and texture (UV) vectors.
	using TexturedNormalVertex = v_tup<glm::vec3, glm::vec3, glm::vec2>;
	/// Vertex which has position, normal, tangent, and texture (UV) vectors.
	using TexturedNTVertex = v_tup<glm::vec3, glm::vec3, glm::vec3, glm::vec2>;

	/// Reversed layout viewer for attribute offsets.
	using ReversedViewer = offset_viewer<true>;
	/// Standard layout viewer for attribute offsets.
	using StandardViewer = offset_viewer<false>;
}

#endif