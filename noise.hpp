#ifndef NOISE_H
#define NOISE_H

#include <random>
#include <algorithm>
#include <cstdint>
#include <array>

/*
 * ====== VERSION ======
 * 0.1
 * ====== CHANGELOG ======
 * 1.0 TBA
 */

// TODO: Color mapping module (noise + color scheme --> vector or smt)
// TODO: Doxygen style comments
// TODO: Fix indentation

namespace pn {
  struct vec3 {
    double x, y, z;
    
    vec3(double x, double y, double z): x(x), y(y), z(z) {};
    vec3(): x(0.0), y(0.0), z(0.0) {};
  
    /// Dot product
    inline double dot(const vec3& u) const { return x * u.x + y * u.y + z * u.z; }
    
    /// Length of the vector
    inline double length() const { return std::sqrt(x*x + y*y + z*z); }
    
    /// Returns a copy of this vector normalized
    inline vec3 normalize() const {
      const double lng = length();
      return {x / lng, y / lng, z / lng};
    }
    
    /// Sum of the components of the vector
    inline double sum() const { return x + y + z; }
    
    /// Floors the components
    vec3 floor() const { return {std::floor(x), std::floor(y), std::floor(z)}; }
    
    /// Operators
    inline vec3 operator+(const vec3 &rhs) const { return vec3{x + rhs.x, y + rhs.y, z + rhs.z}; }
    
    inline vec3 operator-(const vec3 &rhs) const { return vec3{x - rhs.x, y - rhs.y, z - rhs.z}; }
  };
  
  struct vec2 {
    double x, y;
    
    vec2(double x, double y): x(x), y(y) {};
    vec2(): x(0.0), y(0.0) {};
    
    /// Dot product
    inline double dot(const vec2& u) const { return x * u.x + y * u.y; }
    
    /// Operators
    vec2 operator+(const vec2& rhs) const { return {x + rhs.x, y + rhs.y}; }
    
    vec2 operator-(const vec2& rhs) const { return {x - rhs.x, y - rhs.y}; }
    
    /// Returns a copy of this vector normalized
    inline vec2 normalize() const {
      const double lng = length();
      return {x / lng, y / lng};
    }
    
    /// Length of the vector
    inline double length() const { return std::sqrt(x*x + y*y); }
  };
  
  /// Vector operations (inspired by glm)
  template<typename T>
  inline double length(const T& v) { return v.length(); }
  
  template<typename T>
  inline double dot(const T& v, const T& u) { return v.dot(u); }
  
  template<typename T>
  inline T normalize(const T& v) { return v.normalize(); }
  
  template<typename T>
  inline T floor(const T& v) { return v.floor(); }
  
  template<typename T>
  inline double sum(const T& v) { return v.sum(); }
  
  /**
   * Base class for noise generating classes
   */
  class generator {
  public:
      /// 2D raw noise from the underlying noise algorithm
      virtual double operator()(const double x, const double y) const = 0;
  
      /// 3D raw noise from the underlying noise algorithm
      virtual double operator()(const double x, const double y, const double z) const = 0;
    
      // FIXME: Is turbulence like defined here really from the original Perlin patent?
      // FIXME: Is it a visually useful effect?
      /// 3D turbulence noise which simulates fBm
      double turbulence(const double x, const double y, const double zoom_factor) const {
        double value = 0;
        double zoom = zoom_factor;
          while (zoom >= 1.0) {
              value += std::abs(operator()(x / zoom, y / zoom) * zoom);
              zoom /= 2;
          }
          return value / zoom_factor;
      }
  
      // FIXME: Is turbulence like defined here really from the original Perlin patent?
      // FIXME: Is it a visually useful effect?
      /// 3D turbulence noise which simulates fBm
      /// Reference: http://lodev.org/cgtutor/randomnoise.html & orignal Perlin noise paper
      double turbulence(const double x, const double y, const double z, const double zoom_factor) const {
        double value = 0;
        double zoom = zoom_factor;
          while (zoom >= 1.0) {
              value += std::abs(operator()(x / zoom, y / zoom, z / zoom) * zoom);
              zoom /= 2;
          }
          return value / zoom_factor;
      }
    
      /// 2D turbulence noise which simulates fBm
      double fbm(const pn::vec2 v, const double zoom_factor) const {
          return fbm(v.x, v.y, zoom_factor);
      }
  
      /// 2D turbulence noise which simulates fBm
      double fbm(const double x, const double y, const double zoom_factor) const {
        double value = 0;
        double zoom = zoom_factor;
          while (zoom >= 1.0) {
              value += operator()(x / zoom, y / zoom) * zoom;
              zoom /= 2;
          }
          return value / zoom_factor;
      }
  
      /// 3D turbulence noise which simulates fBm
      double fbm(const pn::vec3 v, const double zoom_factor) const {
          return fbm(v.x, v.y, v.z, zoom_factor);
      }
    
      /// 3D turbulence noise which simulates fBm
      double fbm(const double x, const double y, const double z, const double zoom_factor) const {
        double value = 0;
        double zoom = zoom_factor;
          while (zoom >= 1.0) {
              value += operator()(x / zoom, y / zoom, z / zoom) * zoom;
              zoom /= 2;
          }
          return value / zoom_factor;
      }
  
