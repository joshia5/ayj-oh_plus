#include "Omega_h_beziers.hpp"
#include "Omega_h_vector.hpp"
#include "Omega_h_scalar.hpp"

int binomial(int n, int i) {

  i = std::min(n-i,i);

  if(i == 0)
    return 1;
  if(i == 1)
    return n;

  static int const bn4[1] = {6};
  static int const bn5[1] = {10};
  static int const bn6[2] = {15,20};
  static int const bn7[2] = {21,35};
  static int const bn8[3] = {28,56,70};
  static int const bn9[3] = {36,84,126};
  static int const bn10[4] = {45,120,210,252};
  static int const bn11[4] = {55,165,330,462};
  static int const bn12[5] = {66,220,495,792,924};
  static int const bn13[5] = {78,286,715,1287,1716};
  static int const bn14[6] = {91,364,1001,2002,3003,3432};
  static int const bn15[6] = {105,455,1365,3003,5005,6435};
  static int const bn16[7] = {120,560,1820,4368,8008,11440,12870};
  static int const bn17[7] = {136,680,2380,6188,12376,19448,24310};
  static int const bn18[8] = {153,816,3060,8568,18564,31824,43758,48620};
  static int const bn19[8] = {171,969,3876,11628,27132,50388,75582,92378};
  static int const bn20[9] = {190,1140,4845,15504,38760,77520,125970,167960,
      184756};
  static int const bn21[9] = {210,1330,5985,20349,54264,116280,203490,293930,
      352716};
  static int const bn22[10] = {231,1540,7315,26334,74613,170544,319770,497420,
      646646,705432};
  static int const bn23[10] = {253,1771,8855,33649,100947,245157,490314,817190,
      1144066,1352078};
  static int const bn24[11] = {276,2024,10626,42504,134596,346104,735471,1307504,
      1961256,2496144,2704156};
  static int const bn25[11] = {300,2300,12650,53130,177100,480700,1081575,2042975,
      3268760,4457400,5200300};
  static int const bn26[12] = {325,2600,14950,65780,230230,657800,1562275,3124550,
      5311735,7726160,9657700,10400600};
  static int const bn27[12] = {351,2925,17550,80730,296010,888030,2220075,4686825,
      8436285,13037895,17383860,20058300};
  static int const bn28[13] = {378,3276,20475,98280,376740,1184040,3108105,6906900,
      13123110,21474180,30421755,37442160,40116600};
  static int const bn29[13] = {406,3654,23751,118755,475020,1560780,4292145,10015005,
      20030010,34597290,51895935,67863915,77558760};
  static int const bn30[14] = {435,4060,27405,142506,593775,2035800,5852925,14307150,
      30045015,54627300,86493225,119759850,145422675,155117520};
  static int const bn31[14] = {465,4495,31465,169911,736281,2629575,7888725,20160075,
      44352165,84672315,141120525,206253075,265182525,300540195};
  static int const bn32[15] = {496,4960,35960,201376,906192,3365856,10518300,28048800,
      64512240,129024480,225792840,347373600,471435600,565722720,601080390};
  static int const bn33[15] = {528,5456,40920,237336,1107568,4272048,13884156,38567100,
      92561040,193536720,354817320,573166440,818809200,1037158320,1166803110};


  static int const* const bnTable[34] = {0,0,0,0,bn4,bn5,bn6,bn7,bn8,
      bn9,bn10,bn11,bn12,bn13,bn14,bn15,bn16,bn17,bn18,bn19,bn20,bn21,bn22,bn23,
      bn24,bn25,bn26,bn27,bn28,bn29,bn30,bn31,bn32,bn33};

  return bnTable[n][i-2];

}

int trinomial(int n, int i, int j) {
  return binomial(n,i)*binomial(n-i,j);
}

