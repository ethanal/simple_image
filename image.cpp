#include <iostream>
#include <math.h>
#include <sstream>
#include <stdio.h>

#include "image.h"

#define BIT_DEPTH 8

namespace simple_image {

struct RGB ParseHex(std::string hex) {
  if (hex.at(0) == '#') {
    hex = hex.substr(1);
  }
  std::stringstream ss(hex);
  unsigned int x;
  ss << std::hex << hex;
  ss >> x;

  struct RGB rgb;

  unsigned char mask = (1 << BIT_DEPTH) - 1;
  rgb.red = (x >> BIT_DEPTH >> BIT_DEPTH) & mask;
  rgb.green = (x >> BIT_DEPTH) & mask;
  rgb.blue = x & mask;

  return rgb;
}

struct RGBA ParseHexAlpha(std::string hex) {
  if (hex.at(0) == '#') {
    hex = hex.substr(1);
  }
  std::stringstream ss(hex);
  unsigned int x;
  ss << std::hex << hex;
  ss >> x;

  struct RGBA rgba;

  unsigned char mask = (1 << BIT_DEPTH) - 1;
  rgba.red = (x >> BIT_DEPTH >> BIT_DEPTH >> BIT_DEPTH) & mask;
  rgba.green = (x >> BIT_DEPTH >> BIT_DEPTH) & mask;
  rgba.blue = (x >> BIT_DEPTH) & mask;
  rgba.alpha = x & mask;

