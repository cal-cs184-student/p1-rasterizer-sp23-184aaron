#include "rasterizer.h"

using namespace std;

namespace CGL {

  RasterizerImp::RasterizerImp(PixelSampleMethod psm, LevelSampleMethod lsm,
    size_t width, size_t height,
    unsigned int sample_rate) {
    this->psm = psm;
    this->lsm = lsm;
    this->width = width;
    this->height = height;
    this->sample_rate = sample_rate;

    sample_buffer.resize(width * height * sample_rate, Color::White);
  }

  // Used by rasterize_point and rasterize_line
  void RasterizerImp::fill_pixel(size_t x, size_t y, Color c) {
    // TODO: Task 2: You might need to this function to fix points and lines (such as the black rectangle border in test4.svg)
    // NOTE: You are not required to implement proper supersampling for points and lines
    // It is sufficient to use the same color for all supersamples of a pixel for points and lines (not triangles)

    //TODO: FOR POINTS AND LINES, SET ALL SUPERSAMPLE POINTS TO THE COLOR.
    sample_buffer[(int) (y * width * std::sqrt(this->sample_rate)) + (int) (x)] = c;

  }

  // Rasterize a point: simple example to help you start familiarizing
  // yourself with the starter code.
  //
  void RasterizerImp::rasterize_point(float x, float y, Color color) {
    // fill in the nearest pixel
    x *= std::sqrt(this->sample_rate);
    y *= std::sqrt(this->sample_rate);
    int sx = (int)floor(x);
    int sy = (int)floor(y);

    // check bounds
    if (sx < 0 || sx >= width * std::sqrt(this->sample_rate)) return;
    if (sy < 0 || sy >= height * std::sqrt(this->sample_rate)) return;
    for (int i = 0; i < std::sqrt(this->sample_rate); i++) {
        for (int j = 0; j < std::sqrt(this->sample_rate); j++) {
            //adjacent[(int) std::sqrt(this->sample_rate) * i + j] = sample_buffer[((y * std::sqrt(this->sample_rate)) + j) * width * std::sqrt(this->sample_rate) + (x * std::sqrt(this->sample_rate)) + i];
            fill_pixel(sx + i, sy + j, color);
        }
    }
    fill_pixel(sx, sy, color);
    return;
  }

  // Rasterize a line.
  void RasterizerImp::rasterize_line(float x0, float y0,
    float x1, float y1,
    Color color) {
    if (x0 > x1) {
      swap(x0, x1); swap(y0, y1);
    }

    float pt[] = { x0,y0 };
    float m = (y1 - y0) / (x1 - x0);
    float dpt[] = { 1,m };
    int steep = abs(m) > 1;
    if (steep) {
      dpt[0] = x1 == x0 ? 0 : 1 / abs(m);
      dpt[1] = x1 == x0 ? (y1 - y0) / abs(y1 - y0) : m / abs(m);
    }

    while (floor(pt[0]) <= floor(x1) && abs(pt[1] - y0) <= abs(y1 - y0)) {
      rasterize_point(pt[0], pt[1], color);
      pt[0] += dpt[0]; pt[1] += dpt[1];
    }
  }