      /// 3D Billowy turbulence
      double turbulence_billowy(const double x, const double y, const double z, const double zoom_factor) const {
        double value = 0;
        double zoom = zoom_factor;
          while (zoom >= 1.0) {
              value += std::abs(operator()(x / zoom, y / zoom, z / zoom) * zoom);
              zoom /= 2;
          }
          return value / zoom_factor;
      }
  
      /// 3D Ridged turbulence
      double turbulence_ridged(const double x, const double y, const double z, const double zoom_factor) const {
        double value = 0;
        double zoom = zoom_factor;
          while (zoom >= 1.0) {
              value += (1.0 - std::abs(operator()(x / zoom, y / zoom, z / zoom) * zoom));
              zoom /= 2;
          }
          return value / zoom_factor;
      }
      
      // FIXME: Octaves, is the implementation correct?
      // FIXME: Visually pleasing effect?
      /// 2D fractional Brownian motion noise of the underlying noise algorithm
      double octaves(const double x, const double y, const int octaves, const double persistance = 1.0, double amplitude = 1.0) const {
        double total = 0.0;
        double max_value = 0.0;
        double frequency = 1.0;
          for (size_t i = 0; i < octaves; ++i) {
              total += operator()(x / frequency, y / frequency) * amplitude;
              max_value += amplitude;
  
              amplitude *= persistance;
              frequency *= 2;
          }
  
          // Dividing by the max amplitude sum brings it into [-1, 1] range
          return total / max_value;
      }
  
      /// 3D fractional Brownian motion noise of the underlying noise algorithm
      double octaves(const double x, const double y, const double z, const int octaves, const double persistance = 1.0, double amplitude = 1.0) const {
        double total = 0.0;
        double max_value = 0.0;
        double frequency = 1.0;
          for (size_t i = 0; i < octaves; ++i) {
              total += operator()(x / frequency, y / frequency, z / frequency) * amplitude;
              max_value += amplitude;
  
              amplitude *= persistance;
              frequency *= 2;
          }
  
          // Dividing by the max amplitude sum brings it into [-1, 1] range
          return total / max_value;
      }
  
      /// 3D fractional Brownian motion noise in which each octave gets its own amplitude
      double octaves(const double x, const double y, const double z, const std::vector<double>& amplitudes) const {
        double total = 0.0;
        double max_value = 0.0;
        double frequency = 1.0;
          for (const double& amplitude : amplitudes) {
              total += operator()(x / frequency, y / frequency, z / frequency) * amplitude;
              max_value += amplitude;
              frequency *= 2;
          }
  
          // Dividing by the max amplitude sum brings it into [-1, 1] range
          return total / max_value;
      }
  
      /// Warps the domain of the noise function creating more natural looking features
      double domain_wrapping(const double x, const double y, const double z, const double scale) const {
        pn::vec3 p{x, y, z};
        pn::vec3 offset{50.2, 10.3, 10.5};
  
        pn::vec3 q{fbm(p + offset, scale), fbm(p + offset, scale), fbm(p + offset, scale)};
        pn::vec3 qq{100.0*q.x, 100.0*q.y, 100.0*q.z};
  
        /// Adjusting the scales in r makes a cool ripple effect through the noise
        pn::vec3 r{fbm(p + qq + pn::vec3{1.7f, 9.2f, 5.1f}, scale * 1.0),
                    fbm(p + qq + pn::vec3{8.3f, 2.8f, 2.5f}, scale * 1.0),
                    fbm(p + qq + pn::vec3{1.2f, 6.9f, 8.4f}, scale * 1.0)};
        pn::vec3 rr{100.0*r.x, 100.0*r.y, 100.0*r.z};
  
        return fbm(p + rr, scale);
      }
  
  protected:
      static inline double clamp(double in, double lo, double hi) {
        return std::max(lo, std::min(hi, in));
      }
    
      // TODO: Document
      static inline double smoothstep(const double t) { return t * t * (3 - 2 * t); }
      // TODO: Document
      static inline double quintic_fade(const double t) { return t * t * t * (t * (t * 6 - 15) + 10); }
      /// Linear interpolation between a and b with t as a variable
      static inline double lerp(const double t, const double a, const double b) { return (1 - t) * a + t * b; }
  };
  
  namespace simplex {
      /**
       * Simplex noise/Improved Perlin noise from the 'Improved noise' patent
       * - Gradient creation on-the-fly using bit manipulation
       * - Gradient selection uses bit manipulation (from the above point)
       */
    class patent : public pn::generator {
          /// Bit patterns for the creation of the gradients
          std::array<u_char, 8> bit_patterns;
          
          /// Returns the n'th bit of num
          inline u_char bit(const int num, const int n) const {
              return (u_char) ((num >> n) & 0b1);
          }
      
      public:
          explicit patent(uint64_t seed): bit_patterns{0x15, 0x38, 0x32, 0x2C, 0x0D, 0x13, 0x07, 0x2A} {}
          
          /********************************** Simplex 2D Noise **********************************/
          
          /// Skews the coordinate to normal Euclidean coordinate system
          pn::vec2 skew(const pn::vec2 v) const {
            const double F = (std::sqrtf(1.0 + 2.0) - 1.0) / 2.0;
            const double s = (v.x + v.y) * F;
            return {v.x + s, v.y + s};
          }
          