  return rgba;
}

struct RGBA OpaqueRGBA(struct RGB rgb) {
  return {
    red: rgb.red,
    green: rgb.green,
    blue: rgb.blue,
    alpha: 255,
  };
}

std::vector<double> RGBA2Vector(struct RGBA rgba) {
  return {
    (double)rgba.red,
    (double)rgba.green,
    (double)rgba.blue,
    (double)rgba.alpha
  };
}

struct RGBA Vector2RGBA(std::vector<double> v) {
  return {
    red: (unsigned char)round(v[0]),
    green: (unsigned char)round(v[1]),
    blue: (unsigned char)round(v[2]),
    alpha: (unsigned char)round(v[3]),
  };
}

Image::Image(int width, int height) {
  width_ = width;
  height_ = height;

  buffer_ = std::vector<RGBA>(width_ * height_, {0, 0, 0, 0});
}

bool Image::valid_xy(int x, int y) const {
  return (0 <= x && x < width_) && (0 <= y && y < height_);
}

int Image::xy_to_index(int x, int y) const {
  return y * width_ + x;
}

bool Image::set_red(int x, int y, unsigned char val) {
  if (!valid_xy(x, y)) return false;
  buffer_[xy_to_index(x, y)].red = val;
  return true;
}

bool Image::set_green(int x, int y, unsigned char val) {
  if (!valid_xy(x, y)) return false;
  buffer_[xy_to_index(x, y)].green = val;
  return true;
}

bool Image::set_blue(int x, int y, unsigned char val) {
  if (!valid_xy(x, y)) return false;
  buffer_[xy_to_index(x, y)].blue = val;
  return true;
}

bool Image::set_alpha(int x, int y, unsigned char val) {
  if (!valid_xy(x, y)) return false;
  buffer_[xy_to_index(x, y)].alpha = val;
  return true;
}

bool Image::set_rgb(int x, int y, unsigned char r, unsigned char g,
    unsigned char b) {
  if (!valid_xy(x, y)) return false;

  int i = xy_to_index(x, y);
  buffer_[i].red = r;
  buffer_[i].green = g;
  buffer_[i].blue = b;

  return true;
}

bool Image::set_rgb(int x, int y, struct RGB rgb) {
  if (!valid_xy(x, y)) return false;

  int i = xy_to_index(x, y);
  buffer_[i].red = rgb.red;
  buffer_[i].green = rgb.green;
  buffer_[i].blue = rgb.blue;

  return true;
}

bool Image::set_rgba(int x, int y, unsigned char r, unsigned char g,
    unsigned char b, unsigned char a) {
  if (!valid_xy(x, y)) return false;
  buffer_[xy_to_index(x, y)] = {r, g, b, a};
  return true;
}

bool Image::set_rgba(int x, int y, struct RGBA rgba) {
  if (!valid_xy(x, y)) return false;
  buffer_[xy_to_index(x, y)] = rgba;
  return true;
}

RGBA Image::get_rgba(int x, int y) {
  if (!valid_xy(x, y)) return {0, 0, 0, 0};
  return buffer_[xy_to_index(x, y)];
}

void Image::SetOpaque() {
  for (auto& rgba : buffer_) {
    rgba.alpha = (1 << BIT_DEPTH) - 1;
  }
}

void Image::Fill(unsigned char r, unsigned char g, unsigned char b) {
  for (auto& rgba : buffer_) {
    rgba = {r, g, b, rgba.alpha};
  }
}

void Image::Fill(unsigned char r, unsigned char g, unsigned char b,
    unsigned char a) {
  for (auto& rgba : buffer_) {
    rgba = {r, g, b, a};
  }
}

void Image::Fill(struct RGB rgb) {
  for (auto& rgba : buffer_) {
    rgba = {rgb.red, rgb.green, rgb.blue, rgba.alpha};
  }
}

void Image::Fill(struct RGBA rgba) {
  for (auto& rgba_ref : buffer_) {
    rgba_ref = rgba;
  }
}

void Image::Print() {
  int i = 0;
  for (auto const& rgba : buffer_) {
    std::cout << "{" <<
      int(rgba.red) << "," <<
      int(rgba.green) << "," <<
      int(rgba.blue) << "," <<
      int(rgba.alpha) << "} ";
    if (i % width_ == width_ - 1) {
      std::cout << std::endl;
    }
    ++i;
  }
}

void Image::save_png_cleanup(FILE* fp, png_structp png_ptr,
                             png_infop info_ptr, png_bytep* row_pointers) {
  // Close the input file.
  fclose(fp);

  // Clean up png_ptr and info_ptr;
  png_destroy_write_struct(&png_ptr, &info_ptr);

  // Cleanup row pointers.
  for (int row = 0; row < height_; ++row) {
    free(row_pointers[row]);
  }
  free(row_pointers);
}

// http://zarb.org/~gc/html/libpng.html referenced for libpng usage.
void Image::SavePNG(std::string filepath) {
  FILE *fp = NULL;
  png_structp png_ptr = NULL;
  png_infop info_ptr = NULL;
  png_bytep *row_pointers = NULL;

  try {
    fp = fopen(filepath.c_str(), "wb");
    if (!fp) {
      throw std::runtime_error("file " + filepath +
                               "could not be opened for writing");
    }

    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL,
                                      NULL);
    if (!png_ptr) {
      throw std::runtime_error("png_create_write_struct failed");
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
      throw std::runtime_error("png_create_info_struct failed");
    }

    // See http://www.libpng.org/pub/png/libpng-1.2.5-manual.html
    // "When libpng encounters an error, it expects to longjmp back to your
    // routine."
    if (setjmp(png_jmpbuf(png_ptr))) {
      throw std::runtime_error("error in png_init_io");
    }
    png_init_io(png_ptr, fp);


    // Write header.
    if (setjmp(png_jmpbuf(png_ptr))) {
      throw std::runtime_error("error writing header");
    }
    png_set_IHDR(png_ptr, info_ptr, (png_uint_32)width_, (png_uint_32)height_,
        BIT_DEPTH, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    png_write_info(png_ptr, info_ptr);

    // Copy data from buffer.
    row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height_);
    for (int row = 0; row < height_; ++row) {
      row_pointers[row] = (png_byte*) malloc(png_get_rowbytes(png_ptr,
                                                              info_ptr));

      png_bytep pixel_ptr = row_pointers[row];
      for (int col = 0; col < width_; ++col) {
        int i = xy_to_index(col, row);
        pixel_ptr[0] = buffer_[i].red;
        pixel_ptr[1] = buffer_[i].green;
        pixel_ptr[2] = buffer_[i].blue;
        pixel_ptr[3] = buffer_[i].alpha;
        pixel_ptr += 4;
      }
    }

    // Write data.
    if (setjmp(png_jmpbuf(png_ptr))) {
      throw std::runtime_error("error writing data");
    }
    png_write_image(png_ptr, row_pointers);

    // End write.
    if (setjmp(png_jmpbuf(png_ptr))) {
      throw std::runtime_error("error during png_write_end");
    }
    png_write_end(png_ptr, NULL);
  } catch (const std::exception& e) {
    save_png_cleanup(fp, png_ptr, info_ptr, row_pointers);
    throw;
  }

  save_png_cleanup(fp, png_ptr, info_ptr, row_pointers);
}

}  // namespace simple_image