  // Rasterize a triangle.
  void RasterizerImp::rasterize_triangle(float x0, float y0,
    float x1, float y1,
    float x2, float y2,
    Color color) {
      x0 *= std::sqrt(this->sample_rate);
      x1 *= std::sqrt(this->sample_rate);
      x2 *= std::sqrt(this->sample_rate);
      y0 *= std::sqrt(this->sample_rate);
      y1 *= std::sqrt(this->sample_rate);
      y2 *= std::sqrt(this->sample_rate);
      // TODO: Task 1: Implement basic triangle rasterization here, no supersampling
      float xstart = min(x0, min(x1, x2));
      float ystart;
      if (xstart == x0) {ystart = y0;}
      if (xstart == x1) {ystart = y1;}
      if (xstart == x2) {ystart = y2;}
      float miny = min(y0, min(y1, y2));
      float maxy = max(y0, max(y1, y2));
      int ydir = 1;
      if (maxy == ystart) {
          ydir = -1;
      } else {
          ystart = miny;
      }
      for (int x = xstart; x < width * std::sqrt(this->sample_rate); x+=1) {
          bool last_filled = false;
            for (int y = ystart; y >= 0 && y < height * std::sqrt(this->sample_rate); y+=ydir) {
                float L0 = -(x - x0)*(y1 - y0) + (y - y0)*(x1 - x0);
                float L1 = -(x - x1)*(y2 - y1) + (y - y1)*(x2 - x1);
                float L2 = -(x - x2)*(y0 - y2) + (y - y2)*(x0 - x2);
                if (L0 == 0 || L1 == 0 || L2 == 0) {
                    int sx = (int)floor(x);
                    int sy = (int)floor(y);
                    // check bounds
                    if (sx < 0 || sx >= width * std::sqrt(this->sample_rate)) continue;
                    if (sy < 0 || sy >= height * std::sqrt(this->sample_rate)) continue;
                    fill_pixel(sx, sy, color);
                    last_filled = true;
                    continue;
                }
                else if ((L0 > 0 && L1 > 0 && L2 > 0) || (L0 < 0 && L1 < 0 && L2 < 0)) {
                    int sx = (int)floor(x);
                    int sy = (int)floor(y);
                    // check bounds
                    if (sx < 0 || sx >= width * std::sqrt(this->sample_rate)) continue;
                    if (sy < 0 || sy >= height * std::sqrt(this->sample_rate)) continue;
                    fill_pixel(sx, sy, color);
                    last_filled = true;
                    continue;
                }
                if (last_filled) {
                    break;
                }
            }
        }

    // TODO: Task 2: Update to implement super-sampled rasterization


  }


  void RasterizerImp::rasterize_interpolated_color_triangle(float x0, float y0, Color c0,
    float x1, float y1, Color c1,
    float x2, float y2, Color c2)
  {
    // TODO: Task 4: Rasterize the triangle, calculating barycentric coordinates and using them to interpolate vertex colors across the triangle
    // Hint: You can reuse code from rasterize_triangle
      x0 *= std::sqrt(this->sample_rate);
      x1 *= std::sqrt(this->sample_rate);
      x2 *= std::sqrt(this->sample_rate);
      y0 *= std::sqrt(this->sample_rate);
      y1 *= std::sqrt(this->sample_rate);
      y2 *= std::sqrt(this->sample_rate);
      // TODO: Task 1: Implement basic triangle rasterization here, no supersampling
      float xstart = min(x0, min(x1, x2));
      float ystart;
      if (xstart == x0) {ystart = y0;}
      if (xstart == x1) {ystart = y1;}
      if (xstart == x2) {ystart = y2;}
      float miny = min(y0, min(y1, y2));
      float maxy = max(y0, max(y1, y2));
      int ydir = 1;
      if (maxy == ystart) {
          ydir = -1;
      } else {
          ystart = miny;
      }
      for (int x = xstart; x < width * std::sqrt(this->sample_rate); x+=1) {
          bool last_filled = false;
          for (int y = ystart; y >= 0 && y < height * std::sqrt(this->sample_rate); y+=ydir) {
              //float a = (x-x1*(y-y2)/(y1-y2)-x2+x2*(y-y2)/(y1-y2))/(x0+(-x1*y0+x1*y2)/(y1-y2)-x2-(x2*y0+x2*y2)/(y1-y2));
              //float b = (y - (1 - a - b) * y2 - a * y0) / y1;
              //float c = 1 - a - b;
              float alpha = (-(x-x1)*(y2-y1) + (y-y1)*(x2-x1))/(-(x0-x1)*(y2-y1)+(y0-y1)*(x2-x1));
              float beta = (-(x-x2)*(y0-y2)+(y-y2)*(x0-x2))/(-(x1-x2)*(y0-y2)+(y1-y2)*(x0-x2));
              float gamma = 1-alpha-beta;

              Color color = alpha*c0 + beta*c1 + gamma*c2;

              float L0 = -(x - x0)*(y1 - y0) + (y - y0)*(x1 - x0);
              float L1 = -(x - x1)*(y2 - y1) + (y - y1)*(x2 - x1);
              float L2 = -(x - x2)*(y0 - y2) + (y - y2)*(x0 - x2);
              if (L0 == 0 || L1 == 0 || L2 == 0) {
                  int sx = (int)floor(x);
                  int sy = (int)floor(y);
                  // check bounds
                  if (sx < 0 || sx >= width * std::sqrt(this->sample_rate)) continue;
                  if (sy < 0 || sy >= height * std::sqrt(this->sample_rate)) continue;
                  fill_pixel(sx, sy, color);
                  last_filled = true;
                  continue;
              }
              else if ((L0 > 0 && L1 > 0 && L2 > 0) || (L0 < 0 && L1 < 0 && L2 < 0)) {
                  int sx = (int)floor(x);
                  int sy = (int)floor(y);
                  // check bounds
                  if (sx < 0 || sx >= width * std::sqrt(this->sample_rate)) continue;
                  if (sy < 0 || sy >= height * std::sqrt(this->sample_rate)) continue;
                  fill_pixel(sx, sy, color);
                  last_filled = true;
                  continue;
              }
              if (last_filled) {
                  break;
              }
          }
      }




  }


