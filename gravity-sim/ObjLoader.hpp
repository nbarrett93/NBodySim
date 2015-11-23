#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#define NOMINMAX

#include <cstdint>
#include <vector>
#include <sstream>
#include <limits>
#include <utility>
#include <algorithm>
#include <map>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <GL/glew.h>

template <size_t T>
struct Face
{
	uint16_t Vertices[T];
};

template <size_t T = 3>
struct Model
{
	std::vector<uint16_t> Indices;
	std::vector<glm::vec4> Vertices;
	std::vector<glm::vec3> Normals;
	std::vector<glm::vec2> TexCoords;

	Model()
	{
	};
	Model(const Model<T> &rhs) = default;
	Model& operator=(const Model<T> &rhs) = default;
	Model(Model &&rhs);
	~Model();

	static Model<T> FromStream(std::istream &s);
	static Model<T> FromString(const std::string &s);
	static Model<T> FromCStr(const char *s);
};

template <size_t T>
Model<T>::~Model()
{
}

template <size_t T>
Model<T>::Model(Model<T> &&rhs) :
	Indices(std::move(rhs.Indices)),
	Vertices(std::move(rhs.Vertices)),
	Normals(std::move(rhs.Normals)),
	TexCoords(std::move(rhs.TexCoords))
{
}

