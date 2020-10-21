#pragma once
#include <vector>
#include <cmath>
#include <algorithm>

#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

#define EPSILON_BARY 0.05
#define EPSILON_SCOPE 0.0001
#define DEPTH 255

void line(Vec3i p1, Vec3i p2, TGAImage& image, TGAColor color);
void triangle(Vec3i* screen, std::vector<int>& z_buffer, TGAImage& image, TGAColor color_surface);

//void triangle_bary(std::vector<Vec3f> vertices, std::vector<float>& z_buffer, TGAImage& image, TGAColor color_surface, TGAColor color_edge);
//void triangle_bary(std::vector<Vec3f> vertices, std::vector<float>& z_buffer, TGAImage& image, TGAColor color_surface);
Vec3f baryCentric(std::vector<Vec3f> vertices, Vec3f p);
Vec3f normalVector(Vec3f* triangle);
Matrix viewport(int x, int y, int w, int h);
Vec3f m2v(Matrix m);
Matrix v2m(Vec3f v);