          /// Unskews the coordinate back to the simpletic coordinate system
          pn::vec2 unskew(const pn::vec2 v) const {
            const double G = (1.0 - (1.0 / sqrt(2.0 + 1.0))) / 2.0;
            const double s = (v.x + v.y) * G;
            return {v.x - s, v.y - s};
          }
          
          /// Given a coordinate (i, j) selects the B'th bit
          u_char b(const int i, const int j, const int B) const {
              const auto bit_index = 2 * (i & (0b1 << B)) + (j & (0b1 << B));
              return bit_patterns[bit_index];
          }
          
          /// Given a coordinate (i, j) generates a gradient vector
          pn::vec2 grad(const int i, const int j) const {
              const uint32_t bit_sum = b(i, j, 0) + b(j, i, 1) + b(i, j, 2) + b(j, i, 3);
              auto u = (bit_sum & 0b01) ? 1.0 : 0.0;
              auto v = (bit_sum & 0b10) ? 1.0 : 0.0;
              u = (bit_sum & 0b1000) ? -u : u;
              v = (bit_sum & 0b0100) ? -v : v;
              return {u, v};
          }
          
          // FIXME: double check against the patent
          double operator()(const double x, const double y) const override {
            /// Skew
            const double F = (std::sqrt(2.0 + 1.0) - 1.0) / 2.0;
            double s = (x + y) * F;
            double xs = x + s;
            double ys = y + s;
            int i = (int) std::floor(xs);
            int j = (int) std::floor(ys);
            
            /// Unskew - find first vertex of the simplex
            const double G = (3.0 - std::sqrt(2.0 + 1.0)) / 6.0;
            double t = (i + j) * G;
            pn::vec2 cell_origin{i - t, j - t};
            pn::vec2 vertex_a = pn::vec2{x, y} - cell_origin;
            
            // Figure out which vertex is next
            auto x_step = 0;
            auto y_step = 0;
            if (vertex_a.x > vertex_a.y) { // Lower triangle
                x_step = 1;
            } else {
                y_step = 1;
            }
            
            // A change of one unit step is; x = x' + (x' + y') * G <--> x = 1.0 + (1.0 + 1.0) * G <--> x = 1.0 + 2.0 * G
            pn::vec2 vertex_b{vertex_a.x - x_step + G, vertex_a.y - y_step + G};
            pn::vec2 vertex_c{vertex_a.x - 1.0 + 2.0 * G, vertex_a.y - 1.0 + 2.0 * G};
            
            auto grad_a = grad(i, j);
            auto grad_b = grad(i + x_step, j + y_step);
            auto grad_c = grad(i + 1, j + 1);
            
            /// Calculate contribution from the vertices in a circle
            // max(0, r^2 - d^2)^4 * gradient.dot(vertex)
            const double radius = 0.6f * 0.6f; // Radius of the surflet circle (0.6 in patent)
            double sum = 0.0;
    
            double t0 = radius - pn::length(vertex_a) * pn::length(vertex_a);
            if (t0 > 0) {
              sum += std::pow(t0, 4) * pn::dot(grad_a, vertex_a);
            }
  
            double t1 = radius - pn::length(vertex_b) * pn::length(vertex_b);
            if (t1 > 0) {
              sum += std::pow(t1, 4) * pn::dot(grad_b, vertex_b);
            }
    
            double t2 = radius - pn::length(vertex_c) * pn::length(vertex_c);
            if (t2 > 0) {
              sum += std::pow(t2, 4) * pn::dot(grad_c, vertex_c);
            }
            
            return 220.0 * sum;
          }
          
          /********************************** Simplex 3D Noise **********************************/
          
          /// Hashes a coordinate (i, j, k) then selects one of the bit patterns
          u_char b(const int i, const int j, const int k, const int B) const {
              const u_char bit_index = bit(i, B) << 2 | bit(j, B) << 2 | bit(k, B);
              return bit_patterns[bit_index];
          }
          
          /**
           * Generates the gradient vector from the vertex and the relative vector.
           * @param vertex Skewed coordinate of the vertex, used to generate the bit sum.
           * @param rel Relative vector of (x, y, z) and the vertex in the unskewed coordinate system.
           * @return Gradient vector
           */
          pn::vec3 grad(const pn::vec3 vertex, const pn::vec3 rel) const {
            const int i = (int) vertex.x;
            const int j = (int) vertex.y;
            const int k = (int) vertex.z;
            int sum = b(i, j, k, 0) + b(j, k, i, 1) + b(k, i, j, 2) + b(i, j, k, 3) + b(j, k, i, 4) + b(k, i, j, 5) +
                      b(i, j, k, 6) + b(j, k, i, 7);
            
            // Magnitude computation based on the three lower bits of the bit sum
            pn::vec3 pqr = rel;
            if (bit(sum, 0) == !bit(sum, 1)) { // xor on bit 0, 1 --> rotation and zeroing
              if (bit(sum, 0)) { // Rotation
                pqr.x = rel.y;
                pqr.y = rel.z;
                pqr.z = rel.x;
              } else {
                pqr.x = rel.z;
                pqr.y = rel.x;
                pqr.z = rel.y;
              }
              
              if (bit(sum, 2)) { // Zeroing out
                pqr.y = 0.0;
              } else {
                pqr.z = 0.0;
              }
            } else if (bit(sum, 0) && bit(sum, 1)) {
              if (bit(sum, 2)) { // Zeroing out
                pqr.y = 0.0;
              } else {
                pqr.z = 0.0;
              }
            }
            
            // Octant computation based on the three upper bits of the bit sum
            if (bit(sum, 5) == bit(sum, 3)) { pqr.x = -pqr.x; }
            if (bit(sum, 5) == bit(sum, 4)) { pqr.y = -pqr.y; }
            if (bit(sum, 5) != (bit(sum, 4) == !bit(sum, 3))) { pqr.z = -pqr.z; }
            
            return pqr;
          }
          
