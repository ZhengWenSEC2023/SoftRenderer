#pragma once

#include <vector>
#include "geometry.h"

class Model
{
private:
	// verts_: coordinates of each vertices.
	// faces_: index of the vertices composing a single face.  

	// both with 3 length.

	std::vector<Vec3f> verts_;
	std::vector<std::vector<int>> verts_textures_;
	std::vector<std::vector<int>> faces_;
	std::vector<Vec2f> textures_;
public:
	Model(const char* filename);
	~Model();
	int nverts();
	int nfaces();
	int nvert_textures();
	Vec3f vert(int i);
	std::vector<int> face(int idx);
	std::vector<int> face_texture(int idx);
};