  void RasterizerImp::rasterize_textured_triangle(float x0, float y0, float u0, float v0,
    float x1, float y1, float u1, float v1,
    float x2, float y2, float u2, float v2,
    Texture& tex)
  {
    // TODO: Task 5: Fill in the SampleParams struct and pass it to the tex.sample function.
    // TODO: Task 6: Set the correct barycentric differentials in the SampleParams struct.
    // Hint: You can reuse code from rasterize_triangle/rasterize_interpolated_color_triangle
      // Hint: You can reuse code from rasterize_triangle
      x0 *= std::sqrt(this->sample_rate);
      x1 *= std::sqrt(this->sample_rate);
      x2 *= std::sqrt(this->sample_rate);
      y0 *= std::sqrt(this->sample_rate);
      y1 *= std::sqrt(this->sample_rate);
      y2 *= std::sqrt(this->sample_rate);
      float xstart = min(x0, min(x1, x2));
      float ystart;
      if (xstart == x0) {ystart = y0;}
      if (xstart == x1) {ystart = y1;}
      if (xstart == x2) {ystart = y2;}
      float miny = min(y0, min(y1, y2));
      float maxy = max(y0, max(y1, y2));
      int ydir = 1;
      if (maxy == ystart) {
          ydir = -1;
      } else {
          ystart = miny;
      }
      for (int x = xstart; x < width * std::sqrt(this->sample_rate); x+=1) {
          bool last_filled = false;
          for (int y = ystart; y >= 0 && y < height * std::sqrt(this->sample_rate); y+=ydir) {

              float alpha = (-(x-x1)*(y2-y1) + (y-y1)*(x2-x1))/(-(x0-x1)*(y2-y1)+(y0-y1)*(x2-x1));
              float beta = (-(x-x2)*(y0-y2)+(y-y2)*(x0-x2))/(-(x1-x2)*(y0-y2)+(y1-y2)*(x0-x2));
              float gamma = 1-alpha-beta;

              float L0 = -(x - x0)*(y1 - y0) + (y - y0)*(x1 - x0);
              float L1 = -(x - x1)*(y2 - y1) + (y - y1)*(x2 - x1);
              float L2 = -(x - x2)*(y0 - y2) + (y - y2)*(x0 - x2);

              if ((L0 == 0 || L1 == 0 || L2 == 0) ||(L0 > 0 && L1 > 0 && L2 > 0) || (L0 < 0 && L1 < 0 && L2 < 0)) {
                  int sx = (int)floor(x);
                  int sy = (int)floor(y);
                  // check bounds
                  if (sx < 0 || sx >= width * std::sqrt(this->sample_rate)) continue;
                  if (sy < 0 || sy >= height * std::sqrt(this->sample_rate)) continue;
                  SampleParams sp;
                  Vector2D uv0 = {u0, v0};
                  Vector2D uv1 = {u1, v1};
                  Vector2D uv2 = {u2, v2};
                  Vector2D uv = alpha*uv0 + beta*uv1 + gamma*uv2;
                  sp.p_uv = uv;

                  alpha = (-(x+1-x1)*(y2-y1) + (y-y1)*(x2-x1))/(-(x0-x1)*(y2-y1)+(y0-y1)*(x2-x1));
                  beta = (-(x+1-x2)*(y0-y2)+(y-y2)*(x0-x2))/(-(x1-x2)*(y0-y2)+(y1-y2)*(x0-x2));
                  gamma = 1-alpha-beta;
                  uv = alpha*uv0 + beta*uv1 + gamma*uv2;
                  sp.p_dx_uv = uv;

                  alpha = (-(x-x1)*(y2-y1) + (y+1-y1)*(x2-x1))/(-(x0-x1)*(y2-y1)+(y0-y1)*(x2-x1));
                  beta = (-(x-x2)*(y0-y2)+(y+1-y2)*(x0-x2))/(-(x1-x2)*(y0-y2)+(y1-y2)*(x0-x2));
                  gamma = 1-alpha-beta;
                  uv = alpha*uv0 + beta*uv1 + gamma*uv2;
                  sp.p_dy_uv = uv;
                  sp.psm = this->psm;
                  sp.lsm = this->lsm;
                  float lvl = tex.get_level(sp);

                  Color color = Color(1, 0, 1);
                  Color color2;
                  if (lsm == 2) {
                      if (psm == 0) {
                          color = tex.sample_nearest(uv, floor(lvl));
                          color2 = tex.sample_nearest(uv, ceil(lvl));
                          color = color * (lvl-floor(lvl)) + color2 * (ceil(lvl)-lvl);
                      } else {
                          color = tex.sample_bilinear(uv, floor(lvl));
                          color2 = tex.sample_bilinear(uv, ceil(lvl));
                          color = color * (lvl-floor(lvl)) + color2 * (ceil(lvl)-lvl);
                      }
                  } else {
                      if (psm == 0) {
                          color = tex.sample_nearest(uv, lvl);
                      } else {
                          color = tex.sample_bilinear(uv, lvl);
                      }
                  }
                  fill_pixel(sx, sy, color);
                  last_filled = true;
                  continue;
              }
              if (last_filled) {
                  break;
              }
          }
      }




  }