          /// Skews the coordinate to normal Euclidean coordinate system
          pn::vec3 skew(const pn::vec3 v) const {
            const double F = (std::sqrt(1.0 + 3.0) - 1.0) / 3.0;
            const double s = (v.x + v.y + v.z) * F;
            return {v.x + s, v.y + s, v.z + s};
          }
          
          /// Unskews the coordinate back to the simpletic coordinate system
          pn::vec3 unskew(const pn::vec3 v) const {
            const double G = (1.0 - (1.0 / sqrt(3.0 + 1.0))) / 3.0;
            const double s = (v.x + v.y + v.z) * G;
            return {v.x - s, v.y - s, v.z - s};
          }
          
          /**
           * Computes the spherical kernel contribution from one vertex in a simpletic cell offseted by the vector ijk.
           * @param uvw Position within the simplex cell (unskewed)
           * @param ijk First vertex in the simplex cell (unskewed)
           * @param vertex Vertex in the unit simplex cell (unskewed)
           * @return Contribution from the vertex
           */
          double kernel(const pn::vec3 uvw, const pn::vec3 ijk, const pn::vec3 vertex) const {
            double sum = 0.0;
            const pn::vec3 rel = uvw - vertex; // Relative simplex cell vertex
            double t = 0.6 - pn::length(rel) * pn::length(rel); // 0.6 - x*x - y*y - z*z
            if (t > 0) {
              const pn::vec3 pqr = grad(ijk + vertex, rel); // Generate gradient vector for vertex
              t *= t;
              sum += 8 * t * t * pn::sum(pqr);
            }
            return sum;
          }
    
        double operator()(const double x, const double y, const double z) const override {
          /// Skew in the coordinate to the euclidean coordinate system
          pn::vec3 xyz = {x, y, z};
          pn::vec3 xyzs = skew(xyz);
          /// Skewed unit simplex cell
          pn::vec3 ijks = pn::floor(xyzs); // First vertex in euclidean coordinates
          pn::vec3 ijk = unskew(ijks); // First vertex in the simpletic cell
          
          /// Finding the traversal order of vertices of the unit simplex in which (x,y,z) is in.
          pn::vec3 uvw = xyz - ijk; // Relative unit simplex cell origin
          std::array<pn::vec3, 4> vertices{}; // n + 1 is the number of vertices in a n-dim. simplex
          vertices[0] = unskew({0.0, 0.0, 0.0});
          if (uvw.x > uvw.y) {
            if (uvw.y > uvw.z) {
              // u, v, w
              vertices[1] = unskew({1.0, 0.0, 0.0});
              vertices[2] = unskew({1.0, 1.0, 0.0});
            } else {
              if (uvw.x > uvw.z) {
                // u, w, v
                vertices[1] = unskew({1.0, 0.0, 0.0});
                vertices[2] = unskew({1.0, 0.0, 1.0});
              } else {
                // w, u, v
                vertices[1] = unskew({0.0, 0.0, 1.0});
                vertices[2] = unskew({1.0, 0.0, 1.0});
              }
            }
          } else {
            if (uvw.y > uvw.z) {
              if (uvw.z > uvw.x) {
                // v, w, u
                vertices[1] = unskew({0.0, 1.0, 0.0});
                vertices[2] = unskew({0.0, 1.0, 1.0});
              } else {
                // v, u, w
                vertices[1] = unskew({0.0, 1.0, 0.0});
                vertices[2] = unskew({1.0, 1.0, 0.0});
              }
            } else {
              // w, v, u
              vertices[1] = unskew({0.0, 0.0, 1.0});
              vertices[2] = unskew({0.0, 1.0, 1.0});
            }
          }
          vertices[3] = unskew({1.0, 1.0, 1.0});
          
          /// Spherical kernel summation - contribution from each vertex
          double sum = kernel(uvw, ijk, vertices[0]) + kernel(uvw, ijk, vertices[1]) +
                      kernel(uvw, ijk, vertices[2]) + kernel(uvw, ijk, vertices[3]);
          
          return clamp(sum, -1.0, 1.0);
        }
      };
  
      /**
       * Simplex noise implementation using the a hybrid approach with features from simplex & Perlin noise
       * - Gradient table instead of on-the-fly gradient creation, as the original Perlin noise algorithm
       * - Permutation table instead of bit manipulation, unlike the patented algorithm
       * - Using modulo hashing to select the gradients via the permutation table
       */
      template<int num_grads = 256>
    class tables : public pn::generator {
          /// 2D Normalized gradients table
          std::array<pn::vec2, num_grads> grads2;
          
