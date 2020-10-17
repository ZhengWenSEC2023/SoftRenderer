#pragma once

#include <fstream>

typedef unsigned char uchar;
typedef unsigned long ulong;
typedef unsigned int uint;

#pragma pack(push, 1)
struct TGA_Header {
	char id_length;
	char color_map_type;
	char datatype_code;
	short color_map_origin;
	short color_map_length;
	char color_map_depth;
	short x_origin;
	short y_origin;
	short width;
	short height;
	char bits_per_pixel;
	char image_descriptor;
};
#pragma pack(pop)

struct TGAColor {
	union {
		struct {
			uchar blue, green, red, alpha;
		};
		uchar raw[4];
		uint val;
	};
	int byte_per_pixel;

	TGAColor() : val(0), byte_per_pixel(1) {}
	TGAColor(uchar R, uchar G, uchar B, uchar A) : red(R), green(G), blue(B), alpha(A), byte_per_pixel(4) {}
	TGAColor(int v, int bpp) : val(v), byte_per_pixel(bpp) {}
	TGAColor(const TGAColor &c) : val(c.val), byte_per_pixel(c.byte_per_pixel) {}
	TGAColor(const uchar *p, int bpp) : val(0), byte_per_pixel(bpp) {
		for (int i = 0; i < bpp; i++)
		{
			raw[i] = p[i];
		}
	}
	TGAColor& operator=(const TGAColor& c)
	{
		if (this != &c)
		{
			byte_per_pixel = c.byte_per_pixel;
			val = c.val;
		}
		return *this;
	}
};



class TGAImage
{
protected:
	uchar* data;
	int width, height;
	int byte_per_pixel;

	bool load_rle_data(std::ifstream& in);
	bool unload_rle_data(std::ofstream& out);
public:
	enum Format { GRAYSCALE = 1, RGB = 3, RGBA = 4 };

	TGAImage();
	TGAImage(int w, int h, int bpp);
	TGAImage(const TGAImage& img);
	bool read_tga_file(const char* filename);
	bool write_tga_file(const char* filename, bool rle=true);
	bool flip_horizontally();
	bool flip_vertically();
	bool scale(int w, int h);
	TGAColor get(int x, int y);
	bool set(int x, int y, TGAColor c);
	~TGAImage();
	TGAImage& operator=(const TGAImage& img);
	int get_width();
	int get_height();
	int get_bytespp();
	uchar* buffer();
	void clear();
};
