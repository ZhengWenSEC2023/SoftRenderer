#include "basic.h"

void line(Vec3i p0, Vec3i p1, TGAImage& image, TGAColor color)
{
	bool steep = false;
	if (std::abs(p0.x - p1.x) < std::abs(p0.y - p1.y))
	{
		std::swap(p0.x, p0.y);
		std::swap(p1.x, p1.y);
		steep = true;
	}
	if (p0.x > p1.x)
	{
		std::swap(p0, p1);
	}

	if (steep && p0.x >= image.get_height())
	{
		return;
	}
	if (!steep && p0.x >= image.get_width())
	{
		return;
	}

	int dx = p1.x - p0.x;
	int dy = p1.y - p0.y;
	int yincr = (p1.y > p0.y) ? 1 : -1;

	//float derror = std::abs(dy / (float)dx);				// in float
	//float error = 0;										// in float
	int derror = std::abs(dy) * 2;
	// change error with int, error = (dy / dx) > 0.5 ?
	// --> dy > 0.5 * dx ? 
	// --> 2 * dy > dx ?
	int error = 0;
	int y = p0.y;

	if (steep)
	{
		for (int x = p0.x; x <= p1.x; x++)
		{
			if (x >= 0 && x < image.get_height() && y >= 0 && y < image.get_width())
				image.set(y, x, color);
			error += derror;
			//if (error > 0.5)									// in float
			if (error > dx)
			{
				y += yincr;
				error -= dx * 2;
			}
		}
	}
	else
	{
		for (int x = p0.x; x <= p1.x; x++)
		{
			if (x >= 0 && x < image.get_width() && y >= 0 && y < image.get_height())
				image.set(x, y, color);
			error += derror;
			//if (error > 0.5)									// in float
			if (error > dx)
			{
				y += yincr;
				error -= dx * 2;
			}
		}
	}
}

void triangle(Vec3i* screen, std::vector<int>& z_buffer, TGAImage& image, TGAColor color_surface)
{
	// line version triangle

	if (screen[0].y == screen[1].y && screen[0].y == screen[2].y)
		return;

	if (screen[0].y < screen[1].y)
	{
		std::swap(screen[0], screen[1]);
	}
	if (screen[0].y < screen[2].y)
	{
		std::swap(screen[0], screen[2]);
	}
	if (screen[1].y < screen[2].y)
	{
		std::swap(screen[1], screen[2]);
	}

	int y_02 = screen[0].y - screen[2].y;
	int y_01 = screen[0].y - screen[1].y;
	int y_12 = screen[1].y - screen[2].y;
	for (int i = 0; i < y_02; i++)
	{
		bool second_half = (i > y_12) || (y_12 == 0);
		int part_height = second_half ? y_01 : y_12;
		float ratio_total = (float)i / y_02;
		float ratio_part = second_half ? ((float)(i - y_12) / part_height) : ((float)i / part_height);
		
		// CAUTION!!! should be Vec3f(screen[0] - screen[2]) instead of (screen[0] - screen[2]), for the loss of precision may cause black lines 
		Vec3i S_02 = screen[2] + Vec3f(screen[0] - screen[2]) * ratio_total;
		Vec3i S_part = second_half ? (screen[1] + Vec3f(screen[0] - screen[1]) * ratio_part) : (screen[2] + Vec3f(screen[1] - screen[2]) * ratio_part);
		if (S_02.x < S_part.x)
		{
			std::swap(S_02, S_part);
		}
		for (int k = S_part.x; k <= S_02.x; k++)
		{
			float ratio_x = (S_part.x == S_02.x) ? 1. : (float)(k - S_part.x) / (float)(S_02.x - S_part.x);
			Vec3i bary = Vec3f(S_part) + Vec3f(S_02 - S_part) * ratio_x;
			if (z_buffer[bary.x + bary.y * image.get_width()] < bary.z) {
				image.set(bary.x, bary.y, color_surface);
				z_buffer[bary.x + bary.y * image.get_width()] = bary.z;
			}
		}
	}
	//for (int i = 0; i < 3; i++)
	//{
	//	line(vs.at(i), vs.at((i + 1) % 3), image, color_edge);
	//}
}