          /// 3D Normalized gradients table
          std::array<pn::vec3, num_grads> grads3;
          
          /// Permutation table for indices to the gradients
          std::array<u_char, num_grads> perms;
      public:
          /// Perms size is double that of grad to avoid index wrapping
          explicit tables(uint64_t seed) {
              std::mt19937 engine(seed);
              std::uniform_real_distribution<double> distr(-1.0, 1.0);
              /// Fill the gradients list with random normalized vectors
              for (int i = 0; i < grads2.size(); i++) {
                const double x = distr(engine);
                const double y = distr(engine);
                const double z = distr(engine);
                auto grad_vector = pn::normalize(pn::vec2{x, y});
                grads2[i] = grad_vector;
                auto grad3_vector = pn::normalize(pn::vec3{x, y, z});
                grads3[i] = grad3_vector;
              }
              
              /// Fill gradient lookup array with random indices to the gradients list
              /// Fill with indices from 0 to perms.size()
              std::iota(perms.begin(), perms.end(), 0);
              
              /// Randomize the order of the indices
              std::shuffle(perms.begin(), perms.end(), engine);
          }
    
        double operator()(const double x, const double y) const override {
          const double F = (std::sqrt(2.0 + 1.0) - 1.0) / 2.0; // F = (sqrt(n + 1) - 1) / n
          double s = (x + y) * F;
          double xs = x + s;
          double ys = y + s;
          const int i = (int) std::floor(xs);
          const int j = (int) std::floor(ys);
          
          const double G = (3.0 - std::sqrt(2.0 + 1.0)) / 6.0; // G = (1 - (1 / sqrt(n + 1)) / n
          double t = (i + j) * G;
          pn::vec2 cell_origin{i - t, j - t};
          pn::vec2 vertex_a = pn::vec2{x, y} - cell_origin;
          
          auto x_step = 0;
          auto y_step = 0;
          if (vertex_a.x > vertex_a.y) { // Lower triangle
              x_step = 1;
          } else {
              y_step = 1;
          }
          
          pn::vec2 vertex_b{vertex_a.x - x_step + G, vertex_a.y - y_step + G};
          pn::vec2 vertex_c{vertex_a.x - 1.0 + 2.0 * G, vertex_a.y - 1.0 + 2.0 * G};
          
          auto ii = i % 255; // FIXME: Bit mask instead? Measure speedup
          auto jj = j % 255;
          auto grad_a = grads2[perms[ii + perms[jj]]];
          auto grad_b = grads2[perms[ii + x_step + perms[jj + y_step]]];
          auto grad_c = grads2[perms[ii + 1 + perms[jj + 1]]];
          
          /// Calculate contribution from the vertices in a circle
          const double radius = 0.6; // Radius of the surflet circle (0.6 in patent)
          double sum = 0.0;
    
          double t0 = radius - pn::length(vertex_a) * pn::length(vertex_a);
          if (t0 > 0) {
            sum += 8 * std::pow(t0, 4) * pn::dot(grad_a, vertex_a);
          }
    
          double t1 = radius - pn::length(vertex_b) * pn::length(vertex_b);
          if (t1 > 0) {
            sum += 8 * std::pow(t1, 4) * pn::dot(grad_b, vertex_b);
          }
    
          double t2 = radius - pn::length(vertex_c) * pn::length(vertex_c);
          if (t2 > 0) {
            sum += 8 * std::pow(t2, 4) * pn::dot(grad_c, vertex_c);
          }
            
            return clamp(sum, -1.0, 1.0);
          }
          
          // TODO: Implement
          double operator()(double x, double y, double z) const override { exit(EXIT_FAILURE); }
      };
  }
  
  namespace perlin {
      /**
       * Improved Perlin noise from 2002
       * ACM: http://dl.acm.org/citation.cfm?id=566636
       *
       * The improvements made by Kenneth Perlin were twofold;
       * 1) Randomly generated gradients changed to static gradients
       * 2) Changed interpolation function
       */
      template<int num_grads = 256>
      class improved : public pn::generator {
          /// 2D Normalized gradients table
          std::array<pn::vec2, 4> grads;
          
          /// 3D Normalized gradients table
          std::array<pn::vec3, 16> grads3;
          
          /// Permutation table for indices to the gradients (2D)
          std::array<u_char, num_grads> perms;
          
          /// Permutation table for indices to the gradients (3D)
          std::array<u_char, num_grads> perms3;
      
