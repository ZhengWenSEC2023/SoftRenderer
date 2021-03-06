#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#include "model.h"

Model::Model(const char* filename) : verts_(), faces_(), textures_(), verts_textures_()
{
	std::ifstream in;
	in.open(filename, std::ifstream::in);
	if (in.fail())
	{
		std::cout << "Cannot open the wavefront file!" << std::endl;
		exit(1);
	}
	std::string line;
	while (!in.eof())
	{
		std::getline(in, line);
		std::istringstream iss(line.c_str());
		char trash;
		if (!line.compare(0, 2, "v "))
		{
			iss >> trash;
			Vec3f v;
			for (int i = 0; i < 3; i++)
				iss >> v[i];
			verts_.push_back(v);
		}
		if (!line.compare(0, 3, "vt "))
		{
			iss >> trash;
			Vec2f v;
			iss >> v[0];
			iss >> v[1];
			iss >> trash;
			textures_.push_back(v);
		}
		else if (!line.compare(0, 2, "f "))
		{
			std::vector<int> f, vt;
			int itrash, idx_f, idx_vt;
			iss >> trash;                           
			// f 24/1/24 25/2/25 26/3/26
			// ~
			while (iss >> idx_f >> trash >> idx_vt >> trash >> itrash) {
				// f 24/1/24 25/2/25 26/3/26
				//   **-~-~~						-->  **: idx, -: trash, ~~: itrash 
				idx_f--;								// idx begin from 1 --> 0
				f.push_back(idx_f);
				vt.push_back(idx_vt);
			}
			faces_.push_back(f);
			verts_textures_.push_back(vt);
		}
	}
	std::cout << "# v# " << verts_.size() << " f# " << faces_.size() << " vt# " << textures_.size() << std::endl;
}

Model::~Model() {
}

int Model::nverts() {
	return (int)verts_.size();
}

int Model::nfaces() {
	return (int)faces_.size();
}

int Model::nvert_textures() {
	return (int)textures_.size();
}

std::vector<int> Model::face(int idx) {
	return faces_[idx];
}

std::vector<int> Model::face_texture(int idx) {
	return verts_textures_[idx];
}

Vec3f Model::vert(int i) {
	return verts_[i];
}