  void RasterizerImp::set_sample_rate(unsigned int rate) {
    // TODO: Task 2: You may want to update this function for supersampling support

    this->sample_rate = rate;
    this->sample_buffer.resize(width * height * rate, Color::White);
  }


  void RasterizerImp::set_framebuffer_target(unsigned char* rgb_framebuffer,
    size_t width, size_t height)
  {
    // TODO: Task 2: You may want to update this function for supersampling support

    this->width = width;
    this->height = height;
    this->rgb_framebuffer_target = rgb_framebuffer;
    this->sample_buffer.resize(width * height * this->sample_rate, Color::White);
  }


  void RasterizerImp::clear_buffers() {
    std::fill(rgb_framebuffer_target, rgb_framebuffer_target + 3 * width * height, 255);
    std::fill(sample_buffer.begin(), sample_buffer.end(), Color::White);
  }


  // This function is called at the end of rasterizing all elements of the
  // SVG file.  If you use a supersample buffer to rasterize SVG elements
  // for antialising, you could use this call to fill the target framebuffer
  // pixels from the supersample buffer data.
  //
  void RasterizerImp::resolve_to_framebuffer() {
    // TODO: Task 2: You will likely want to update this function for supersampling support

    for (int x = 0; x < width; ++x) {
      for (int y = 0; y < height; ++y) {
          //Color col = sample_buffer[y * width * this->sample_rate + x * std::sqrt(this->sample_rate)];
          Color colorAvg = Color::Black;
          for (int i = 0; i < std::sqrt(this->sample_rate); i++) {
              for (int j = 0; j < std::sqrt(this->sample_rate); j++) {
                  //adjacent[(int) std::sqrt(this->sample_rate) * i + j] = sample_buffer[((y * std::sqrt(this->sample_rate)) + j) * width * std::sqrt(this->sample_rate) + (x * std::sqrt(this->sample_rate)) + i];
                  colorAvg += sample_buffer[((y * std::sqrt(this->sample_rate)) + j) * width * std::sqrt(this->sample_rate) + (x * std::sqrt(this->sample_rate)) + i];
              }
          }

        for (int k = 0; k < 3; ++k) {
            //Color adjacent[this->sample_rate];

            /**
            float colorAvg = 0;
            for (int i = 0; i < this->sample_rate; i++) {
                Color col = adjacent[i];
                colorAvg += (&col.r)[k] * 255;
            }
            colorAvg = colorAvg / this->sample_rate;
            Color colorAvg;
            for (Color col : adjacent) {
                colorAvg += col;
            } */
            this->rgb_framebuffer_target[3 * (y * width + x) + k] = (&colorAvg.r)[k] * 255 / this->sample_rate;
        }
      }
    }

  }

  Rasterizer::~Rasterizer() { }


}// CGL