      public:
        explicit improved(uint64_t seed) {
              std::mt19937 engine(seed);
              std::uniform_real_distribution<> distr(-1.0, 1.0);
              /// 4 gradients for each edge of a unit square, no need for padding, is power of 2
              grads = {
                      pn::vec2{ 1.0,  0.0},
                      pn::vec2{ 0.0,  1.0},
                      pn::vec2{-1.0,  0.0},
                      pn::vec2{ 0.0, -1.0}
              };
              // FIXME: Is all of the vectors inside grads?
              /// 12 gradients from the center to each edge of a unit cube, 4 duplicated vectors for padding so that the modulo is on a power of 2 (faster)
              grads3 = {
                      pn::vec3{ 1.0,  1.0,  0.0},
                      pn::vec3{-1.0,  1.0,  0.0},
                      pn::vec3{ 1.0, -1.0,  0.0},
                      pn::vec3{-1.0, -1.0,  0.0},
                      pn::vec3{ 1.0,  0.0,  1.0},
                      pn::vec3{-1.0,  0.0,  1.0},
                      pn::vec3{ 1.0,  0.0, -1.0},
                      pn::vec3{-1.0,  0.0, -1.0},
                      pn::vec3{ 0.0,  1.0,  1.0},
                      pn::vec3{ 0.0, -1.0,  1.0},
                      pn::vec3{ 0.0,  1.0, -1.0},
                      pn::vec3{ 0.0, -1.0, -1.0},
                      pn::vec3{ 1.0,  1.0,  0.0},
                      pn::vec3{-1.0,  1.0,  0.0},
                      pn::vec3{ 0.0, -1.0,  1.0},
                      pn::vec3{ 0.0, -1.0, -1.0}
              };
              
              /// Fill gradient lookup array with random indices to the gradients list
              for (int i = 0; i < perms.size(); i++) { perms[i] = i % grads.size(); }
              for (int i = 0; i < perms3.size(); i++) { perms3[i] = i % grads3.size(); }
              
              /// Randomize the order of the indices
              std::shuffle(perms.begin(), perms.end(), engine);
          }
    
        double operator()(double X, double Y) const override {
          /// Compress the coordinates inside the chunk; double part + int part = point coordinate
          X += 0.1;
          Y += 0.1; // Skew coordinates to avoid integer lines becoming zero
          /// Grid points from the chunk in the world
          const int X0 = (int) std::floor(X);
          const int Y0 = (int) std::floor(Y);
          const int X1 = (int) std::ceil(X);
          const int Y1 = (int) std::ceil(Y);
          
          /// Gradients using hashed indices from lookup list
          pn::vec2 x0y0 = grads[perms[(X0 + perms[Y0 % perms.size()]) % perms.size()]];
          pn::vec2 x1y0 = grads[perms[(X1 + perms[Y0 % perms.size()]) % perms.size()]];
          pn::vec2 x0y1 = grads[perms[(X0 + perms[Y1 % perms.size()]) % perms.size()]];
          pn::vec2 x1y1 = grads[perms[(X1 + perms[Y1 % perms.size()]) % perms.size()]];
          
          /// Vectors from gradients to point in unit square
          auto v00 = pn::vec2{X - X0, Y - Y0};
          auto v10 = pn::vec2{X - X1, Y - Y0};
          auto v01 = pn::vec2{X - X0, Y - Y1};
          auto v11 = pn::vec2{X - X1, Y - Y1};
              
              /// Contribution of gradient vectors by dot product between relative vectors and gradients
          double d00 = pn::dot(x0y0, v00);
          double d10 = pn::dot(x1y0, v10);
          double d01 = pn::dot(x0y1, v01);
          double d11 = pn::dot(x1y1, v11);
              
              /// Interpolate dot product values at sample point using polynomial interpolation 6x^5 - 15x^4 + 10x^3
          double yf = Y - Y0; // double offset inside the square [0, 1]
          double xf = X - X0; // double offset inside the square [0, 1]
          
          auto wx = quintic_fade(xf);
          auto wy = quintic_fade(yf);
          
          /// Interpolate along x for the contributions from each of the gradients
          auto xa = lerp(wx, d00, d10);
          auto xb = lerp(wx, d01, d11);
          
          auto val = lerp(wy, xa, xb);
          
          return clamp(val, -1.0, 1.0);
        }
    
