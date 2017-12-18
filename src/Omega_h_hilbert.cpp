#include "Omega_h_hilbert.hpp"

#include "Omega_h_bbox.hpp"
#include "Omega_h_loop.hpp"
#include "Omega_h_sort.hpp"

namespace Omega_h {

namespace hilbert {

/* for each set of (dim) floating-point coordinates, this function
   outputs a set of (dim) 64-bit integers which represent the
   closest point of a fine-grid Hilbert curve to the coordinates.
   the resolution of the grid is chosen to be 52 bits (the floating-point
   mantissa size), giving 2^52 grid points per axis,
   and is scaled to the bounding box of the coordinates.
   the output integers are such that sort_by_keys() will sort along the
   Hilbert curve.
   More precisely, the bits of the Hilbert distance are spread evenly
   among the integers, with the first integer getting the most significant
   bits, and the last integer getting the least significant bits. */

template <Int dim>
static Read<I64> dists_from_coords_dim(Reals coords) {
  auto bbox = find_bounding_box<dim>(coords);
  Real maxl = 0;
  for (Int i = 0; i < dim; ++i) maxl = max2(maxl, bbox.max[i] - bbox.min[i]);
  LO npts = coords.size() / dim;
  Write<I64> out(npts * dim);
  auto f = OMEGA_H_LAMBDA(LO i) {
    constexpr Int nbits = MANTISSA_BITS;
    auto spatial_coord = get_vector<dim>(coords, i);
    auto hilbert_coord =
        hilbert::from_spatial(bbox.min, maxl, nbits, spatial_coord);
    for (Int j = 0; j < dim; ++j) {
      out[i * dim + j] = static_cast<I64>(hilbert_coord[j]);
    }
  };
  parallel_for(npts, f, "hilbert::dists_from_coords");
  return out;
}

static Read<I64> dists_from_coords(Reals coords, Int dim) {
  if (dim == 3) return dists_from_coords_dim<3>(coords);
  if (dim == 2) return dists_from_coords_dim<2>(coords);
  if (dim == 1) return dists_from_coords_dim<1>(coords);
  OMEGA_H_NORETURN(Read<I64>());
}

LOs sort_coords(Reals coords, Int dim) {
  auto keys = hilbert::dists_from_coords(coords, dim);
  return sort_by_keys(keys, dim);
}

}  // end namespace hilbert

}  // end namespace Omega_h