static unsigned const b2_0_0[1] = {2};
static unsigned const* const b2_0[1] = {b2_0_0};
static unsigned const b2_1_0[2] = {2,1};
static unsigned const b2_1_1[1] = {0};
static unsigned const* const b2_1[2] = {b2_1_0,b2_1_1};
static unsigned const b2_2_0[3] = {2,4,1};
static unsigned const b2_2_1[2] = {5,3};
static unsigned const b2_2_2[1] = {0};
static unsigned const* const b2_2[3] = {b2_2_0,b2_2_1,b2_2_2};
static unsigned const b2_3_0[4] = {2,6,5,1};
static unsigned const b2_3_1[3] = {7,9,4};
static unsigned const b2_3_2[2] = {8,3};
static unsigned const b2_3_3[1] = {0};
static unsigned const* const b2_3[4] = {b2_3_0,b2_3_1,b2_3_2,b2_3_3};
static unsigned const b2_4_0[5] = {2,8,7,6,1};
static unsigned const b2_4_1[4] = {9,14,13,5};
static unsigned const b2_4_2[3] = {10,12,4};
static unsigned const b2_4_3[2] = {11,3};
static unsigned const b2_4_4[1] = {0};
static unsigned const* const b2_4[5] = {b2_4_0,b2_4_1,b2_4_2,b2_4_3,b2_4_4};
static unsigned const b2_5_0[6] = {2,10,9,8,7,1};
static unsigned const b2_5_1[5] = {11,20,19,17,6};
static unsigned const b2_5_2[4] = {12,18,16,5};
static unsigned const b2_5_3[3] = {13,15,4};
static unsigned const b2_5_4[2] = {14,3};
static unsigned const b2_5_5[1] = {0};
static unsigned const* const b2_5[6] =
{b2_5_0,b2_5_1,b2_5_2,b2_5_3,b2_5_4,b2_5_5};
static unsigned const b2_6_0[7] = {2,12,11,10,9,8,1};
static unsigned const b2_6_1[6] = {13,27,26,24,21,7};
static unsigned const b2_6_2[5] = {14,25,23,20,6};
static unsigned const b2_6_3[4] = {15,22,19,5};
static unsigned const b2_6_4[3] = {16,18,4};
static unsigned const b2_6_5[2] = {17,3};
static unsigned const b2_6_6[1] = {0};
static unsigned const* const b2_6[7] =
{b2_6_0,b2_6_1,b2_6_2,b2_6_3,b2_6_4,b2_6_5,b2_6_6};
static unsigned const b2_7_0[8] = {2,14,13,12,11,10,9,1};
static unsigned const b2_7_1[7] = {15,35,34,32,29,25,8};
static unsigned const b2_7_2[6] = {16,33,31,28,24,7};
static unsigned const b2_7_3[5] = {17,30,27,23,6};
static unsigned const b2_7_4[4] = {18,26,22,5};
static unsigned const b2_7_5[3] = {19,21,4};
static unsigned const b2_7_6[2] = {20,3};
static unsigned const b2_7_7[1] = {0};
static unsigned const* const b2_7[8] =
{b2_7_0,b2_7_1,b2_7_2,b2_7_3,b2_7_4,b2_7_5,b2_7_6,b2_7_7};
static unsigned const b2_8_0[9] = {2,16,15,14,13,12,11,10,1};
static unsigned const b2_8_1[8] = {17,44,43,41,38,34,29,9};
static unsigned const b2_8_2[7] = {18,42,40,37,33,28,8};
static unsigned const b2_8_3[6] = {19,39,36,32,27,7};
static unsigned const b2_8_4[5] = {20,35,31,26,6};
static unsigned const b2_8_5[4] = {21,30,25,5};
static unsigned const b2_8_6[3] = {22,24,4};
static unsigned const b2_8_7[2] = {23,3};
static unsigned const b2_8_8[1] = {0};
static unsigned const* const b2_8[9] =
{b2_8_0,b2_8_1,b2_8_2,b2_8_3,b2_8_4,b2_8_5,b2_8_6,b2_8_7,b2_8_8};
static unsigned const b2_9_0[10] = {2,18,17,16,15,14,13,12,11,1};
static unsigned const b2_9_1[9] = {19,54,53,51,48,44,39,33,10};
static unsigned const b2_9_2[8] = {20,52,50,47,43,38,32,9};
static unsigned const b2_9_3[7] = {21,49,46,42,37,31,8};
static unsigned const b2_9_4[6] = {22,45,41,36,30,7};
static unsigned const b2_9_5[5] = {23,40,35,29,6};
static unsigned const b2_9_6[4] = {24,34,28,5};
static unsigned const b2_9_7[3] = {25,27,4};
static unsigned const b2_9_8[2] = {26,3};
static unsigned const b2_9_9[1] = {0};
static unsigned const* const b2_9[10] =
{b2_9_0,b2_9_1,b2_9_2,b2_9_3,b2_9_4,b2_9_5,b2_9_6,b2_9_7,b2_9_8,b2_9_9};
static unsigned const b2_10_0[11] = {2,20,19,18,17,16,15,14,13,12,1};
static unsigned const b2_10_1[10] = {21,65,64,62,59,55,50,44,37,11};
static unsigned const b2_10_2[9] = {22,63,61,58,54,49,43,36,10};
static unsigned const b2_10_3[8] = {23,60,57,53,48,42,35,9};
static unsigned const b2_10_4[7] = {24,56,52,47,41,34,8};
static unsigned const b2_10_5[6] = {25,51,46,40,33,7};
static unsigned const b2_10_6[5] = {26,45,39,32,6};
static unsigned const b2_10_7[4] = {27,38,31,5};
static unsigned const b2_10_8[3] = {28,30,4};
static unsigned const b2_10_9[2] = {29,3};
static unsigned const b2_10_10[1] = {0};
static unsigned const* const b2_10[11] =
{b2_10_0,b2_10_1,b2_10_2,b2_10_3,b2_10_4,b2_10_5,b2_10_6,b2_10_7,b2_10_8,
    b2_10_9,b2_10_10};