template <size_t T = 3>
Model<T> Model<T>::FromStream(std::istream &s)
{
	struct Face_
	{
		uint16_t v[T];
		uint16_t vt[T];
		uint16_t vn[T];
	};

	std::vector<glm::vec4> GeomVerts;
	std::vector<glm::vec2> TexCoords;
	std::vector<glm::vec3> Normals;
	std::vector<glm::vec3> ParameterVerts;
	std::vector<Face_> Faces;

	std::string buf;
	float tmp[4];

	// Parsing code
	while (!s.eof() && s.good())
	{
		tmp[0] = 0.0f; tmp[1] = 0.0f; tmp[2] = 0.0f;
		switch (s.get())
		{
		case '#':
			s.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			break;
		case 'v':

			switch (s.get())
			{
			case ' ':
				// vertex coord follows
				tmp[3] = 1.0f;
				s >> tmp[0];
				s >> tmp[1];
				s >> tmp[2];
				while (s.peek() == ' ')
					s.ignore(1);
				if (s.peek() == '\r')
					s.ignore(2);
				else if (s.peek() == '\n')
					s.ignore(1);
				else
				{
					s >> tmp[3];
					s.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				}
				GeomVerts.emplace_back(glm::vec4(tmp[0], tmp[1], tmp[2], tmp[3]));
				break;
			case 't':
				// texture coord follows
				tmp[2] = 0.0f;
				s >> tmp[0];
				s >> tmp[1];
				while (s.peek() == ' ')
					s.ignore(1);
				if (s.peek() == '\r')
					s.ignore(2);
				else if (s.peek() == '\n')
					s.ignore(1);
				else
				{
					s >> tmp[2];
					s.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				}
				TexCoords.emplace_back(glm::vec2(tmp[0], tmp[1]));
				break;
			case 'n':
				// normal coord follows
				s >> tmp[0];
				s >> tmp[1];
				s >> tmp[2];
				s.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				Normals.emplace_back(glm::vec3(tmp[0], tmp[1], tmp[2]));
				break;
			case 'p':
				// parameter coord follows
				s >> tmp[0];
				while (s.peek() == ' ')
					s.ignore(1);
				if (s.peek() == '\r')
					s.ignore(2);
				else if (s.peek() == '\n')
					s.ignore(1);
				else
				{
					s >> tmp[1];
					
					while (s.peek() == ' ')
						s.ignore(1);
					if (s.peek() == '\r')
						s.ignore(2);
					else if (s.peek() == '\n')
						s.ignore(1);
					else
					{
						s >> tmp[2];
						s.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					}
				}
				ParameterVerts.emplace_back(glm::vec3(tmp[0], tmp[1], tmp[2]));
				break;
			default:
				s.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				break;
			}

			break;
		case 'f':
			// face follows
			Face_ face;
			while (s.peek() == ' ')
				s.ignore(1);
			if (s.peek() == '\r' || s.peek() == '\n')
			{
				s.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				break;
			}
			for (size_t i = 0; i < T; ++i)
			{
				while (s.peek() == ' ')
					s.ignore(1);
				if (s.peek() == '\r' || s.peek() == '\n')
				{
					s.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					break;
				}
				// should have a form like: m OR m/n OR m/n/p OR m//p
				std::string tmp_str;
				while (s.peek() != ' ' && s.peek() != '\r' && s.peek() != '\n')
					tmp_str += s.get();
				const char *tmp_str_ = tmp_str.c_str();
				char *endptr = NULL;

				face.v[i] = (uint16_t)strtol(tmp_str_, &endptr, 10) - 1;
				if (*endptr != '\0')
				{
					// skip the slash
					++endptr;
					if (*endptr == '/')
					{
						// we had a double slash -> v//vn
						++endptr;
						face.vn[i] = (uint16_t)strtol(endptr, NULL, 10) - 1;
					}
					else
					{
						// we have at least m/n
						face.vt[i] = (uint16_t)strtol(endptr, &endptr, 10) - 1;
						if (*endptr == '/')
						{
							// we have m/n/p
							++endptr;
							face.vn[i] = (uint16_t)strtol(endptr, NULL, 10) - 1;
						}
					}
				}
			}
			if (s.peek() == '\r' || s.peek() == '\n')
			{
				s.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				//break;
			}
			Faces.push_back(face);
			break;
		default:
			s.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			break;
		}
	}

	Model<T> ret;

	// vertex index in old array
	typedef uint16_t KeyType;
	// Old arrays: normal, texture indices
	// New array index
	typedef std::tuple<uint16_t, uint16_t, uint16_t> ValType;

	std::multimap<KeyType, ValType> cache;

	bool have_verts = GeomVerts.size() != 0;
	bool have_norm = Normals.size() != 0;
	bool have_tex = TexCoords.size() != 0;

	uint16_t ix = 0;
	for (auto &face : Faces)
	{
		// foreach (Vert v : f)
		for (uint32_t j = 0; j < T; ++j)
		{
			auto pos_i = face.v[j];
			auto norm_i = face.vn[j];
			auto tex_i = face.vt[j];

			auto new_vertex = [&]() {
				ret.Indices.push_back(ix);

				if (have_verts)
					ret.Vertices.push_back(GeomVerts[pos_i]);
				if (have_norm)
					ret.Normals.push_back(Normals[norm_i]);
				if (have_tex)
					ret.TexCoords.push_back(TexCoords[tex_i]);

				cache.insert(std::make_pair(
					face.v[j],
					std::make_tuple(norm_i, tex_i, ix++)
					)
				);
			};

			auto it_pair = cache.equal_range(pos_i);
			if (std::get<0>(it_pair) == cache.end())
			{
				new_vertex();
			}
			else
			{
				// it might be a re-usable vertex
				// we have to check all vertices in the cache that have this index

				auto &beg = it_pair.first;
				auto &end = it_pair.second;

				auto res = std::find_if(beg, end, [&](auto &tpl) {
					// value of the K-V element
					auto &snd = std::get<1>(tpl);
					// normal index & texture index
					return std::get<0>(snd) == norm_i && std::get<1>(snd) == tex_i;
				});

				if (res != end)
					ret.Indices.push_back(std::get<2>(std::get<1>(*res)));
				else
					new_vertex();
			}
		}
	}

	return ret;
}

template <size_t T = 3>
Model<T> Model<T>::FromString(const std::string &s)
{
	std::stringstream ss;
	ss << s;
	return Model<T>::FromStream(ss);
}

template <size_t T = 3>
Model<T> Model<T>::FromCStr(const char *s)
{
	std::string stl_str(s);
	return Model<T>::FromString(stl_str);
}

#endif