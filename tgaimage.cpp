#include <iostream>
#include <fstream>
#include <string.h>
#include <time.h>
#include <math.h>

#include "tgaimage.h"

TGAImage::TGAImage() : data(NULL), width(0), height(0), byte_per_pixel(0) {}

TGAImage::TGAImage(int w, int h, int bpp) :data(NULL), width(w), height(h), byte_per_pixel(bpp) 
{
	ulong nbytes = width * height * byte_per_pixel;
	data = new uchar[nbytes];
	memset(data, 0, nbytes);
}

TGAImage::TGAImage(const TGAImage& img)
{
	width = img.width;
	height = img.height;
	byte_per_pixel = img.byte_per_pixel;
	ulong nbytes = width * height * byte_per_pixel;
	data = new uchar[nbytes];
	memcpy(data, img.data, nbytes);
}

TGAImage::~TGAImage() {
	if (data) 
		delete[] data;
}

TGAImage& TGAImage::operator=(const TGAImage& img)
{
	if (this != &img)
	{
		if (data) 
			delete[] data;
		width = img.width;
		height = img.height;
		byte_per_pixel = img.byte_per_pixel;
		ulong nbytes = width * height * byte_per_pixel;
		data = new uchar[nbytes];
		memcpy(data, img.data, nbytes);
	}
	return *this;
}

bool TGAImage::read_tga_file(const char* filename)
{
	if (data) 
		delete[] data;
	data = NULL;
	std::ifstream in;
	in.open(filename, std::ios::binary);
	if (!in.is_open())
	{
		std::cout << "can't open file" << filename << std::endl;
		in.close();
		return false;
	}
	TGA_Header header;
	in.read((char*)&header, sizeof(header));
	if (!in.good())
	{
		in.close();
		std::cout << "an error occurred while reading the header" << std::endl;
		return false;
	}
	width = header.width;
	height = header.height;
	byte_per_pixel = header.bits_per_pixel >> 3;
	if (width <= 0 || height <= 0 || (byte_per_pixel != GRAYSCALE && byte_per_pixel != RGB && byte_per_pixel != RGBA))
	{
		in.close();
		std::cout << "bad byte per pixel (or width/height) value" << std::endl;
		return false;
	}
	ulong nbytes = byte_per_pixel * width * height;
	data = new uchar[nbytes];
	if (3 == header.datatype_code || 2 == header.datatype_code)
	{
		in.read((char*)data, nbytes);
		if (!in.good())
		{
			in.close();
			std::cout << "an error occurred while reading the data" << std::endl;
			return false;
		}
	}
	else if (10 == header.datatype_code || 11 == header.datatype_code)
	{
		if (!load_rle_data(in))
		{
			in.close();
			std::cout << "an error occurred while reading the data" << std::endl;
			return false;
		}
	}
	else
	{
		in.close();
		std::cout << "unknown file format " << (int)header.datatype_code << "" << std::endl;
		return false;
	}
	if (!(header.image_descriptor & 0x20))
	{
		flip_vertically();
	}
	if (header.image_descriptor & 0x10)
	{
		flip_horizontally();
	}
	std::cout << width << "x" << height << "/" << byte_per_pixel * 8 << "" << std::endl;
	in.close();
	return true;
}

bool TGAImage::load_rle_data(std::ifstream& in)
{
	ulong pixel_count = width * height;
	ulong current_pixel = 0;
	ulong current_byte = 0;
	TGAColor color_buffer;
	do {
		uchar chunk_header = 0;     // a head of a chunk of pixel, denote the number of pixel, maximum is 128
		chunk_header = in.get();
		if (!in.good())
		{
			std::cout << "an error occured while reading the data" << std::endl;
			return false;
		}
		if (chunk_header < 128) {
			chunk_header++;
			for (int i = 0; i < chunk_header; i++)
			{
				in.read((char*)color_buffer.raw, byte_per_pixel);
				if (!in.good())
				{
					std::cout << "an error occured while reading the header" << std::endl;
					return false;
				}
				for (int t = 0; t < byte_per_pixel; t++)
					data[current_byte++] = color_buffer.raw[t];
				current_pixel++;
				if (current_pixel > pixel_count)
				{
					std::cout << "Too many pixels read" << std::endl;
					return false;
				}
			}
		}
		else {
			chunk_header -= 127;
			in.read((char*)color_buffer.raw, byte_per_pixel);
			if (!in.good())
			{
				std::cout << "an error occurred while reading the header\b";
				return false;
			}
			for (int i = 0; i < chunk_header; i++)
			{
				for (int t = 0; t < byte_per_pixel; t++)
					data[current_byte++] = color_buffer.raw[t];
				current_pixel++;
				if (current_pixel > pixel_count)
				{
					std::cerr << "Too many pixels read" << std::endl;
					return false;
				}
			}
		}
	} while (current_pixel < pixel_count);
	return true;
}