unsigned const* const* const b2[11] =
{b2_0,b2_1,b2_2,b2_3,b2_4,b2_5,b2_6,b2_7,b2_8,b2_9,b2_10};

int computeTriNodeIndex(int P, int i, int j) {
  int k = P-i-j;
  if(i == P) return 0;
  if(j == P) return 1;
  if(k == P) return 2;
  if(k == 0) return 2+j; // 0-1
  if(i == 0) return 2+(P-1)+k; // 1-2
  if(j == 0) return 2+(P-1)*2+i; // 2-0
  return k*(P-1)-k*(k-1)/2+j+2*P;
}

int getTriNodeIndex(int P, int i, int j) {
  // use a table if its small, otherwise dynamically generate it on the fly
  if(P <= 10)
    return crv::b2[P][i][j];
  else
    return computeTriNodeIndex(P,i,j);
}

static double getTriPartialJacobianDet(Reals nodes,
    int P, int i1, int j1, int i2, int j2) {
  int p00 = getTriNodeIndex(P,i1+1,j1);
  int p01 = getTriNodeIndex(P,i1,j1+1);
  int p10 = getTriNodeIndex(P,i2+1,j2);
  int p11 = getTriNodeIndex(P,i2,j2);
  return apf::cross(nodes[p01]-nodes[p00], nodes[p11]-nodes[p10])[2];
}

static double Nijk(Reals nodes, int d, int I, int J) {
  double sum = 0.;
  int CD = trinomial(2*(d-1),I,J);
  for(int j1 = 0; j1 <= J; ++j1){
    int i1start = std::max(0,I+J-j1-(d-1));
    int i1end = std::min(I,d-1-j1);
    for(int i1 = i1start; i1 <= i1end; ++i1){
      sum += trinomial(d-1,i1,j1)*trinomial(d-1,I-i1,J-j1)
          *getTriPartialJacobianDet(nodes,d,i1,j1,I-i1,J-j1);
    }
  }
  return sum*d*d/CD;
}

OMEGA_H_INLINE LOs getTriJacDetNodes(int P, Reals elemNodes) {
  Write<Real> nodes(P*(2*P-1));
  for (int I = 0; I <= 2*(P-1); ++I)
    for (int J = 0; J <= 2*(P-1)-I; ++J)
      nodes[getTriNodeIndex(2*(P-1),I,J)] = Nijk(elemNodes,P,I,J);
  return Reals(nodes);
}