        double operator()(const double X, const double Y, const double Z) const override {
          /// Compress the coordinates inside the chunk; double part + int part = point coordinate
          /// Grid points from the chunk in the world
          const int X0 = (int) std::floor(X);
          const int Y0 = (int) std::floor(Y);
          const int X1 = (int) std::ceil(X);
          const int Y1 = (int) std::ceil(Y);
          const int Z0 = (int) std::floor(Z);
          const int Z1 = (int) std::ceil(Z);
          
          /// Gradients using hashed indices from lookup list
          pn::vec3 x0y0z0 = grads3[perms[(X0 + perms[(Y0 + perms[Z0 % perms.size()]) % perms.size()]) % perms.size()]];
          pn::vec3 x1y0z0 = grads3[perms[(X1 + perms[(Y0 + perms[Z0 % perms.size()]) % perms.size()]) % perms.size()]];
          pn::vec3 x0y1z0 = grads3[perms[(X0 + perms[(Y1 + perms[Z0 % perms.size()]) % perms.size()]) % perms.size()]];
          pn::vec3 x1y1z0 = grads3[perms[(X1 + perms[(Y1 + perms[Z0 % perms.size()]) % perms.size()]) % perms.size()]];
          
          pn::vec3 x0y0z1 = grads3[perms[(X0 + perms[(Y0 + perms[Z1 % perms.size()]) % perms.size()]) % perms.size()]];
          pn::vec3 x1y0z1 = grads3[perms[(X1 + perms[(Y0 + perms[Z1 % perms.size()]) % perms.size()]) % perms.size()]];
          pn::vec3 x0y1z1 = grads3[perms[(X0 + perms[(Y1 + perms[Z1 % perms.size()]) % perms.size()]) % perms.size()]];
          pn::vec3 x1y1z1 = grads3[perms[(X1 + perms[(Y1 + perms[Z1 % perms.size()]) % perms.size()]) % perms.size()]];
          
          /// Vectors from gradients to point in unit cube
          auto v000 = pn::vec3{X - X0, Y - Y0, Z - Z0};
          auto v100 = pn::vec3{X - X1, Y - Y0, Z - Z0};
          auto v010 = pn::vec3{X - X0, Y - Y1, Z - Z0};
          auto v110 = pn::vec3{X - X1, Y - Y1, Z - Z0};
          
          auto v001 = pn::vec3{X - X0, Y - Y0, Z - Z1};
          auto v101 = pn::vec3{X - X1, Y - Y0, Z - Z1};
          auto v011 = pn::vec3{X - X0, Y - Y1, Z - Z1};
          auto v111 = pn::vec3{X - X1, Y - Y1, Z - Z1};
          
          /// Contribution of gradient vectors by dot product between relative vectors and gradients
          double d000 = pn::dot(x0y0z0, v000);
          double d100 = pn::dot(x1y0z0, v100);
          double d010 = pn::dot(x0y1z0, v010);
          double d110 = pn::dot(x1y1z0, v110);
    
          double d001 = pn::dot(x0y0z1, v001);
          double d101 = pn::dot(x1y0z1, v101);
          double d011 = pn::dot(x0y1z1, v011);
          double d111 = pn::dot(x1y1z1, v111);
              
              /// Interpolate dot product values at sample point using polynomial interpolation 6x^5 - 15x^4 + 10x^3
          double yf = Y - Y0; // double offset inside the cube [0, 1]
          double xf = X - X0; // double offset inside the cube [0, 1]
          double zf = Z - Z0; // double offset inside the cube [0, 1]
          
          auto wx = quintic_fade(xf);
          auto wy = quintic_fade(yf);
          auto wz = quintic_fade(zf);
          
          /// Interpolate along x for the contributions from each of the gradients
          auto xa = lerp(wx, d000, d100);
          auto xb = lerp(wx, d010, d110);
          
          auto xc = lerp(wx, d001, d101);
          auto xd = lerp(wx, d011, d111);
          
          /// Interpolate along y for the contributions from each of the gradients
          auto ya = lerp(wy, xa, xb);
          auto yb = lerp(wy, xc, xd);
          
          /// Interpolate along z for the contributions from each of the gradients
          auto za = lerp(wz, ya, yb);
          
          return clamp(za, -1.0, 1.0);
        }
      };
  
    /**
     * Original Perlin noise from 1985
     * ACM: http://dl.acm.org/citation.cfm?id=325247&CFID=927914208&CFTOKEN=31672107
     */
  class Original : public pn::generator {
      /// 2D Normalized gradients table
      std::vector<pn::vec2> grads;
      
      /// 3D Normalized gradients table
      std::vector<pn::vec3> grads3;
      
      /// Permutation table for indices to the gradients
      std::vector<u_char> perms;
      
    public:
      Original(uint64_t seed) : grads(256), grads3(256), perms(256) {
        std::mt19937 engine(seed);
        std::uniform_real_distribution<double> distr(-1.0, 1.0);
        /// Fill the gradients list with random normalized vectors
        for (int i = 0; i < grads.size(); i++) {
          const double x = distr(engine);
          const double y = distr(engine);
          const double z = distr(engine);
          auto grad_vector = pn::normalize(pn::vec2{x, y});
          grads[i] = grad_vector;
          auto grad3_vector = pn::normalize(pn::vec3{x, y, z});
          grads3[i] = grad3_vector;
        }
        
        /// Fill gradient lookup array with random indices to the gradients list
        /// Fill with indices from 0 to perms.size()
        std::iota(perms.begin(), perms.end(), 0);
        
        /// Randomize the order of the indices
        std::shuffle(perms.begin(), perms.end(), engine);
      }
    
      double operator()(double X, double Y) const override {
        /// Compress the coordinates inside the chunk; double part + int part = point coordinate
        X += 0.1;
        Y += 0.1; // Skew coordinates to avoid integer lines becoming zero
        /// Grid points from the chunk in the world
        const int X0 = (int) std::floor(X);
        const int Y0 = (int) std::floor(Y);
        const int X1 = (int) std::ceil(X);
        const int Y1 = (int) std::ceil(Y);
        
        /// Gradients using hashed indices from lookup list
        // FIXME: Implement variation where perms.size() is a power of two in order to do a bit masking instead, measure speedup.
        pn::vec2 x0y0 = grads[perms[(X0 + perms[Y0 % perms.size()]) % perms.size()]];
        pn::vec2 x1y0 = grads[perms[(X1 + perms[Y0 % perms.size()]) % perms.size()]];
        pn::vec2 x0y1 = grads[perms[(X0 + perms[Y1 % perms.size()]) % perms.size()]];
        pn::vec2 x1y1 = grads[perms[(X1 + perms[Y1 % perms.size()]) % perms.size()]];
        
        /// Vectors from gradients to point in unit square
        auto v00 = pn::vec2{X - X0, Y - Y0};
        auto v10 = pn::vec2{X - X1, Y - Y0};
        auto v01 = pn::vec2{X - X0, Y - Y1};
        auto v11 = pn::vec2{X - X1, Y - Y1};
        
        /// Contribution of gradient vectors by dot product between relative vectors and gradients
        double d00 = pn::dot(x0y0, v00);
        double d10 = pn::dot(x1y0, v10);
        double d01 = pn::dot(x0y1, v01);
        double d11 = pn::dot(x1y1, v11);
        
        /// Interpolate dot product values at sample point using polynomial interpolation 6x^5 - 15x^4 + 10x^3
        double yf = Y - Y0; // double offset inside the square [0, 1]
        double xf = X - X0; // double offset inside the square [0, 1]
        
        auto wx = smoothstep(xf);
        auto wy = smoothstep(yf);
        
        /// Interpolate along x for the contributions from each of the gradients
        auto xa = lerp(wx, d00, d10);
        auto xb = lerp(wx, d01, d11);
        
        auto val = lerp(wy, xa, xb);
        
        return clamp(val, -1.0, 1.0);
      }
      
