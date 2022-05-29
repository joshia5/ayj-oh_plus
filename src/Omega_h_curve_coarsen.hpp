#ifndef OMEGA_H_CURVE_COARSEN_HPP
#define OMEGA_H_CURVE_COARSEN_HPP

#include "Omega_h_mesh.hpp"
#include "Omega_h_beziers.hpp"
#include "Omega_h_element.hpp"
#include "Omega_h_for.hpp"
#include "Omega_h_map.hpp"
#include "Omega_h_array_ops.hpp"
#include "Omega_h_vector.hpp"
#include "Omega_h_atomics.hpp"

#include <iostream>
#include <fstream>
namespace Omega_h {

template <Int dim>
void coarsen_curved_verts_and_edges(Mesh *mesh, Mesh *new_mesh, LOs old2new,
    LOs prods2new, LOs old_verts2new_verts, LOs keys2verts, LOs keys2verts_onto,
    LOs keys2prods, LOs same_verts2new_verts, LOs same_verts2old_verts) {
  auto const nold_verts = mesh->nverts();
  auto const nold_edges = mesh->nedges();
  auto const nold_faces = mesh->nfaces();
  auto const old_ev2v = mesh->get_adj(1, 0).ab2b;
  auto const old_fe2e = mesh->get_adj(2, 1).ab2b;
  auto const old_ef2f = mesh->ask_up(1, 2).ab2b;
  auto const old_e2ef = mesh->ask_up(1, 2).a2ab;
  auto const old_fv2v = mesh->ask_down(2, 0).ab2b;
  auto const old_v2vf = mesh->ask_up(0,2).a2ab;
  auto const old_vf2f = mesh->ask_up(0,2).ab2b;
  auto const nkeys = keys2verts.size();
  if (!mesh->has_tag(0, "bezier_pts")) {
    mesh->add_tag<Real>(0, "bezier_pts", dim, mesh->coords());
  }
  auto const old_vertCtrlPts = mesh->get_ctrlPts(0);
  auto const old_edgeCtrlPts = mesh->get_ctrlPts(1);
  auto const old_coords = mesh->coords();

  auto const new_ev2v = new_mesh->get_adj(1, 0).ab2b;
  auto const new_coords = new_mesh->coords();
  auto const nnew_edge = new_mesh->nedges();
  auto const n_edge_pts = mesh->n_internal_ctrlPts(1);
  auto const nnew_verts = new_mesh->nverts();

  Write<Real> edge_ctrlPts(nnew_edge*n_edge_pts*dim, INT8_MAX);
  Write<I8> edge_crv2bdry_dim(nnew_edge, -1);
  Write<I8> edge_dualCone(nnew_edge, -1);
  Write<Real> vert_ctrlPts(nnew_verts*1*dim, INT8_MAX);

  //copy ctrl pts for verts
  auto copy_sameCtrlPts = OMEGA_H_LAMBDA(LO i) {
    if (old_verts2new_verts[i] != -1) {
      LO new_vert = old_verts2new_verts[i];
      for (I8 d = 0; d < dim; ++d) {
        vert_ctrlPts[new_vert*dim + d] = old_vertCtrlPts[i*dim + d];
      }
    }
  };
  parallel_for(nold_verts, std::move(copy_sameCtrlPts),
      "copy same vtx ctrlPts");
  new_mesh->add_tag<Real>(0, "bezier_pts", dim);
  new_mesh->set_tag_for_ctrlPts(0, Reals(vert_ctrlPts));

  //copy ctrl pts for edges
  auto copy_sameedgePts = OMEGA_H_LAMBDA(LO i) {
    if (old2new[i] != -1) {
      LO new_edge = old2new[i];
      for (I8 d = 0; d < dim; ++d) {
        edge_ctrlPts[new_edge*n_edge_pts*dim + d] = old_edgeCtrlPts[i*n_edge_pts*dim + d];
        edge_ctrlPts[new_edge*n_edge_pts*dim + dim + d] = old_edgeCtrlPts[i*n_edge_pts*dim + dim + d];
      }
    }
  };
  parallel_for(nold_edges, std::move(copy_sameedgePts),
      "copy_same_edgectrlPts");
  auto prod_edge_points = OMEGA_H_LAMBDA(LO i) {
    LO e = prods2new[i];
    auto const v0 = new_ev2v[e*2 + 0];
    auto const v1 = new_ev2v[e*2 + 1];
    for (LO j=0; j<dim; ++j) {
      edge_ctrlPts[e*n_edge_pts*dim + j] = new_coords[v0*dim + j] +
          (new_coords[v1*dim + j] - new_coords[v0*dim + j])*xi_1_cube();
      edge_ctrlPts[e*n_edge_pts*dim + dim + j] = new_coords[v0*dim + j] +
          (new_coords[v1*dim + j] - new_coords[v0*dim + j])*xi_2_cube();
    }
  };
  parallel_for(prods2new.size(), std::move(prod_edge_points),
      "prod_edge_points");

  auto const newedge_gdim = new_mesh->get_array<I8>(1, "class_dim");
  auto const newedge_gid = new_mesh->get_array<LO>(1, "class_id");
  auto const oldvert_gdim = mesh->get_array<I8>(0, "class_dim");
  auto const oldvert_gid = mesh->get_array<LO>(0, "class_id");
  auto const oldface_gdim = mesh->get_array<I8>(2, "class_dim");
  auto const oldface_gid = mesh->get_array<LO>(2, "class_id");

  auto const v2v_old = mesh->ask_star(0);
  auto const v2vv_old = v2v_old.a2ab;
  auto const vv2v_old = v2v_old.ab2b;
  auto const v2v_new = new_mesh->ask_star(0);
  auto const v2vv_new = v2v_new.a2ab;
  auto const vv2v_new = v2v_new.ab2b;
  auto const old_v2e = mesh->ask_up(0, 1);
  auto const old_v2ve = old_v2e.a2ab;
  auto const old_ve2e = old_v2e.ab2b;
  auto vert_ctrlPts_r = Reals(vert_ctrlPts);

  auto new_verts2same_verts = invert_map_by_atomics(same_verts2new_verts,
						    nnew_verts);
  auto const a2ab = new_verts2same_verts.a2ab;
  auto const ab2b = new_verts2same_verts.ab2b;
  printf("nsame nnew verts %d, %d\n", same_verts2old_verts.size(), nnew_verts);

  auto curve_bdry_edges = OMEGA_H_LAMBDA(LO i) {
    LO const v_key = keys2verts[i];
    LO const v_onto = keys2verts_onto[i];
    for (LO prod = keys2prods[i]; prod < keys2prods[i+1]; ++prod) {
      LO new_edge = prods2new[prod];
      LO const new_edge_v0 = new_ev2v[new_edge*2 + 0];
      LO const new_edge_v1 = new_ev2v[new_edge*2 + 1];
      auto const c0 = get_vector<dim>(vert_ctrlPts_r, new_edge_v0);
      auto const c3 = get_vector<dim>(vert_ctrlPts_r, new_edge_v1);
      Vector<dim> c1;
      Vector<dim> c2;

      if ((oldvert_gdim[v_key] <= 1) && (oldvert_gdim[v_onto] <= 1) &&
          (newedge_gdim[new_edge] == 1)) {
	edge_crv2bdry_dim[new_edge] = 1;
	auto old_p1 = get_vector<dim>(old_vertCtrlPts, v_key);
	Real xi_1 = 0.5;
	Real sum_dist1 = 0.0;
	Real sum_dist2 = 0.0;
	for (Int j = 0; j < dim; ++j) {
	  sum_dist1 += (old_p1[j] - c0[j])*(old_p1[j] - c0[j]);
	  sum_dist2 += (old_p1[j] - c3[j])*(old_p1[j] - c3[j]);
	}
	sum_dist1 = std::sqrt(sum_dist1/(dim*1.0));
	sum_dist2 = std::sqrt(sum_dist2/(dim*1.0));
	xi_1 = sum_dist1/(sum_dist1 + sum_dist2);
	Vector<dim> old_c1;
	for (Int j = 0; j < dim; ++j) {
	  old_c1[j] = (old_p1[j] - B0_quad(xi_1)*c0[j] - 
                       B2_quad(xi_1)*c3[j])/B1_quad(xi_1);
	}
	for (LO d = 0; d < dim; ++d) {
	  c1[d] = (1.0/3.0)*c0[d] + (2.0/3.0)*old_c1[d];
	  c2[d] = (2.0/3.0)*old_c1[d] + (1.0/3.0)*c3[d];
	}
      }

      if ((dim == 3) && (newedge_gdim[new_edge] == 2)) {
        LO const e_g_face = newedge_gid[new_edge]; 
        LO const new_edge_v0_old = same_verts2old_verts[ab2b[a2ab[new_edge_v0]]];
        LO const new_edge_v1_old = same_verts2old_verts[ab2b[a2ab[new_edge_v1]]];
        LO c1_face, c2_face = -1;
	auto c0_old = get_vector<dim>(old_vertCtrlPts, new_edge_v0_old);
	auto c3_old = get_vector<dim>(old_vertCtrlPts, new_edge_v1_old);
        printf("new v0 coords {%f,%f,%f}\n", c0[0], c0[1], c0[2]);
        printf("old v0 coords {%f,%f,%f}\n", c0_old[0], c0_old[1], c0_old[2]);
        printf("new v1 coords {%f,%f,%f}\n", c3[0], c3[1], c3[2]);
        printf("old v1 coords {%f,%f,%f}\n", c3_old[0], c3_old[1], c3_old[2]);

        LO count_faces = 0;
        for (LO vf = old_v2vf[v_key]; vf < old_v2vf[v_key + 1]; ++vf) {
          LO const f = old_vf2f[vf];
          if ((oldface_gdim[f] == 2) && (oldface_gid[f] == e_g_face)) {
            //this face is class. on same model face as collapsing edge
            //now one of the verts is v0 then c1 face or v1 then c2 face
            for (LO k = 0; k < 2; ++k) {
              if (old_fv2v[f*3 + k] == new_edge_v0_old) c1_face = f;
              if (old_fv2v[f*3 + k] == new_edge_v1_old) c2_face = f;
            }
          }
        printf("%d %d \n", c1_face, c2_face);
        }
      }
      for (LO d = 0; d < dim; ++d) {
        edge_ctrlPts[new_edge*n_edge_pts*dim + d] = c1[d];
        edge_ctrlPts[new_edge*n_edge_pts*dim + dim + d] = c2[d];
      }
    }
  };
  parallel_for(keys2verts.size(), std::move(curve_bdry_edges));

  Write<LO> count_dualCone_cavities(1, 0);
  Write<LO> count_interior_dualCone_cavities(1, 0);
  Write<LO> count_interior_cavities(1, 0);
  Write<I8> face_dualCone(nold_faces, -1);
  auto count_dualCone_cav = OMEGA_H_LAMBDA(LO i) {
    LO const v_onto = keys2verts_onto[i];
    LO const v_key = keys2verts[i];
    for (LO vf = old_v2vf[v_key]; vf < old_v2vf[v_key + 1]; ++vf) {
      LO const f = old_vf2f[vf];
      face_dualCone[f] = 1;
    }
    if ((oldvert_gdim[v_key] == dim) && (oldvert_gdim[v_onto] == dim)) {
      atomic_increment(&count_interior_cavities[0]);
    }
    if ((keys2prods[i+1] - keys2prods[i]) == 1) {
      atomic_increment(&count_dualCone_cavities[0]);
      if ((oldvert_gdim[v_key] == dim) && (oldvert_gdim[v_onto] == dim)) {
        atomic_increment(&count_interior_dualCone_cavities[0]);
      }
    }
  };
  parallel_for(keys2prods.size()-1, std::move(count_dualCone_cav));
  printf("total nkeys %d, nkeys in interior %d nkeys with dual cone cavities %d, %d in interior\n", keys2prods.size()-1, 
      count_interior_cavities[0], count_dualCone_cavities[0], count_interior_dualCone_cavities[0]);
  mesh->add_tag<I8>(2, "face_dualCone", 1, Read<I8>(face_dualCone));

  if (dim == 3) {
    auto v2t = mesh->ask_up(0, 3);
    auto v2vt = v2t.a2ab;
    auto vt2t = v2t.ab2b;
    auto v2e = mesh->ask_up(0, 1);
    auto v2ve = v2e.a2ab;
    auto ve2e = v2e.ab2b;
    auto te2e = mesh->ask_down(3, 1).ab2b;
    auto ev2v = mesh->get_adj(1, 0).ab2b;
 
    // for every edge, calc and store 2 tangents from either vertex
    Write<Real> tangents(nold_edges*2*dim, 0);
    auto calc_tangents = OMEGA_H_LAMBDA (LO e) {
      LO e_v0 = ev2v[e*2 + 0];
      LO e_v1 = ev2v[e*2 + 1];
      Real length1 = 0.0;
      Real length2 = 0.0;
      for (LO d = 0; d < dim; ++d) {
        tangents[e*2*dim + d] = old_edgeCtrlPts[e*2*dim + d] - old_vertCtrlPts[e_v0*dim + d];
        tangents[e*2*dim + dim + d] = old_edgeCtrlPts[e*2*dim + dim + d] - old_vertCtrlPts[e_v1*dim + d];
        length1 += tangents[e*2*dim + d] * tangents[e*2*dim + d];
        length2 += tangents[e*2*dim + dim + d] * tangents[e*2*dim + dim + d];
      }
      for (LO d = 0; d < 3; ++d) {
        tangents[e*2*dim + d] = tangents[e*2*dim + d]/std::sqrt(length1);
        tangents[e*2*dim + dim + d] = tangents[e*2*dim + dim + d]/std::sqrt(length2);
      }
    };
    parallel_for(nold_edges, std::move(calc_tangents));

    auto curve_dualCone_cav = OMEGA_H_LAMBDA (LO i) {
      if ((keys2prods[i+1] - keys2prods[i]) == 1) {
        LO const v_onto = keys2verts_onto[i];
        LO const v_key = keys2verts[i];
        if ((oldvert_gdim[v_key] == dim) && (oldvert_gdim[v_onto] == dim)) {
          LO const new_edge = prods2new[keys2prods[i]];
          edge_dualCone[new_edge] = 1;
          auto new_edge_v0 = new_ev2v[new_edge*2 + 0];
          auto new_edge_v1 = new_ev2v[new_edge*2 + 1];
          auto new_edge_v0_c = get_vector<dim>(new_coords, new_edge_v0);
          auto new_edge_v1_c = get_vector<dim>(new_coords, new_edge_v1);

          //count upper edges
          Few<LO, 128> upper_edges;
          Few<LO, 128> vtx_ring;
          Few<I8, 128> from_first_vtx;
          LO count_upper_edge = 0;
          for (LO vt = v2vt[v_key]; vt < v2vt[v_key + 1]; ++vt) {
            //adj tets of vkey
            LO adj_t = vt2t[vt];
            for (LO te = 0; te < 6; ++te) {
              LO adj_t_e = te2e[adj_t*6 + te];
              //adj edges of tet
              LO adj_t_e_v0 = ev2v[adj_t_e*2 + 0];
              LO adj_t_e_v1 = ev2v[adj_t_e*2 + 1];
              //adj verts of edge
              if ((adj_t_e_v0 == v_onto) && (adj_t_e_v1 != v_key)) {
                OMEGA_H_CHECK(count_upper_edge < 128);
                LO is_duplicate = -1;
                for (LO upper_e = 0; upper_e < count_upper_edge; ++upper_e) {
                  if (adj_t_e == upper_edges[upper_e]) is_duplicate = 1;
                }
                if (is_duplicate == -1) {
                  upper_edges[count_upper_edge] = adj_t_e;
                  from_first_vtx[count_upper_edge] = 1;
                  vtx_ring[count_upper_edge] = adj_t_e_v1;
                  ++count_upper_edge;
                }
              }
              if ((adj_t_e_v1 == v_onto) && (adj_t_e_v0 != v_key)) {
                LO is_duplicate = -1;
                for (LO upper_e = 0; upper_e < count_upper_edge; ++upper_e) {
                  if (adj_t_e == upper_edges[upper_e]) is_duplicate = 1;
                }
                if (is_duplicate == -1) {
                  upper_edges[count_upper_edge] = adj_t_e;
                  from_first_vtx[count_upper_edge] = -1;
                  vtx_ring[count_upper_edge] = adj_t_e_v0;
                  ++count_upper_edge;
                }
              }
            }
          }

          Few<Real, dim> t_avg;
          for (LO d = 0; d < dim; ++d) t_avg[d] = 0.0; 
          for (LO upper_e = 0; upper_e < count_upper_edge; ++upper_e) {
            if (from_first_vtx[upper_e] == 1) {
              for (LO d = 0; d < dim; ++d) {
                t_avg[d] += tangents[upper_edges[upper_e]*2*dim + d];
              }
            }
            if (from_first_vtx[upper_e] == -1) {
              for (LO d = 0; d < dim; ++d) {
                t_avg[d] += tangents[upper_edges[upper_e]*2*dim + dim + d];
              }
            }
          }
          for (LO d = 0; d < dim; ++d) t_avg[d] = t_avg[d]/count_upper_edge;
          Real length_t = 0.0;
          for (LO d = 0; d < dim; ++d) length_t += t_avg[d]*t_avg[d]; 
          for (LO d = 0; d < dim; ++d) t_avg[d] = t_avg[d]/std::sqrt(length_t);

          //find lower vtx
          LO v_lower = -1;
          for (LO ve = v2ve[v_key]; ve < v2ve[v_key + 1]; ++ve) {
            //adj edges of vkey
            LO adj_e = ve2e[ve];
            LO adj_e_v0 = ev2v[adj_e*2 + 0];
            LO adj_e_v1 = ev2v[adj_e*2 + 1];
            LO other_vtx = -1;
            LO count_not_ring = 0;
            //note id of other end of edge
            if (v_key == adj_e_v0) {
              other_vtx = adj_e_v1;
            }
            if (v_key == adj_e_v1) {
              other_vtx = adj_e_v0;
            }
            if (other_vtx != v_onto) {//eliminate v_onto
              for (LO upper_e = 0; upper_e < count_upper_edge; ++upper_e) {
                if (other_vtx != vtx_ring[upper_e]) ++count_not_ring;
              }
              if (count_not_ring == count_upper_edge) {
                v_lower = other_vtx;
                break;
              }
            }
          }

          Few<LO, 128> lower_edges;
          Few<I8, 128> from_first_vtx_low;
          LO count_lower_edge = 0;
          for (LO vt = v2vt[v_key]; vt < v2vt[v_key + 1]; ++vt) {
            //adj tets of vkey
            LO adj_t = vt2t[vt];
            for (LO te = 0; te < 6; ++te) {
              LO adj_t_e = te2e[adj_t*6 + te];
              //adj edges of tet
              LO adj_t_e_v0 = ev2v[adj_t_e*2 + 0];
              LO adj_t_e_v1 = ev2v[adj_t_e*2 + 1];
              //adj verts of edge
              if ((adj_t_e_v0 == v_lower) && (adj_t_e_v1 != v_key)) {
                OMEGA_H_CHECK(count_lower_edge < 128);
                LO is_duplicate = -1;
                for (LO lower_e = 0; lower_e < count_lower_edge; ++lower_e) {
                  if (adj_t_e == lower_edges[lower_e]) is_duplicate = 1;
                }
                if (is_duplicate == -1) {
                  lower_edges[count_lower_edge] = adj_t_e;
                  from_first_vtx_low[count_lower_edge] = 1;
                  ++count_lower_edge;
                }
              }
              if ((adj_t_e_v1 == v_lower) && (adj_t_e_v0 != v_key)) {
                LO is_duplicate = -1;
                for (LO lower_e = 0; lower_e < count_lower_edge; ++lower_e) {
                  if (adj_t_e == lower_edges[lower_e]) is_duplicate = 1;
                }
                if (is_duplicate == -1) {
                  lower_edges[count_lower_edge] = adj_t_e;
                  from_first_vtx_low[count_lower_edge] = -1;
                  ++count_lower_edge;
                }
              }
            }
          }
          printf("upper edge %d lower %d\n", count_upper_edge ,count_lower_edge);

          Few<Real, dim> t_avg_l;
          for (LO d = 0; d < dim; ++d) t_avg_l[d] = 0.0; 
          for (LO lower_e = 0; lower_e < count_lower_edge; ++lower_e) {
            if (from_first_vtx_low[lower_e] == 1) {
              for (LO d = 0; d < dim; ++d) {
                t_avg_l[d] += tangents[lower_edges[lower_e]*2*dim + d];
              }
            }
            if (from_first_vtx_low[lower_e] == -1) {
              for (LO d = 0; d < dim; ++d) {
                t_avg_l[d] += tangents[lower_edges[lower_e]*2*dim + dim + d];
              }
            }
          }
          for (LO d = 0; d < dim; ++d) t_avg_l[d] = t_avg_l[d]/count_lower_edge;
          Real length_t_l = 0.0;
          for (LO d = 0; d < dim; ++d) length_t_l += t_avg_l[d]*t_avg_l[d]; 
          for (LO d = 0; d < dim; ++d) t_avg_l[d] = t_avg_l[d]/std::sqrt(length_t_l);
          Vector<dim> c_lower;
          Vector<dim> c_upper;
          auto new_length = 
            (new_edge_v1_c[0] - new_edge_v0_c[0])*(new_edge_v1_c[0] - new_edge_v0_c[0]) + 
            (new_edge_v1_c[1] - new_edge_v0_c[1])*(new_edge_v1_c[1] - new_edge_v0_c[1]) + 
            (new_edge_v1_c[2] - new_edge_v0_c[2])*(new_edge_v1_c[2] - new_edge_v0_c[2]);
          new_length = std::sqrt(new_length);
          for (LO d = 0; d < dim; ++d) {
            c_upper[d] = old_coords[v_onto*dim + d] + t_avg[d]*new_length/3.0;
            c_lower[d] = old_coords[v_lower*dim + d] + t_avg_l[d]*new_length/3.0;
          }

          //check for new edge, first vertex is vlower or vupper
          LO v_onto_is_first = -1;
          if ((std::abs(new_edge_v0_c[0] - old_coords[v_onto*dim + 0]) < EPSILON) && 
              (std::abs(new_edge_v0_c[1] - old_coords[v_onto*dim + 1]) < EPSILON) &&
              (std::abs(new_edge_v0_c[2] - old_coords[v_onto*dim + 2]) < EPSILON)) {
            v_onto_is_first = 1;
          }
          else {
            OMEGA_H_CHECK (
              (std::abs(new_edge_v0_c[0] - old_coords[v_lower*dim + 0]) < EPSILON) && 
              (std::abs(new_edge_v0_c[1] - old_coords[v_lower*dim + 1]) < EPSILON) &&
              (std::abs(new_edge_v0_c[2] - old_coords[v_lower*dim + 2]) < EPSILON));
          }
          printf("edge %d voif %d\n", new_edge, v_onto_is_first);
          
          for (LO d = 0; d < dim; ++d) {
            if (v_onto_is_first == 1) {
              edge_ctrlPts[new_edge*n_edge_pts*dim + d] = c_upper[d];
              edge_ctrlPts[new_edge*n_edge_pts*dim + dim + d] = c_lower[d];
            }
            else {
              OMEGA_H_CHECK (v_onto_is_first == -1);
              edge_ctrlPts[new_edge*n_edge_pts*dim + d] = c_lower[d];
              edge_ctrlPts[new_edge*n_edge_pts*dim + dim + d] = c_upper[d];
            }
          }
        }
      }
    };
    parallel_for(nkeys, std::move(curve_dualCone_cav), "curve_dualCone_cav");
  }

  new_mesh->add_tag<Real>(1, "bezier_pts", n_edge_pts*dim, Reals(edge_ctrlPts));
  new_mesh->add_tag<I8>(1, "edge_crv2bdry_dim", 1, Read<I8>(edge_crv2bdry_dim));
  new_mesh->add_tag<I8>(1, "edge_dualCone", 1, Read<I8>(edge_dualCone));

  return;
}

static Few<Real, 10> const BlendedTriangleGetValues(
    Vector<3> const xi, LO b) {
  Few<Real, 10> values;
  Real const blendingTol = 1.e-12;
  double xii[3] = {1.-xi[0]-xi[1],xi[0],xi[1]};

  for (LO i = 0; i < 3; ++i)
    values[i] = -std::pow(xii[i], b);
  LO const n = 10;//10 nodes per tri
  for (LO i = 3; i < n; ++i)
    values[i] = 0.0;


  const LO nE = 2;//2 nodes per edge
  Few<LO, 6> tev;
  tev[0] = 0;
  tev[1] = 1;
  tev[2] = 1;
  tev[3] = 2;
  tev[4] = 2;
  tev[5] = 0;

  for (LO i = 0; i < 3; ++i) {
    Real x, xiix, xv;
    Few<Real, 4> v;
    x = xii[tev[i*2 + 0]] + xii[tev[i*2 + 1]];

    if (x < blendingTol)
      xiix = 0.5;
    else
      xiix = xii[tev[i*2 +1]]/x;

    xv = xiix;
    v[0] = Bi(3, 0, xv);
    v[1] = Bi(3, 3, xv);
    v[2] = Bi(3, 1, xv);
    v[3] = Bi(3, 2, xv);

    for (LO j = 0; j < 2; ++j)
      values[tev[i*2 + j]] += v[j]*std::pow(x, b);
    for (LO j = 0; j < nE; ++j)
      values[3+i*nE+j] = v[2+j]*std::pow(x, b);
  }
  return values;
}

template <Int dim>
void coarsen_curved_faces(Mesh *mesh, Mesh *new_mesh, LOs old2new,
    LOs prods2new) {
  auto const new_fv2v = new_mesh->ask_down(2, 0).ab2b;
  auto const new_fe2e = new_mesh->get_adj(2, 1).ab2b;
  auto const new_ev2v = new_mesh->get_adj(1, 0).ab2b;
  auto const new_coords = new_mesh->coords();
  auto const nnew_faces = new_mesh->nfaces();
  auto const nnew_edges = new_mesh->nedges();
  auto const nold_faces = mesh->nfaces();
  auto const old_faceCtrlPts = mesh->get_ctrlPts(2);
  auto const vertCtrlPts = new_mesh->get_ctrlPts(0);
  auto const edgeCtrlPts = new_mesh->get_ctrlPts(1);
  Write<Real> face_ctrlPts(nnew_faces*1*dim, INT8_MAX);

  //copy ctrl pts for faces
  auto copy_samefacePts = OMEGA_H_LAMBDA(LO i) {
    if (old2new[i] != -1) {
      LO new_face = old2new[i];
      for (I8 d = 0; d < dim; ++d) {
        face_ctrlPts[new_face*dim + d] = old_faceCtrlPts[i*dim + d];
      }
    }
  };
  parallel_for(nold_faces, std::move(copy_samefacePts),
      "copy_same_facectrlPts");

  auto face_centroids = OMEGA_H_LAMBDA(LO i) {
    LO const tri = prods2new[i];
    LO const v0 = new_fv2v[tri*3 + 0];
    LO const v1 = new_fv2v[tri*3 + 1];
    LO const v2 = new_fv2v[tri*3 + 2];
    for (LO j = 0; j < dim; ++j) {
      face_ctrlPts[tri*dim + j] = (new_coords[v0*dim + j] +
         new_coords[v1*dim + j] + new_coords[v2*dim + j])/3.0;
    }
  };
  parallel_for(prods2new.size(), std::move(face_centroids), "face_centroids");

  //loop over new edges
  //if new edge dual cone
  //  find adjacent faces in a for loop
  //  pass face ids as input to blended tri fn 
  //  then take first 9 values and mult them with all ctrl pts of face
  //  that gives the interp pt
  //  face interp to ctrl pt
  if (dim == 3) {
    Vector<3> face_xi;
    face_xi[0] = 1.0/3.0;
    face_xi[1] = 1.0/3.0;
    face_xi[2] = 1.0/3.0;
    auto const weights = BlendedTriangleGetValues(face_xi, 1);
    auto edge_dualCone = new_mesh->get_array<I8>(1, "edge_dualCone");
    auto const e2et = new_mesh->ask_up(1, 2).a2ab;
    auto const et2t = new_mesh->ask_up(1, 2).ab2b;

    Write<I8> face_dualCone(nnew_faces, -1);
    auto face_blends = OMEGA_H_LAMBDA(LO e) {
      if (edge_dualCone[e] == 1) {
        for (LO et = e2et[e]; et < e2et[e + 1]; ++et) {
          LO const tri = et2t[et];
          face_dualCone[tri] = 1;

          auto p11 = face_blend_interp_3d(3, tri, new_ev2v, new_fe2e,
              vertCtrlPts, edgeCtrlPts, new_fv2v, weights);
          auto newface_c11 = face_interpToCtrlPt_3d(3, tri, new_ev2v, new_fe2e,
              vertCtrlPts, edgeCtrlPts, p11, new_fv2v);
          for (LO k = 0; k < 3; ++k) {
            face_ctrlPts[tri*dim + k] = newface_c11[k];
          }

        }
      }
    };
    parallel_for(nnew_edges, std::move(face_blends), "face_blends");
    new_mesh->add_tag<I8>(2, "face_dualCone", 1, Read<I8>(face_dualCone));
  }

  new_mesh->add_tag<Real>(2, "bezier_pts", dim);
  new_mesh->set_tag_for_ctrlPts(2, Reals(face_ctrlPts));

  return;
}

void correct_curved_edges(Mesh *new_mesh);
void check_validity_new_curved_edges(Mesh *new_mesh);

#define OMEGA_H_INST(T)                                                       
OMEGA_H_INST(I8)
OMEGA_H_INST(I32)
OMEGA_H_INST(I64)
OMEGA_H_INST(Real)
#undef OMEGA_H_INST

} // namespace Omega_h

#endif