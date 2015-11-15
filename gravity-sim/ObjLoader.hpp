#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#define NOMINMAX

#include <cstdint>
#include <vector>
#include <sstream>
#include <limits>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

template <size_t T>
class Face
{
	uint16_t v[T];
	uint16_t vt[T];
	uint16_t vn[T];
};

template <size_t T = 4>
class Model
{
public:
	std::vector<glm::vec4> GeomVerts;
	std::vector<glm::vec3> TexCoords;
	std::vector<glm::vec3> Normals;
	std::vector<glm::vec3> ParameterVerts;
	std::vector< Face<T> > Faces;

	static Model<T> FromStream(std::istream &s);
	static Model<T> FromString(const std::string &s);
	static Model<T> FromCStr(const char *s);
};

template <size_t T>
Model<T> Model<T>::FromStream(std::istream &s)
{
	Model<T> ret;
	std::string buf;
	float tmp[4];

	while (!s.eof())
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
				ret.GeomVerts.push_back(glm::vec4(tmp[0], tmp[1], tmp[2], tmp[3]));
				break;
			case 't':
				// texture coord follows
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
				ret.TexCoords.push_back(glm::vec3(tmp[0], tmp[1], tmp[2]));
				break;
			case 'n':
				// normal coord follows
				s >> tmp[0];
				s >> tmp[1];
				s >> tmp[2];
				s.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				ret.Normals.push_back(glm::vec3(tmp[0], tmp[1], tmp[2]));
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
				ret.ParameterVerts.push_back(glm::vec3(tmp[0], tmp[1], tmp[2]));
				break;
			default:
				s.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				break;
			}

			break;
		case 'f':
			// face follows
			Face<T> face;
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
			}
			break;
		default:
			s.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			break;
		}
	}

	return ret;
}

template <size_t T>
Model<T> Model<T>::FromString(const std::string &s)
{
	std::stringstream ss;
	ss << s;
	return Model<T>::FromStream(ss);
}

template <size_t T>
Model<T> Model<T>::FromCStr(const char *s)
{
	std::string stl_str(s);
	return Model<T>::FromString(stl_str);
}

#endif