      double operator()(const double X, const double Y, const double Z) const override {
        /// Compress the coordinates inside the chunk; double part + int part = point coordinate
        /// Grid points from the chunk in the world
        const int X0 = (int) std::floor(X);
        const int Y0 = (int) std::floor(Y);
        const int X1 = (int) std::ceil(X);
        const int Y1 = (int) std::ceil(Y);
        const int Z0 = (int) std::floor(Z);
        const int Z1 = (int) std::ceil(Z);
        
        /// Gradients using hashed indices from lookup list
        pn::vec3 x0y0z0 = grads3[perms[(X0 + perms[(Y0 + perms[Z0 % perms.size()]) % perms.size()]) % perms.size()]];
        pn::vec3 x1y0z0 = grads3[perms[(X1 + perms[(Y0 + perms[Z0 % perms.size()]) % perms.size()]) % perms.size()]];
        pn::vec3 x0y1z0 = grads3[perms[(X0 + perms[(Y1 + perms[Z0 % perms.size()]) % perms.size()]) % perms.size()]];
        pn::vec3 x1y1z0 = grads3[perms[(X1 + perms[(Y1 + perms[Z0 % perms.size()]) % perms.size()]) % perms.size()]];
        
        pn::vec3 x0y0z1 = grads3[perms[(X0 + perms[(Y0 + perms[Z1 % perms.size()]) % perms.size()]) % perms.size()]];
        pn::vec3 x1y0z1 = grads3[perms[(X1 + perms[(Y0 + perms[Z1 % perms.size()]) % perms.size()]) % perms.size()]];
        pn::vec3 x0y1z1 = grads3[perms[(X0 + perms[(Y1 + perms[Z1 % perms.size()]) % perms.size()]) % perms.size()]];
        pn::vec3 x1y1z1 = grads3[perms[(X1 + perms[(Y1 + perms[Z1 % perms.size()]) % perms.size()]) % perms.size()]];
        
        /// Vectors from gradients to point in unit cube
        auto v000 = pn::vec3{X - X0, Y - Y0, Z - Z0};
        auto v100 = pn::vec3{X - X1, Y - Y0, Z - Z0};
        auto v010 = pn::vec3{X - X0, Y - Y1, Z - Z0};
        auto v110 = pn::vec3{X - X1, Y - Y1, Z - Z0};
        
        auto v001 = pn::vec3{X - X0, Y - Y0, Z - Z1};
        auto v101 = pn::vec3{X - X1, Y - Y0, Z - Z1};
        auto v011 = pn::vec3{X - X0, Y - Y1, Z - Z1};
        auto v111 = pn::vec3{X - X1, Y - Y1, Z - Z1};
            
            /// Contribution of gradient vectors by dot product between relative vectors and gradients
        double d000 = pn::dot(x0y0z0, v000);
        double d100 = pn::dot(x1y0z0, v100);
        double d010 = pn::dot(x0y1z0, v010);
        double d110 = pn::dot(x1y1z0, v110);
  
        double d001 = pn::dot(x0y0z1, v001);
        double d101 = pn::dot(x1y0z1, v101);
        double d011 = pn::dot(x0y1z1, v011);
        double d111 = pn::dot(x1y1z1, v111);
        
        /// Interpolate dot product values at sample point using polynomial interpolation 6x^5 - 15x^4 + 10x^3
        double yf = Y - Y0; // double offset inside the cube [0, 1]
        double xf = X - X0; // double offset inside the cube [0, 1]
        double zf = Z - Z0; // double offset inside the cube [0, 1]
        
        auto wx = smoothstep(xf);
        auto wy = smoothstep(yf);
        auto wz = smoothstep(zf);
        
        /// Interpolate along x for the contributions from each of the gradients
        auto xa = lerp(wx, d000, d100);
        auto xb = lerp(wx, d010, d110);
        
        auto xc = lerp(wx, d001, d101);
        auto xd = lerp(wx, d011, d111);
        
        /// Interpolate along y for the contributions from each of the gradients
        auto ya = lerp(wy, xa, xb);
        auto yb = lerp(wy, xc, xd);
        
        /// Interpolate along z for the contributions from each of the gradients
        auto za = lerp(wz, ya, yb);
        
        return clamp(za, -1.0, 1.0);
      }
    };
  }
}

#endif // NOISE_H