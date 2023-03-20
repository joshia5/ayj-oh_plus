#include "Omega_h_mesh.hpp"
#include "Omega_h_beziers.hpp"
#include "Omega_h_element.hpp"
#include "Omega_h_for.hpp"
#include "Omega_h_atomics.hpp"
#include "Omega_h_map.hpp"
#include "Omega_h_array_ops.hpp"
#include "Omega_h_vector.hpp"

namespace Omega_h {

Real B0_quad(Real u) {
  return (1.0-u)*(1.0-u);
}

Real B1_quad(Real u) {
  return 2.0*u*(1.0-u);
}

Real B2_quad(Real u) {
  return u*u;
}

Real B0_cube(Real u) {
  return (1.0-u)*(1.0-u)*(1.0-u);
}

Real B1_cube(Real u) {
  return 3.0*u*(1.0-u)*(1.0-u);
}

Real B2_cube(Real u) {
  return 3.0*(1.0-u)*u*u;
}

Real B3_cube(Real u) {
  return u*u*u;
}

Real B0_quart(Real u) {
  return (1.0-u)*(1.0-u)*(1.0-u)*(1.0-u);
}

Real B1_quart(Real u) {
  return 4.0*u*(1.0-u)*(1.0-u)*(1.0-u);
}

Real B2_quart(Real u) {
  return 6.0*u*u*(1.0-u)*(1.0-u);
}

Real B3_quart(Real u) {
  return 4.0*u*u*u*(1.0-u);
}

Real B4_quart(Real u) {
  return u*u*u*u;
}

Real B00_quad(Real u, Real v) {
  return (1.0-u-v)*(1.0-u-v);
}

Real B10_quad(Real u, Real v) {
  return 2.0*u*(1.0-u-v);
}

Real B20_quad(Real u, Real v) {
  return u*u + 0.0*v;
}

Real B11_quad(Real u, Real v) {
  return 2.0*u*v;
}

Real B02_quad(Real u, Real v) {
  return v*v + 0.0*u;
}

Real B01_quad(Real u, Real v) {
  return 2.0*v*(1.0-u-v);
}

Real B00_cube(Real u, Real v) {
  return (1.0-u-v)*(1.0-u-v)*(1.0-u-v);
}

Real B10_cube(Real u, Real v) {
  return 3.0*u*(1.0-u-v)*(1.0-u-v);
}

Real B20_cube(Real u, Real v) {
  return 3.0*u*u*(1.0-u-v);
}

Real B30_cube(Real u, Real v) {
  return u*u*u + 0.0*v;
}

Real B21_cube(Real u, Real v) {
  return 3.0*u*u*v;
}

Real B12_cube(Real u, Real v) {
  return 3.0*u*v*v;
}

Real B03_cube(Real u, Real v) {
  return v*v*v + 0.0*u;
}

Real B02_cube(Real u, Real v) {
  return 3.0*v*v*(1.0-u-v);
}

Real B01_cube(Real u, Real v) {
  return 3.0*v*(1.0-u-v)*(1.0-u-v);
}

Real B11_cube(Real u, Real v) {
  return 6.0*u*v*(1.0-u-v);
}

Real B00_quart(Real u, Real v) {
  return (1.0-u-v)*(1.0-u-v)*(1.0-u-v)*(1.0-u-v);
}

Real B10_quart(Real u, Real v) {
  return 4.0*u*(1.0-u-v)*(1.0-u-v)*(1.0-u-v);
}

Real B20_quart(Real u, Real v) {
  return 6.0*u*u*(1.0-u-v)*(1.0-u-v);
}

Real B30_quart(Real u, Real v) {
  return 4.0*u*u*u*(1.0-u-v);
}

Real B40_quart(Real u, Real v) {
  return u*u*u*u + 0.0*v;
}

Real B31_quart(Real u, Real v) {
  return 4.0*u*u*u*v;
}

Real B22_quart(Real u, Real v) {
  return 6.0*u*u*v*v;
}

Real B13_quart(Real u, Real v) {
  return 4.0*u*v*v*v;
}

Real B04_quart(Real u, Real v) {
  return v*v*v*v + 0.0*u;
}

Real B03_quart(Real u, Real v) {
  return 4.0*v*v*v*(1.0-u-v);
}

Real B02_quart(Real u, Real v) {
  return 6.0*v*v*(1.0-u-v)*(1.0-u-v);
}

Real B01_quart(Real u, Real v) {
  return 4.0*v*(1.0-u-v)*(1.0-u-v)*(1.0-u-v);
}

Real B11_quart(Real u, Real v) {
  return 12.0*u*v*(1.0-u-v)*(1.0-u-v);
}

Real B21_quart(Real u, Real v) {
  return 12.0*u*u*v*(1.0-u-v);
}

Real B12_quart(Real u, Real v) {
  return 12.0*u*v*v*(1.0-u-v);
}

Real xi_1_quad() {
 return 0.5;
}

// Babushka chen points
Real xi_1_cube() {
 return 0.2748043;
}
Real xi_2_cube() {
  return 0.7251957;
}

Real xi_1_quart() {
 return 0.1693976;
}
Real xi_2_quart() {
 return 0.5;
}
Real xi_3_quart() {
 return 0.8306024;
}

Real xi_1_quint() {
 return 0.1133573;
}
Real xi_2_quint() {
 return 0.3568239;
}
Real xi_3_quint() {
 return 0.6431761;
}
Real xi_4_quint() {
  return 0.8866427;
}

Reals xi_11_cube() {
  Write<Real> xi_11(2);
    xi_11[0] = 1.0/3.0;
    xi_11[1] = 1.0/3.0;
 return Reals(xi_11);
}

void calc_quad_ctrlPts_from_interpPts(Mesh *mesh) {
  auto coords = mesh->coords();
  auto interpPts = mesh->get_ctrlPts(1);
  auto ev2v = mesh->get_adj(1, 0).ab2b;
  auto dim = mesh->dim();
  auto nedge = mesh->nedges();
  Real xi_1 = 0.5;
  Write<Real> new_pts(nedge*dim, 0.0);

  auto f = OMEGA_H_LAMBDA (LO i) {
    auto v0 = ev2v[i*2];
    auto v1 = ev2v[i*2 + 1];

    //TODO make this a templated fn like cal_pts_tmpl
    if (dim == 2) {
      Vector<2> c1;
      auto c0 = get_vector<2>(coords, v0);
      auto p1 = get_vector<2>(interpPts, i);
      auto c2 = get_vector<2>(coords, v1);
      for (Int j = 0; j < dim; ++j) {
        c1[j] = (p1[j] - B0_quad(xi_1)*c0[j] - B2_quad(xi_1)*c2[j])/B1_quad(xi_1);
      }
      set_vector(new_pts, i, c1);
    }
    else {
      OMEGA_H_CHECK(dim == 3);
      Vector<3> c1;
      auto c0 = get_vector<3>(coords, v0);
      auto p1 = get_vector<3>(interpPts, i);
      auto c2 = get_vector<3>(coords, v1);
      for (Int j = 0; j < dim; ++j) {
        c1[j] = (p1[j] - B0_quad(xi_1)*c0[j] - B2_quad(xi_1)*c2[j])/B1_quad(xi_1);
      }
      set_vector(new_pts, i, c1);
    }
  };
  parallel_for(nedge, std::move(f));

  mesh->set_tag_for_ctrlPts(1, Reals(new_pts));
  return;
}

void elevate_curve_order_2to3(Mesh* mesh) {
  I8 new_order = 3;
  auto old_ctrl_pts = mesh->get_ctrlPts(1);
  auto old_n_ctrl_pts = mesh->n_internal_ctrlPts(1);
  auto coords = mesh->coords();
  auto nedge = mesh->nedges();
  auto dim = mesh->dim();
  auto ev2v = mesh->get_adj(1, 0).ab2b;
  auto fe2e = mesh->get_adj(2, 1).ab2b;

  mesh->change_max_order(new_order);
  auto n_new_pts = mesh->n_internal_ctrlPts(1);
  Write<Real> new_pts(nedge*n_new_pts*dim, 0.0);
  Write<Real> c1(dim, 0.0);
  Write<Real> c2(dim, 0.0);
  auto calc_edge_pts = OMEGA_H_LAMBDA (LO i) {
    auto v0 = ev2v[i*2];
    auto v1 = ev2v[i*2 + 1];
    for (LO d = 0; d < dim; ++d) {
      c1[d] = (1.0/3.0)*coords[v0*dim + d] +
              (2.0/3.0)*old_ctrl_pts[i*old_n_ctrl_pts*dim + d];
      c2[d] = (2.0/3.0)*old_ctrl_pts[i*old_n_ctrl_pts*dim + d] +
              (1.0/3.0)*coords[v1*dim + d];
      new_pts[i*n_new_pts*dim + d] = c1[d];
      new_pts[i*n_new_pts*dim + dim + d] = c2[d];
    }
  };
  parallel_for(nedge, calc_edge_pts);
  mesh->set_tag_for_ctrlPts(1, Reals(new_pts));

  auto nface = mesh->nfaces();
  n_new_pts = mesh->n_internal_ctrlPts(2);
  Write<Real> face_pts(nface*n_new_pts*dim, 0.0);
  if (!mesh->has_tag(2, "interp_pts")) {
    Write<Real> c11(dim, 0.0);
    auto calc_face_pts = OMEGA_H_LAMBDA (LO i) {
      auto e0 = fe2e[i*3];
      auto e1 = fe2e[i*3 + 1];
      auto e2 = fe2e[i*3 + 2];
      for (LO d = 0; d < dim; ++d) {
        c11[d] = (1.0/3.0)*old_ctrl_pts[e0*old_n_ctrl_pts*dim + d] +
          (1.0/3.0)*old_ctrl_pts[e1*old_n_ctrl_pts*dim + d] +
          (1.0/3.0)*old_ctrl_pts[e2*old_n_ctrl_pts*dim + d];
        face_pts[i*n_new_pts*dim + d] = c11[d];
      }
    };
    parallel_for(nface, calc_face_pts);
  }
  mesh->set_tag_for_ctrlPts(2, Reals(face_pts));

  return;
}

void elevate_curve_order_3to4(Mesh* mesh) {
  I8 new_order = 4;
  auto old_edge_ctrl_pts = mesh->get_ctrlPts(1);
  auto pts_per_edge = mesh->n_internal_ctrlPts(1);
  auto old_face_ctrlPts = mesh->get_ctrlPts(2);
  auto old_face_n_ctrl_pts = mesh->n_internal_ctrlPts(2);
  auto coords = mesh->coords();
  auto nedge = mesh->nedges();
  auto dim = mesh->dim();
  auto ev2v = mesh->get_adj(1, 0).ab2b;
  auto fe2e = mesh->get_adj(2, 1).ab2b;
  auto fv2v = mesh->ask_down(2, 0).ab2b;

  mesh->change_max_order(new_order);
  auto n_new_pts = mesh->n_internal_ctrlPts(1);
  Write<Real> new_pts(nedge*n_new_pts*dim, 0.0);
  Write<Real> c1(dim, 0.0);
  Write<Real> c2(dim, 0.0);
  Write<Real> c3(dim, 0.0);
  auto calc_pts = OMEGA_H_LAMBDA (LO i) {
    auto v0 = ev2v[i*2];
    auto v1 = ev2v[i*2 + 1];
    for (LO d = 0; d < dim; ++d) {
      c1[d] = (1.0/4.0)*coords[v0*dim + d] +
              (3.0/4.0)*old_edge_ctrl_pts[i*pts_per_edge*dim + d];
      c2[d] = (1.0/2.0)*old_edge_ctrl_pts[i*pts_per_edge*dim + d] +
              (1.0/2.0)*old_edge_ctrl_pts[i*pts_per_edge*dim + 1*dim + d];
      c3[d] = (3.0/4.0)*old_edge_ctrl_pts[i*pts_per_edge*dim + 1*dim + d] +
              (1.0/4.0)*coords[v1*dim + d];
      new_pts[i*n_new_pts*dim + d] = c1[d];
      new_pts[i*n_new_pts*dim + 1*dim + d] = c2[d];
      new_pts[i*n_new_pts*dim + 2*dim + d] = c3[d];
    }
  };
  parallel_for(nedge, std::move(calc_pts));

  mesh->set_tag_for_ctrlPts(1, Reals(new_pts));

  auto nface = mesh->nfaces();
  auto n_new_face_pts = mesh->n_internal_ctrlPts(2);
  Write<Real> face_pts(nface*n_new_face_pts*dim, 0.0);

  auto calc_face_pts = OMEGA_H_LAMBDA (LO i) {
    auto e0 = fe2e[i*3];
    auto e1 = fe2e[i*3 + 1];
    auto e2 = fe2e[i*3 + 2];
    auto v0 = fv2v[i*3];
    auto v1 = fv2v[i*3 + 1];
    auto v2 = fv2v[i*3 + 2];

    I8 e0_flip = -1;
    I8 e1_flip = -1;
    I8 e2_flip = -1;

    auto e0v0 = ev2v[e0*2 + 0];
    auto e0v1 = ev2v[e0*2 + 1];
    auto e1v0 = ev2v[e1*2 + 0];
    auto e1v1 = ev2v[e1*2 + 1];
    auto e2v0 = ev2v[e2*2 + 0];
    auto e2v1 = ev2v[e2*2 + 1];
    if ((e0v0 == v1) && (e0v1 == v0)) {
      e0_flip = 1;
    }
    else {
      OMEGA_H_CHECK((e0v0 == v0) && (e0v1 == v1));
    }
    if ((e1v0 == v2) && (e1v1 == v1)) {
      e1_flip = 1;
    }
    else {
      OMEGA_H_CHECK((e1v0 == v1) && (e1v1 == v2));
    }
    if ((e2v0 == v0) && (e2v1 == v2)) {
      e2_flip = 1;
    }
    else {
      OMEGA_H_CHECK((e2v0 == v2) && (e2v1 == v0));
    }

    Real cx10 = old_edge_ctrl_pts[e0*pts_per_edge*dim + 0];
    Real cy10 = old_edge_ctrl_pts[e0*pts_per_edge*dim + 1];
    Real cz10 = old_edge_ctrl_pts[e0*pts_per_edge*dim + 2];
    Real cx20 = old_edge_ctrl_pts[e0*pts_per_edge*dim + dim + 0];
    Real cy20 = old_edge_ctrl_pts[e0*pts_per_edge*dim + dim + 1];
    Real cz20 = old_edge_ctrl_pts[e0*pts_per_edge*dim + dim + 2];
    if (e0_flip > 0) {
      auto tempx = cx10;
      auto tempy = cy10;
      auto tempz = cz10;
      cx10 = cx20;
      cy10 = cy20;
      cz10 = cz20;
      cx20 = tempx;
      cy20 = tempy;
      cz20 = tempz;
    }

    Real cx21 = old_edge_ctrl_pts[e1*pts_per_edge*dim + 0];
    Real cy21 = old_edge_ctrl_pts[e1*pts_per_edge*dim + 1];
    Real cz21 = old_edge_ctrl_pts[e1*pts_per_edge*dim + 2];
    Real cx12 = old_edge_ctrl_pts[e1*pts_per_edge*dim + dim + 0];
    Real cy12 = old_edge_ctrl_pts[e1*pts_per_edge*dim + dim + 1];
    Real cz12 = old_edge_ctrl_pts[e1*pts_per_edge*dim + dim + 2];
    if (e1_flip > 0) {
      auto tempx = cx21;
      auto tempy = cy21;
      auto tempz = cz21;
      cx21 = cx12;
      cy21 = cy12;
      cz21 = cz12;
      cx12 = tempx;
      cy12 = tempy;
      cz12 = tempz;
    }

    Real cx02 = old_edge_ctrl_pts[e2*pts_per_edge*dim + 0];
    Real cy02 = old_edge_ctrl_pts[e2*pts_per_edge*dim + 1];
    Real cz02 = old_edge_ctrl_pts[e2*pts_per_edge*dim + 2];
    Real cx01 = old_edge_ctrl_pts[e2*pts_per_edge*dim + dim + 0];
    Real cy01 = old_edge_ctrl_pts[e2*pts_per_edge*dim + dim + 1];
    Real cz01 = old_edge_ctrl_pts[e2*pts_per_edge*dim + dim + 2];
    if (e2_flip > 0) {
      auto tempx = cx02;
      auto tempy = cy02;
      auto tempz = cz02;
      cx02 = cx01;
      cy02 = cy01;
      cz02 = cz01;
      cx01 = tempx;
      cy01 = tempy;
      cz01 = tempz;
    }

    auto oldc11 = Reals({old_face_ctrlPts[i*old_face_n_ctrl_pts*dim + 0],
                         old_face_ctrlPts[i*old_face_n_ctrl_pts*dim + 1],
                         old_face_ctrlPts[i*old_face_n_ctrl_pts*dim + 2]});

    Write<Real> c11(dim, 0.0);
    Write<Real> c21(dim, 0.0);
    Write<Real> c12(dim, 0.0);

    c11[0] = (1.0/4.0)*(cx10 + cx01 + 2.0*oldc11[0]);
    c11[1] = (1.0/4.0)*(cy10 + cy01 + 2.0*oldc11[1]);
    c11[2] = (1.0/4.0)*(cz10 + cz01 + 2.0*oldc11[2]);
    c21[0] = (1.0/4.0)*(cx12 + cx02 + 2.0*oldc11[0]);
    c21[1] = (1.0/4.0)*(cy12 + cy02 + 2.0*oldc11[1]);
    c21[2] = (1.0/4.0)*(cz12 + cz02 + 2.0*oldc11[2]);
    c12[0] = (1.0/4.0)*(cx20 + cx21 + 2.0*oldc11[0]);
    c12[1] = (1.0/4.0)*(cy20 + cy21 + 2.0*oldc11[1]);
    c12[2] = (1.0/4.0)*(cz20 + cz21 + 2.0*oldc11[2]);

    for (LO d = 0; d < dim; ++d) {
      face_pts[i*n_new_face_pts*dim + d] = c11[d];
      face_pts[i*n_new_face_pts*dim + dim + d] = c21[d];
      face_pts[i*n_new_face_pts*dim + dim + dim + d] = c12[d];
    }
  };
  parallel_for(nface, std::move(calc_face_pts));
  mesh->set_tag_for_ctrlPts(2, Reals(face_pts));

  //formulae to calc pts inside regions?

  return;
}

void elevate_curve_order_4to5(Mesh* mesh) {
  I8 new_order = 5;
  auto old_ctrl_pts = mesh->get_ctrlPts(1);
  auto old_n_ctrl_pts = mesh->n_internal_ctrlPts(1);
  auto coords = mesh->coords();
  auto nedge = mesh->nedges();
  auto dim = mesh->dim();
  auto ev2v = mesh->get_adj(1, 0).ab2b;

  mesh->change_max_order(new_order);
  auto n_new_pts = mesh->n_internal_ctrlPts(1);
  Write<Real> new_pts(nedge*n_new_pts*dim, 0.0);
  Write<Real> c1(dim, 0.0);
  Write<Real> c2(dim, 0.0);
  Write<Real> c3(dim, 0.0);
  Write<Real> c4(dim, 0.0);
  auto calc_pts = OMEGA_H_LAMBDA (LO i) {
    auto v0 = ev2v[i*2];
    auto v1 = ev2v[i*2 + 1];
    for (LO d = 0; d < dim; ++d) {
      c1[d] = (1.0/5.0)*coords[v0*dim + d] +
              (4.0/5.0)*old_ctrl_pts[i*old_n_ctrl_pts*dim + d];
      c2[d] = (2.0/5.0)*old_ctrl_pts[i*old_n_ctrl_pts*dim + d] +
              (3.0/5.0)*old_ctrl_pts[i*old_n_ctrl_pts*dim + 1*dim + d];
      c3[d] = (3.0/5.0)*old_ctrl_pts[i*old_n_ctrl_pts*dim + 1*dim + d] +
              (2.0/5.0)*old_ctrl_pts[i*old_n_ctrl_pts*dim + 2*dim + d];
      c4[d] = (4.0/5.0)*old_ctrl_pts[i*old_n_ctrl_pts*dim + 2*dim + d] +
              (1.0/5.0)*coords[v1*dim + d];
      new_pts[i*n_new_pts*dim + d] = c1[d];
      new_pts[i*n_new_pts*dim + 1*dim + d] = c2[d];
      new_pts[i*n_new_pts*dim + 2*dim + d] = c3[d];
      new_pts[i*n_new_pts*dim + 3*dim + d] = c4[d];
    }
  };
  parallel_for(nedge, std::move(calc_pts));

  mesh->set_tag_for_ctrlPts(1, Reals(new_pts));
  return;
}

void elevate_curve_order_5to6(Mesh* mesh) {
  I8 new_order = 6;
  auto old_ctrl_pts = mesh->get_ctrlPts(1);
  auto old_n_ctrl_pts = mesh->n_internal_ctrlPts(1);
  auto coords = mesh->coords();
  auto nedge = mesh->nedges();
  auto dim = mesh->dim();
  auto ev2v = mesh->get_adj(1, 0).ab2b;

  mesh->change_max_order(new_order);
  auto n_new_pts = mesh->n_internal_ctrlPts(1);
  Write<Real> new_pts(nedge*n_new_pts*dim, 0.0);
  Write<Real> c1(dim, 0.0);
  Write<Real> c2(dim, 0.0);
  Write<Real> c3(dim, 0.0);
  Write<Real> c4(dim, 0.0);
  Write<Real> c5(dim, 0.0);
  auto calc_pts = OMEGA_H_LAMBDA (LO i) {
    auto v0 = ev2v[i*2];
    auto v1 = ev2v[i*2 + 1];
    for (LO d = 0; d < dim; ++d) {
      c1[d] = (1.0/6.0)*coords[v0*dim + d] +
              (5.0/6.0)*old_ctrl_pts[i*old_n_ctrl_pts*dim + d];
      c2[d] = (1.0/3.0)*old_ctrl_pts[i*old_n_ctrl_pts*dim + d] +
              (2.0/3.0)*old_ctrl_pts[i*old_n_ctrl_pts*dim + 1*dim + d];
      c3[d] = (1.0/2.0)*old_ctrl_pts[i*old_n_ctrl_pts*dim + 1*dim + d] +
              (1.0/2.0)*old_ctrl_pts[i*old_n_ctrl_pts*dim + 2*dim + d];
      c4[d] = (2.0/3.0)*old_ctrl_pts[i*old_n_ctrl_pts*dim + 2*dim + d] +
              (1.0/3.0)*old_ctrl_pts[i*old_n_ctrl_pts*dim + 3*dim + d];
      c5[d] = (5.0/6.0)*old_ctrl_pts[i*old_n_ctrl_pts*dim + 3*dim + d] +
              (1.0/6.0)*coords[v1*dim + d];
      new_pts[i*n_new_pts*dim + d] = c1[d];
      new_pts[i*n_new_pts*dim + 1*dim + d] = c2[d];
      new_pts[i*n_new_pts*dim + 2*dim + d] = c3[d];
      new_pts[i*n_new_pts*dim + 3*dim + d] = c4[d];
      new_pts[i*n_new_pts*dim + 4*dim + d] = c5[d];
    }
  };
  parallel_for(nedge, std::move(calc_pts));

  mesh->set_tag_for_ctrlPts(1, Reals(new_pts));
  return;
}

#define OMEGA_H_INST(T)
OMEGA_H_INST(I8)
OMEGA_H_INST(I32)
OMEGA_H_INST(I64)
OMEGA_H_INST(Real)
#undef OMEGA_H_INST

} // namespace Omega_h