//void triangle_bary(std::vector<Vec3i> vs, std::vector<float>& z_buffer, TGAImage& image, TGAColor color_surface, TGAColor color_edge)
//{
//	if (vs.size() != 3)
//	{
//		std::cout << "Number of vertices should be equal to 3, actually " << vs.size() << std::endl;
//		exit(1);
//	}
//	if (vs[0].y == vs[1].y && vs[0].y == vs[2].y)
//		return;
//	Vec2i bound_up_right(0, 0);
//	Vec2i bound_low_left(image.get_width() - 1, image.get_height() - 1);
//	Vec2i bound_image(image.get_width() - 1, image.get_height() - 1);
//
//	for (Vec3i v : vs)
//	{
//		bound_up_right.x = std::min(bound_image.x, std::max(bound_up_right.x, (int)v.x));
//		bound_up_right.y = std::min(bound_image.y, std::max(bound_up_right.y, (int)v.y));
//		bound_low_left.x = std::max(0, std::min(bound_low_left.x, (int)v.x));
//		bound_low_left.y = std::max(0, std::min(bound_low_left.y, (int)v.y));
//	}
//
//	Vec3f point;
//	for (point.x = bound_low_left.x; point.x < bound_up_right.x; point.x++)
//	{
//		for (point.y = bound_low_left.y; point.y < bound_up_right.y; point.y++)
//		{
//			Vec3f bary = baryCentric(vs, point);
//			if (bary.x < -EPSILON_BARY || bary.y < -EPSILON_BARY || bary.z < -EPSILON_BARY)
//				continue;
//			float depth = vs[0].z * bary.x + vs[1].z * bary.y + vs[2].z * bary.z;
//			if (z_buffer[point.x + image.get_width() * point.y] > depth)
//				continue;
//			image.set(point.x, point.y, color_surface);
//			z_buffer[point.x + image.get_width() * point.y] = depth;
//		}
//	}
//	for (int i = 0; i < 3; i++)
//	{
//		line(vs.at(i), vs.at((i + 1) % 3), image, color_edge);
//	}
//}
//
//void triangle_bary(std::vector<Vec3i> vs, std::vector<float>& z_buffer, TGAImage& image, TGAColor color_surface)
//{
//	if (vs.size() != 3)
//	{
//		std::cout << "Number of vertices should be equal to 3, actually " << vs.size() << std::endl;
//		exit(1);
//	}
//	if (vs[0].y == vs[1].y && vs[0].y == vs[2].y)
//		return;
//	Vec2i bound_up_right(0, 0);
//	Vec2i bound_low_left(image.get_width() - 1, image.get_height() - 1);
//	Vec2i bound_image(image.get_width() - 1, image.get_height() - 1);
//	for (Vec3i v : vs)
//	{
//		bound_up_right.x = std::min(bound_image.x, std::max(bound_up_right.x, (int)v.x));
//		bound_up_right.y = std::min(bound_image.y, std::max(bound_up_right.y, (int)v.y));
//		bound_low_left.x = std::max(0, std::min(bound_low_left.x, (int)v.x));
//		bound_low_left.y = std::max(0, std::min(bound_low_left.y, (int)v.y));
//	}
//
//	Vec3f point;
//	for (point.x = bound_low_left.x; point.x < bound_up_right.x; point.x++)
//	{
//		for (point.y = bound_low_left.y; point.y < bound_up_right.y; point.y++)
//		{
//			Vec3f bary = baryCentric(vs, point);
//			if (bary.x < -EPSILON_BARY || bary.y < -EPSILON_BARY || bary.z < -EPSILON_BARY)
//				continue;
//			float depth = vs[0].z * bary.x + vs[1].z * bary.y + vs[2].z * bary.z;
//			if (z_buffer[point.x + image.get_width() * point.y] > depth)
//				continue;
//			image.set(point.x, point.y, color_surface);
//			z_buffer[point.x + image.get_width() * point.y] = depth;
//		}
//	}
//}

Vec3f baryCentric(std::vector<Vec3f> vs, Vec3f p)
{
	// based on vertices 0, P = (1 - u - v)A + uB + vC
	// cross_prod.x = u, cross_prod.y = v
	// vs[0] = A, vs[1] = B, vs[2] = C, P = p
	// u, v is the barycentric coordinates based on triangle vs, and p is a point on rendered image
	// should be normalized by z so that z = 1, homogeneous coordinates.

	Vec3f cross_prod = Vec3f(vs[1].x - vs[0].x, vs[2].x - vs[0].x, vs[0].x - p.x) ^ Vec3f(vs[1].y - vs[0].y, vs[2].y - vs[0].y, vs[0].y - p.y);
	if (cross_prod.z == 0)
		return Vec3f(-1, 1, 1);          // triangle is a segmentation, det(ABx ACx; ABy ACy) = 0
	return Vec3f(1.0 - (cross_prod.x + cross_prod.y) / (float)cross_prod.z, cross_prod.x / (float)cross_prod.z, cross_prod.y / (float)cross_prod.z);
}

Vec3f normalVector(Vec3f* t)
{
	Vec3f norm = (t[2] - t[0]) ^ (t[1] - t[0]);
	return norm.normalize();
}

Vec3f m2v(Matrix m)
{
	// homogeneous coordinates -> vector
	return Vec3f(m[0][0] / m[3][0], m[1][0] / m[3][0], m[2][0] / m[3][0]);
}

Matrix v2m(Vec3f v) 
{
	// vector -> matrix
	Matrix m(4, 1);
	m[0][0] = v.x;
	m[1][0] = v.y;
	m[2][0] = v.z;
	m[3][0] = 1.f;
	return m;
}

Matrix viewport(int x, int y, int w, int h)
{
	// x, y: upper left corner of canvas
	// w, h: weight and height of canvas
	// DEPTH: z_near
	Matrix m = Matrix::identity(4);
	m[0][3] = x + w / 2.0;
	m[1][3] = y + h / 2.0;
	m[2][3] = DEPTH / 2.0;
	m[0][0] = w / 2.0;
	m[1][1] = h / 2.0;
	m[2][2] = DEPTH / 2.0;
	return m;
}