OMEGA_H_INLINE LO checkMinJacDet(LOs nodes) {
  // first 3 vertices
  for (int i = 0; i < 3; ++i){
    if(nodes[i] < minAcceptable){
      //return -1;
      return i+2;
    }
  }

  apf::MeshEntity* edges[3];
  mesh->getDownward(e,1,edges);
  double minJ = 0, maxJ = 0;
  // Vertices will already be flagged in the first check
  for (int edge = 0; edge < 3; ++edge){
    for (int i = 0; i < 2*(order-1)-1; ++i){
      if (nodes[3+edge*(2*(order-1)-1)+i] < minAcceptable){
        minJ = -1e10;
        apf::NewArray<double> edgeNodes(2*(order-1)+1);
        if(algorithm < 2){
          edgeNodes[0] = nodes[apf::tri_edge_verts[edge][0]];
          edgeNodes[2*(order-1)] = nodes[apf::tri_edge_verts[edge][1]];
          for (int j = 0; j < 2*(order-1)-1; ++j)
            edgeNodes[j+1] = nodes[3+edge*(2*(order-1)-1)+j];
        } else {
          edgeNodes[0] = nodes[apf::tri_edge_verts[edge][0]];
          edgeNodes[1] = nodes[apf::tri_edge_verts[edge][1]];
          for (int j = 0; j < 2*(order-1)-1; ++j)
            edgeNodes[j+2] = nodes[3+edge*(2*(order-1)-1)+j];
          bool done = false;
          bool quality = false;
        }
        if(minJ < minAcceptable){
          return 8+edge;
        }
      }
    }
  }

  bool done = false;
  for (int i = 0; i < (2*order-3)*(2*order-4)/2; ++i){
    if (nodes[6*(order-1)+i] < minAcceptable){
      minJ = -1e10;
      if(minJ < minAcceptable){
        return 14;
      }
    }
  }
  return -1;

}

LOs checkValidity_2d(Mesh *mesh, LOs new_tris) {

  auto fv2v = mesh->ask_down(2, 0).ab2b;
  auto fe2e = mesh->get_adj(2, 1).ab2b;
  auto vertCtrlPts = mesh->get_ctrlPts(0);
  auto edgeCtrlPts = mesh->get_ctrlPts(1);
  auto faceCtrlPts = mesh->get_ctrlPts(2);
  auto dim = mesh->dim();
  auto const n_edge_pts = mesh->n_internal_ctrlPts(1);
  auto order = mesh->get_max_order();

  Write<I8> is_invalid(new_tris.size());

  auto check_validity = OMEGA_H_LAMBDA (LO i) {
    auto tri = new_tris(i);
    auto order = mesh->get_max_order();
    OMEGA_H_CHECK(order == 3);
    LO const ntri_pts = 10;
 
    //TODO recheck indexing
    Write<Real> tri_pts(ntri_pts*dim);
    //query the tri's down verts's ctrl pts and store
    for (LO j = 0; j < 3; ++j) {
      auto p = get_vector<2>(vertCtrlPts, fv2v[tri*3 + j]);
      for (LO k = 0; k < dim; ++k) {
        tri_pts[j*dim + k] = p[k];
      }
    }
    //query the tri's down edge's ctrl pts and store
    for (LO j = 0; j < 3; ++j) {
      for (I8 d = 0; d < dim; ++d) {
        tri_pts[3*dim + j*n_edge_pts*dim + d] = 
          edgeCtrlPts[fe2e[tri*3 + j]*n_edge_pts*dim + d];
        tri_pts[3*dim + j*n_edge_pts*dim + dim + d] = 
          edgeCtrlPts[fe2e[tri*3 + j]*n_edge_pts*dim + dim + d];
      }
    }
    //query the tri's ctrl pt and store
    for (I8 d = 0; d < dim; ++d) {
      tri_pts[9*dim + d] = faceCtrlPts[tri*dim + d];
    }
 
    auto nodes = getTriJacDetNodes(order,elemNodes);

    is_invalid[i] = checkMinJacDet(nodes);
  };
  parallel_for(new_tris.size(), std::move(check_validity));
}
