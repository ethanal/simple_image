/**
 * Ethan Lowman, 2016
 *
 * Provides a simple interface to manipulate RGBA image buffers and save to
 * disk using libpng.
 */

#ifndef SIMPLE_IMAGE_IMAGE_H_
#define SIMPLE_IMAGE_IMAGE_H_

#include <vector>
#include <string>
#include <exception>

extern "C" {
#include <png.h>
}

namespace simple_image {

struct RGB {
  unsigned char red, green, blue;
};

struct RGBA {
  unsigned char red, green, blue, alpha;
};

struct RGB ParseHex(std::string hex);

class Image {
private:
  int width_, height_;

  // Row-major pixel buffer.
  std::vector<struct RGBA> buffer_;

  int row_col_to_index(int row, int col) const;
  void save_png_cleanup(FILE* fp, png_structp png_ptr, png_infop info_ptr,
      png_bytep* row_pointers);
public:
  Image(int width, int height);

  // Disable copy and assign.
  Image(Image const&) = delete;
  Image& operator=(Image const&) = delete;

  void set_red(int row, int col, unsigned char val);
  void set_green(int row, int col, unsigned char val);
  void set_blue(int row, int col, unsigned char val);
  void set_alpha(int row, int col, unsigned char val);

  void set_rgb(int row, int col, unsigned char r, unsigned char g,
      unsigned char b);
  void set_rgb(int row, int col, struct RGB rgb);
  void set_rgba(int row, int col, unsigned char r, unsigned char g,
      unsigned char b, unsigned char a);
  void set_rgba(int row, int col, struct RGBA rgba);

  // Set all alpha bytes to 0xFF.
  void SetOpaque();

  // Fills image with a solid color.
  void Fill(unsigned char r, unsigned char g, unsigned char b);
  void Fill(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
  void Fill(struct RGB rgb);
  void Fill(struct RGBA rgba);

  void Print();

  void SavePNG(std::string filepath);
};

}

#endif  // SIMPLE_IMAGE_IMAGE_H_
