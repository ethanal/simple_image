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
struct RGBA ParseHexAlpha(std::string hex);
struct RGBA OpaqueRGBA(struct RGB rgb);
std::vector<double> RGBA2Vector(struct RGBA rgba);
struct RGBA Vector2RGBA(std::vector<double> v);

class Image {
private:
  int width_, height_;

  // Row-major pixel buffer.
  std::vector<struct RGBA> buffer_;

  bool valid_xy(int x, int y) const;
  int xy_to_index(int x, int y) const;
  void save_png_cleanup(FILE* fp, png_structp png_ptr, png_infop info_ptr,
      png_bytep* row_pointers);
public:
  Image(int width, int height);

  // Disable copy and assign.
  Image(Image const&) = delete;
  Image& operator=(Image const&) = delete;

  // Sets a pixel value. Returns false if the (x, y) coordinates passed are
  // out of bounds. Otherwise returns true.
  bool set_red(int x, int y, unsigned char val);
  bool set_green(int x, int y, unsigned char val);
  bool set_blue(int x, int y, unsigned char val);
  bool set_alpha(int x, int y, unsigned char val);

  bool set_rgb(int x, int y, unsigned char r, unsigned char g,
      unsigned char b);
  bool set_rgb(int x, int y, struct RGB rgb);
  bool set_rgba(int x, int y, unsigned char r, unsigned char g,
      unsigned char b, unsigned char a);
  bool set_rgba(int x, int y, struct RGBA rgba);

  // Gets a pixel value. Returns {0, 0, 0, 0} if the coordinates passed are out
  // of bounds.
  RGBA get_rgba(int x, int y);

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

}  // namespace simple_image

#endif  // SIMPLE_IMAGE_IMAGE_H_
