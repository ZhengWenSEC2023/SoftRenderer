#include <vector>
#include <cmath>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include "basic.h"

#define INF 100000.0

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);
const TGAColor yellow = TGAColor(255, 255, 0, 255);

Model* model = NULL;
const int width = 300;
const int height = 300;
const int depth = 255;


int main(int argc, char** argv) {
	if (2 == argc) {
		model = new Model(argv[1]);
	}
	else {
		model = new Model("obj/african_head.obj");
	}

	TGAImage texture;
	texture.read_tga_file("D:/EE599FinalProj/Renderer/Renderer/obj/african_head_diffuse.tga");
	
	// z-buffer

	//camera, light
	Vec3f camera(0, 0, 3);
	const Vec3f light = Vec3f(0., 0., -1.).normalize();

	// draw model
	TGAImage image(width, height, TGAImage::RGB);
	std::vector<int> z_buffer(image.get_height() * image.get_width(), -INF);
	Matrix Projection = Matrix::identity(4);
	Matrix ViewPort = viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
	Projection[3][2] = -1.0 / camera.z;
	// triangle
	for (int i = 0; i < model->nfaces(); i++) {
		std::vector<int> face = model->face(i);
		Vec3i screen_coords[3];
		Vec3f world_coords[3];
		for (int j = 0; j < 3; j++)
		{
			Vec3f v = model->vert(face[j]);
			screen_coords[j] = m2v(ViewPort * Projection * v2m(v));
			world_coords[j] = v;
		}
		Vec3f normal = normalVector(world_coords);
		float intensity = normal * light;
		if (intensity > 0)
		{
			triangle(screen_coords, z_buffer, image, TGAColor(intensity * 255., intensity * 255., intensity * 255., 255));
		}
	}

	// // line
	//for (int i = 0; i < model->nfaces(); i++) {
	//	std::vector<int> face = model->face(i);
	//	std::vector<Vec2i> screen_coords(3);
	//	for (int j = 0; j < 3; j++)
	//	{
	//		Vec3f world_coords = model->vert(face[j]);
	//		screen_coords[j] = Vec2i((world_coords.x + 1.) * width / 2., (world_coords.y + 1.) * height / 2.);
	//	}
	//	for (int j = 0; j < 3; j++) {
	//		line(screen_coords[j], screen_coords[(j + 1) % 3], image, red);
	//	}
	//}

	// // playground
	//std::vector<Vec3i> tri_v_1 = { Vec3i(72, 72, 0), Vec3i(144, 144, 0), Vec3i(72, 144, 0) };
	//std::vector<Vec3f> tri_v_2 = { Vec3f(72, 72, 0), Vec3f(144, 144, 0), Vec3f(72, 144, 0) };
	//triangle(tri_v_1, tri_v_2, z_buffer, image, white);

	image.flip_vertically();                   // origin at the left bottom corner of the image

	bool write_image_flag = image.write_tga_file("output.tga");
	if (!write_image_flag)
	{
		std::cout << "Error in writing .tga image file!" << std::endl;
		delete model;
		return 1;
	}

	//{ // dump z-buffer (debugging purposes only)
	//	TGAImage zbimage(width, height, TGAImage::GRAYSCALE);
	//	for (int i = 0; i < width; i++) {
	//		for (int j = 0; j < height; j++) {
	//			zbimage.set(i, j, TGAColor(z_buffer[i + j * width], 1));
	//		}
	//	}
	//	zbimage.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	//	zbimage.write_tga_file("zbuffer.tga");
	//}

	delete model;
	return 0;
}
