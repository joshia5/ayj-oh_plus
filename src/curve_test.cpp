#include <iostream>
#include <fstream>
#include <math.h>

#include<Omega_h_mesh.hpp>
#include<Omega_h_file.hpp>
#include<Omega_h_beziers.hpp>
#include<Omega_h_matrix.hpp>
#include<Omega_h_defines.hpp>

using namespace Omega_h;

void test_2d(Library *lib, const std::string &mesh_file, const char* vtk_file) {
  auto mesh = Mesh(lib);
  binary::read (mesh_file, lib->world(), &mesh);
  mesh.set_curved(1);
  mesh.set_max_order(3);
  mesh.add_tags_for_ctrlPts();
  auto coords = mesh.coords();
  auto coords_h = HostRead<Real>(coords);
  auto edge_nCtrlPts = mesh.n_internal_ctrlPts(1);
  auto dim = mesh.dim();
  HostWrite<Real> edge_ctrlPts(mesh.nedges()*edge_nCtrlPts*dim);
  auto ev2v = mesh.ask_down(1, 0).ab2b;
  
  Real centerx = 0.5;
  Real centery = 0.5;
  Real R = 0.70711;
  Real xi_1 = 0.2748;
  Real xi_2 = 0.7252;

  std::ofstream edge_file;
  edge_file.open("edges.csv");

  // edge 1(1) (diameter, starts at pi/4, on for pi)
  {
    Real s_c1 = PI * xi_1;
    Real s_c2 = PI * xi_2;
    Matrix<2,1> p0({coords_h[4], coords_h[5]});
    Matrix<2,1> p1({coords_h[2], coords_h[3]});
    Real x1 = centerx - R*cos(PI/4 + s_c1);
    Real y1 = centery + R*sin(PI/4 + s_c1);
    Real x2 = centerx - R*cos(PI/4 + s_c2);
    Real y2 = centery + R*sin(PI/4 + s_c2);

    Matrix<2,1> fx({x1, x2});
    Matrix<2,1> fy({y1, y2});

    Matrix<2,2> M1_inv ({B1(xi_1), B2(xi_1), B1(xi_2), B2(xi_2)});
    auto M1 = invert(M1_inv);
    Matrix<2,2> M2 ({B0(xi_1), B3(xi_1), B0(xi_2), B3(xi_2)});

    auto Cx = M1*fx - M1*M2*p0;
    auto Cy = M1*fy - M1*M2*p1;

    edge_ctrlPts[1*edge_nCtrlPts*dim + 0*dim + 0] = Cx(0,0);
    edge_ctrlPts[1*edge_nCtrlPts*dim + 0*dim + 1] = Cy(0,0);
    edge_ctrlPts[1*edge_nCtrlPts*dim + 1*dim + 0] = Cx(1,0);
    edge_ctrlPts[1*edge_nCtrlPts*dim + 1*dim + 1] = Cy(1,0);

    auto u = Read<Real>(1000, 0.0, 0.001, "samplePts");
    auto u_h = HostRead<Real>(u);
    auto u_ex = Read<Real>(1000, PI/4, 2*PI/1000, "exactPts");
    auto u_ex_h = HostRead<Real>(u_ex);

    edge_file << "x, y\n";
    for (LO i = 0; i < u.size(); ++i) {
      auto x_bezier = p0(0,0)*B0(u_h[i]) + Cx(0,0)*B1(u_h[i]) + Cx(1,0)*B2(u_h[i]) +
                      p1(0,0)*B3(u_h[i]);
      auto y_bezier = p0(1,0)*B0(u_h[i]) + Cy(0,0)*B1(u_h[i]) + Cy(1,0)*B2(u_h[i]) +
                      p1(1,0)*B3(u_h[i]);
      edge_file << x_bezier << ", " << y_bezier << "\n";
    }
    edge_file << "\n";
    edge_file << "\n";
    edge_file << "C1x, C1y\n";
    edge_file << Cx(0,0) << ", " << Cy(0,0) << "\n";
    edge_file << "C2x, C2y\n";
    edge_file << Cx(1,0) << ", " << Cy(1,0) << "\n";
    edge_file << "x1, y1\n";
    edge_file << x1 << ", " << y1 << "\n";
    edge_file << "x2, y2\n";
    edge_file << x2 << ", " << y2 << "\n";
    edge_file << "\n";
    edge_file << "\n";
    edge_file << "exact circle:\n";
    edge_file << "x, y\n";
    for (LO i = 0; i < u_ex.size(); ++i) {
      auto x_circle = centerx - R*cos(u_ex_h[i]);
      auto y_circle = centery + R*sin(u_ex_h[i]);
      edge_file << x_circle << ", " << y_circle << "\n";
    }
  }
  edge_file << "\n";

  //

  // edge 2(0) (along x, starts at 5*pi/4 goes on for pi/2)
  {
    Real s_c1 = PI/2 * xi_1;
    Real s_c2 = PI/2 * xi_2;
    Matrix<2,1> p0({coords_h[2], coords_h[3]});
    Matrix<2,1> p1({coords_h[0], coords_h[1]});
    Real x1 = centerx - R*cos(5*PI/4 + s_c1);
    Real y1 = centery + R*sin(5*PI/4 + s_c1);
    Real x2 = centerx - R*cos(5*PI/4 + s_c2);
    Real y2 = centery + R*sin(5*PI/4 + s_c2);

    Matrix<2,1> fx({x1, x2});
    Matrix<2,1> fy({y1, y2});

    Matrix<2,2> M1_inv ({B1(xi_1), B2(xi_1), B1(xi_2), B2(xi_2)});
    auto M1 = invert(M1_inv);
    Matrix<2,2> M2 ({B0(xi_1), B3(xi_1), B0(xi_2), B3(xi_2)});

    auto Cx = M1*fx - M1*M2*p0;
    auto Cy = M1*fy - M1*M2*p1;

    edge_ctrlPts[0*edge_nCtrlPts*dim + 0*dim + 0] = Cx(0,0);
    edge_ctrlPts[0*edge_nCtrlPts*dim + 0*dim + 1] = Cy(0,0);
    edge_ctrlPts[0*edge_nCtrlPts*dim + 1*dim + 0] = Cx(1,0);
    edge_ctrlPts[0*edge_nCtrlPts*dim + 1*dim + 1] = Cy(1,0);

    auto u = Read<Real>(1000, 0.0, 0.001, "samplePts");
    auto u_h = HostRead<Real>(u);

    edge_file << "for edge 2\n";
    edge_file << "x, y\n";
    for (LO i = 0; i < u.size(); ++i) {
      auto x_bezier = p0(0,0)*B0(u_h[i]) + Cx(0,0)*B1(u_h[i]) + Cx(1,0)*B2(u_h[i]) +
                      p1(0,0)*B3(u_h[i]);
      auto y_bezier = p0(1,0)*B0(u_h[i]) + Cy(0,0)*B1(u_h[i]) + Cy(1,0)*B2(u_h[i]) +
                      p1(1,0)*B3(u_h[i]);
      edge_file << x_bezier << ", " << y_bezier << "\n";
    }
    edge_file << "\n";
    edge_file << "\n";
    edge_file << "C1x, C1y\n";
    edge_file << Cx(0,0) << ", " << Cy(0,0) << "\n";
    edge_file << "C2x, C2y\n";
    edge_file << Cx(1,0) << ", " << Cy(1,0) << "\n";
    edge_file << "x1, y1\n";
    edge_file << x1 << ", " << y1 << "\n";
    edge_file << "x2, y2\n";
    edge_file << x2 << ", " << y2 << "\n";
  }
  edge_file << "\n";

  // edge 3(2) (along y, starts at 7*pi/4 goes on for pi/2)
  {
    Real s_c1 = PI/2 * xi_1;
    Real s_c2 = PI/2 * xi_2;
    Matrix<2,1> p0({coords_h[0], coords_h[1]});
    Matrix<2,1> p1({coords_h[4], coords_h[5]});
    Real x1 = centerx - R*cos(7*PI/4 + s_c1);
    Real y1 = centery + R*sin(7*PI/4 + s_c1);
    Real x2 = centerx - R*cos(7*PI/4 + s_c2);
    Real y2 = centery + R*sin(7*PI/4 + s_c2);

    Matrix<2,1> fx({x1, x2});
    Matrix<2,1> fy({y1, y2});

    Matrix<2,2> M1_inv ({B1(xi_1), B2(xi_1), B1(xi_2), B2(xi_2)});
    auto M1 = invert(M1_inv);
    Matrix<2,2> M2 ({B0(xi_1), B3(xi_1), B0(xi_2), B3(xi_2)});

    auto Cx = M1*fx - M1*M2*p0;
    auto Cy = M1*fy - M1*M2*p1;

    edge_ctrlPts[2*edge_nCtrlPts*dim + 0*dim + 0] = Cx(0,0);
    edge_ctrlPts[2*edge_nCtrlPts*dim + 0*dim + 1] = Cy(0,0);
    edge_ctrlPts[2*edge_nCtrlPts*dim + 1*dim + 0] = Cx(1,0);
    edge_ctrlPts[2*edge_nCtrlPts*dim + 1*dim + 1] = Cy(1,0);

    auto u = Read<Real>(1000, 0.0, 0.001, "samplePts");
    auto u_h = HostRead<Real>(u);

    edge_file << "for edge 3\n";
    edge_file << "x, y\n";
    for (LO i = 0; i < u.size(); ++i) {
      auto x_bezier = p0(0,0)*B0(u_h[i]) + Cx(0,0)*B1(u_h[i]) + Cx(1,0)*B2(u_h[i]) +
                      p1(0,0)*B3(u_h[i]);
      auto y_bezier = p0(1,0)*B0(u_h[i]) + Cy(0,0)*B1(u_h[i]) + Cy(1,0)*B2(u_h[i]) +
                      p1(1,0)*B3(u_h[i]);
      edge_file << x_bezier << ", " << y_bezier << "\n";
    }
    edge_file << "\n";
    edge_file << "\n";
    edge_file << "C1x, C1y\n";
    edge_file << Cx(0,0) << ", " << Cy(0,0) << "\n";
    edge_file << "C2x, C2y\n";
    edge_file << Cx(1,0) << ", " << Cy(1,0) << "\n";
    edge_file << "x1, y1\n";
    edge_file << x1 << ", " << y1 << "\n";
    edge_file << "x2, y2\n";
    edge_file << x2 << ", " << y2 << "\n";
  }
  edge_file << "\n";

  edge_file.close();

  mesh.set_tag_for_ctrlPts(1, Reals(edge_ctrlPts.write()));

  vtk::write_parallel(vtk_file, &mesh, mesh.dim());
  return;
}

int main(int argc, char** argv) {

  auto lib = Library (&argc, &argv);

  if (argc != 3) {
    Omega_h_fail("a.out <2d_in_osh> <2d_out_vtk>\n");
  };
  char const* path_2d = nullptr;
  char const* path_2d_vtk = nullptr;
  path_2d = argv[1];
  path_2d_vtk = argv[2];

  test_2d(&lib, path_2d, path_2d_vtk);

  return 0;
}