bool TGAImage::write_tga_file(const char* filename, bool rle)
{
	uchar developer_area_ref[4] = { 0, 0, 0, 0 };
	uchar extension_area_ref[4] = { 0, 0, 0, 0 };
	uchar footer[18] = { 'T','R','U','E','V','I','S','I','O','N','-','X','F','I','L','E','.','\0' };
	std::ofstream out;
	out.open(filename, std::ios::binary);
	if (!out.is_open())
	{
		std::cout << "Can't open file " << filename << "!" << std::endl;
		out.close();
		return false;
	}
	TGA_Header header;
	memset((void*)&header, 0, sizeof(header));
	header.bits_per_pixel = byte_per_pixel << 3;
	header.width = width;
	header.height = height;
	header.datatype_code = (byte_per_pixel == GRAYSCALE ? (rle ? 11 : 3) : (rle ? 10 : 2));
	header.image_descriptor = 0x20;     // top-left origin
	out.write((char*)&header, sizeof(header));
	if (!out.good()) {
		out.close();
		std::cout << "can't dump the tga file" << std::endl;
		return false;
	}
	if (!rle) {
		out.write((char*)data, width * height * byte_per_pixel);
		if (!out.good()) {
			std::cerr << "can't unload raw data" << std::endl;
			out.close();
			return false;
		}
	}
	else {
		if (!unload_rle_data(out)) {
			out.close();
			std::cerr << "can't unload rle data" << std::endl;
			return false;
		}
	}
	out.write((char*)developer_area_ref, sizeof(developer_area_ref));
	if (!out.good()) {
		std::cout << "can't dump the tga file" << std::endl;
		out.close();
		return false;
	}
	out.write((char*)extension_area_ref, sizeof(extension_area_ref));
	if (!out.good()) {
		std::cout << "can't dump the tga file" << std::endl;
		out.close();
		return false;
	}
	out.write((char*)footer, sizeof(footer));
	if (!out.good()) {
		std::cerr << "can't dump the tga file" << std::endl;
		out.close();
		return false;
	}
	out.close();
	return true;
}

bool TGAImage::unload_rle_data(std::ofstream& out) {
	const uchar max_chunk_length = 128;
	ulong npixels = width * height;
	ulong curpix = 0;
	while (curpix < npixels) {
		ulong chunkstart = curpix * byte_per_pixel;
		ulong curbyte = curpix * byte_per_pixel;
		uchar run_length = 1;
		bool raw = true;
		while (curpix + run_length < npixels && run_length < max_chunk_length) {
			bool succ_eq = true;
			for (int t = 0; succ_eq && t < byte_per_pixel; t++) {
				succ_eq = (data[curbyte + t] == data[curbyte + t + byte_per_pixel]);
			}
			curbyte += byte_per_pixel;
			if (1 == run_length) {
				raw = !succ_eq;
			}
			if (raw && succ_eq) {
				run_length--;
				break;
			}
			if (!raw && !succ_eq) {
				break;
			}
			run_length++;
		}
		curpix += run_length;
		out.put(raw ? run_length - 1 : run_length + 127);
		if (!out.good()) {
			std::cerr << "can't dump the tga file" << std::endl;
			return false;
		}
		out.write((char*)(data + chunkstart), (raw ? run_length * byte_per_pixel : byte_per_pixel));
		if (!out.good()) {
			std::cerr << "can't dump the tga file" << std::endl;
			return false;
		}
	}
	return true;
}

TGAColor TGAImage::get(int x, int y) {
	if (!data || x < 0 || y < 0 || x >= width || y >= height) {
		return TGAColor();
	}
	return TGAColor(data + (x + y * width) * byte_per_pixel, byte_per_pixel);
}

bool TGAImage::set(int x, int y, TGAColor c) {
	if (!data || x < 0 || y < 0 || x >= width || y >= height) {
		std::cout << "Pixel out of boundary, set pixel FAILED! (" << y << ", " << x << ")" << std::endl;
		return false;
	}
	memcpy(data + (x + y * width) * byte_per_pixel, c.raw, byte_per_pixel);
	return true;
}

int TGAImage::get_bytespp() {
	return byte_per_pixel;
}

int TGAImage::get_width() {
	return width;
}

int TGAImage::get_height() {
	return height;
}

bool TGAImage::flip_horizontally() {
	if (!data) return false;
	int half = width >> 1;
	for (int i = 0; i < half; i++) {
		for (int j = 0; j < height; j++) {
			TGAColor c1 = get(i, j);
			TGAColor c2 = get(width - 1 - i, j);
			set(i, j, c2);
			set(width - 1 - i, j, c1);
		}
	}
	return true;
}

bool TGAImage::flip_vertically() {
	if (!data) return false;
	ulong bytes_per_line = width * byte_per_pixel;
	uchar* line = new uchar[bytes_per_line];
	int half = height >> 1;
	for (int j = 0; j < half; j++) {
		ulong l1 = j * bytes_per_line;
		ulong l2 = (height - 1 - j) * bytes_per_line;
		memmove((void*)line, (void*)(data + l1), bytes_per_line);
		memmove((void*)(data + l1), (void*)(data + l2), bytes_per_line);
		memmove((void*)(data + l2), (void*)line, bytes_per_line);
	}
	delete[] line;
	return true;
}

uchar* TGAImage::buffer() {
	return data;
}

void TGAImage::clear() {
	memset((void*)data, 0, width * height * byte_per_pixel);
}

bool TGAImage::scale(int w, int h) {
	if (w <= 0 || h <= 0 || !data) return false;
	uchar* tdata = new uchar[w * h * byte_per_pixel];
	int nscanline = 0;
	int oscanline = 0;
	int erry = 0;
	ulong nlinebytes = w * byte_per_pixel;
	ulong olinebytes = width * byte_per_pixel;
	for (int j = 0; j < height; j++) {
		int errx = width - w;
		int nx = -byte_per_pixel;
		int ox = -byte_per_pixel;
		for (int i = 0; i < width; i++) {
			ox += byte_per_pixel;
			errx += w;
			while (errx >= (int)width) {
				errx -= width;
				nx += byte_per_pixel;
				memcpy(tdata + nscanline + nx, data + oscanline + ox, byte_per_pixel);
			}
		}
		erry += h;
		oscanline += olinebytes;
		while (erry >= (int)height) {
			if (erry >= (int)height << 1) // it means we jump over a scanline
				memcpy(tdata + nscanline + nlinebytes, tdata + nscanline, nlinebytes);
			erry -= height;
			nscanline += nlinebytes;
		}
	}
	delete[] data;
	data = tdata;
	width = w;
	height = h;
	return true;
}