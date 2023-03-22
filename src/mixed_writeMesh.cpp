#include "MeshSim.h"
#include "SimDiscrete.h"
#include "SimMessages.h"
//#include "SimError.h"
//#include "SimErrorCodes.h"
//#include "SimMeshingErrorCodes.h"
//#include "SimDiscreteErrorCodes.h"
#include <iostream>
using namespace std;

#include "Omega_h_comm.hpp"
#include "Omega_h_mesh.hpp"
#include "Omega_h_file.hpp"
using namespace Omega_h;

void messageHandler(int type, const char *msg)
{
  switch (type) {
  case Sim_InfoMsg:
    cout<<"Info: "<<msg<<endl;
    break;
  case Sim_DebugMsg:
    cout<<"Debug: "<<msg<<endl;
    break;
  case Sim_WarningMsg:
    cout<<"Warning: "<<msg<<endl;
    break;
  case Sim_ErrorMsg:
    cout<<"Error: "<<msg<<endl;
    break;
  }
  return;
}

void finalize_write(int numVerts, const double *coords, int numElems,
                    int *elementType, int *elementData, pVertex *vReturn,
                    pEntity *eReturn, const char *mesh_path,
                    const char *model_path) {
  Sim_logOn("importData1.log");
  MS_init();
  Sim_readLicenseFile(0);
  pMesh meshtest;
  SimDiscrete_start(0);
  Sim_setMessageHandler(messageHandler);
  pProgress progress = Progress_new();
  Progress_setDefaultCallback(progress);

  meshtest = M_new(0,0);
  if(M_importFromData(meshtest, numVerts, coords, numElems,
                      elementType, elementData, vReturn, eReturn,
                      progress)) {
    cerr<<"Error importing mesh data"<<endl;
    M_release(meshtest);
  }

  pDiscreteModel modeltest = DM_createFromMesh(meshtest, 0, progress);
  if(!modeltest) {
    cerr<<"Error creating Discrete model from mesh"<<endl;
    M_release(meshtest);
  }

  DM_findEdgesByFaceNormals(modeltest, 0, progress);
  DM_eliminateDanglingEdges(modeltest, progress);
  if(DM_completeTopology(modeltest, progress)) {
    cerr<<"Error completing Discrete model topology"<<endl;
    M_release(meshtest);
    GM_release(modeltest);
  }

  GM_write(modeltest, model_path, 0, progress);
  M_write(meshtest, mesh_path, 0, progress);

  M_release(meshtest);
  GM_release(modeltest);
  Progress_delete(progress);
  SimDiscrete_stop(0);
  Sim_unregisterAllKeys();
  MS_exit();
  Sim_logOff();
}

void hex_gen(CommPtr comm, std::string const mesh_dir) {
  auto mesh = Mesh(comm->library());
  try {
    int numVerts = 8;
    double coords[8*3] =  {
                           0.000, 0.000, 0.000,
                           1.000, 0.000, 0.000,
                           1.000, 1.000, 0.000,
                           0.000, 1.000, 0.000,
                           0.000, 0.000, 1.000,
                           1.000, 0.000, 1.000,
                           1.000, 1.000, 1.000,
                           0.000, 1.000, 1.000
                          };
    int numElems = 1;
    int elementType[1] = {13};
    int elementData[8] = {0,1,2,3,4,5,6,7};
    pVertex vReturn[8];
    pEntity eReturn[1];
    std::string mesh_path = mesh_dir + "/Example_hex.sms";
    std::string model_path = mesh_dir + "/Example_hex.smd";
    finalize_write(numVerts, coords, numElems, elementType, elementData,
                   vReturn, eReturn, mesh_path.c_str(), model_path.c_str());
  }/* 
  catch (pSimError err) {
    cerr<<"SimModSuite error caught:"<<endl;
    cerr<<"  Error code: "<<SimError_code(err)<<endl;
    cerr<<"  Error string: "<<SimError_toString(err)<<endl;
    SimError_delete(err);
  } */
  catch (...) {
    cerr<<"Unhandled exception caught"<<endl;
  }
}

void wedge_gen(CommPtr comm, std::string const mesh_dir) {
  auto mesh = Mesh(comm->library());
  try {
    int numVerts = 6;
    double coords[6*3] =  {0.000, 0.000, 0.000,
                           1.000, 0.000, 0.000,
                           0.500, 0.866, 0.000,
                           0.000, 0.000, 1.000,
                           1.000, 0.000, 1.000,
                           0.500, 0.866, 1.000,
                          };
    int numElems = 1;
    int elementType[1] = {12};
    int elementData[6] = {0,1,2,3,4,5};
    pVertex vReturn[6];
    pEntity eReturn[1];
    std::string mesh_path = mesh_dir + "/Example_wedge.sms";
    std::string model_path = mesh_dir + "/Example_wedge.smd";
    finalize_write(numVerts, coords, numElems, elementType, elementData,
                   vReturn, eReturn, mesh_path.c_str(), model_path.c_str());
  }/* 
  catch (pSimError err) {
    cerr<<"SimModSuite error caught:"<<endl;
    cerr<<"  Error code: "<<SimError_code(err)<<endl;
    cerr<<"  Error string: "<<SimError_toString(err)<<endl;
    SimError_delete(err);
  } */
  catch (...) {
    cerr<<"Unhandled exception caught"<<endl;
  }
}

void pyramid_gen(CommPtr comm, std::string const mesh_dir) {
  auto mesh = Mesh(comm->library());
  try {
    int numVerts = 5;
    double coords[5*3] =  {
                           0.000, 0.000, 1.000,
                           1.000, 0.000, 1.000,
                           1.000, 1.000, 1.000,
                           0.000, 1.000, 1.000,
                           0.500, 0.500, 2.000
                          };
    int numElems = 1;
    int elementType[1] = {11};
    int elementData[5] = {0,1,2,3,4};
    pVertex vReturn[5];
    pEntity eReturn[1];
    std::string mesh_path = mesh_dir + "/Example_pym.sms";
    std::string model_path = mesh_dir + "/Example_pym.smd";
    finalize_write(numVerts, coords, numElems, elementType, elementData,
                   vReturn, eReturn, mesh_path.c_str(), model_path.c_str());
  }
 /* 
  catch (pSimError err) {
    cerr<<"SimModSuite error caught:"<<endl;
    cerr<<"  Error code: "<<SimError_code(err)<<endl;
    cerr<<"  Error string: "<<SimError_toString(err)<<endl;
    SimError_delete(err);
  }*/
  catch (...) {
    cerr<<"Unhandled exception caught"<<endl;
  }
}

void tetOnWedge_gen(CommPtr comm, std::string const mesh_dir) {
  auto mesh = Mesh(comm->library());
  try {
    int numVerts = 7;
    double coords[7*3] =  {0.000, 0.000, 0.000,
                           1.000, 0.000, 0.000,
                           0.500, 0.866, 0.000,
                           0.000, 0.000, 1.000,
                           1.000, 0.000, 1.000,
                           0.500, 0.866, 1.000,
                           0.500, 0.289, 1.866,
                          };
    int numElems = 2;
    int elementType[2] = {12, 10};
    int elementData[6+4] = {0,1,2,3,4,5, 3,4,5,6};
    pVertex vReturn[7];
    pEntity eReturn[2];
    std::string mesh_path = mesh_dir + "/Example_tet_wedge.sms";
    std::string model_path = mesh_dir + "/Example_tet_wedge.smd";
    finalize_write(numVerts, coords, numElems, elementType, elementData,
                   vReturn, eReturn, mesh_path.c_str(), model_path.c_str());
  }/* 
  catch (pSimError err) {
    cerr<<"SimModSuite error caught:"<<endl;
    cerr<<"  Error code: "<<SimError_code(err)<<endl;
    cerr<<"  Error string: "<<SimError_toString(err)<<endl;
    SimError_delete(err);
  }*/ 
  catch (...) {
    cerr<<"Unhandled exception caught"<<endl;
  }
}

void pymOnHex_gen(CommPtr comm, std::string const mesh_dir) {
  auto mesh = Mesh(comm->library());
  try {
    int numVerts = 9;
    double coords[9*3] =  {
                           0.000, 0.000, 0.000,
                           1.000, 0.000, 0.000,
                           1.000, 1.000, 0.000,
                           0.000, 1.000, 0.000,
                           0.000, 0.000, 1.000,
                           1.000, 0.000, 1.000,
                           1.000, 1.000, 1.000,
                           0.000, 1.000, 1.000,
                           0.500, 0.500, 2.000
                          };
    int numElems = 2;
    int elementType[2] = {13, 11};
    int elementData[8+5] = {0,1,2,3,4,5,6,7, 4,5,6,7,8};
    pVertex vReturn[9];
    pEntity eReturn[2];
    std::string mesh_path = mesh_dir + "/Example_pym_hex.sms";
    std::string model_path = mesh_dir + "/Example_pym_hex.smd";
    finalize_write(numVerts, coords, numElems, elementType, elementData,
                   vReturn, eReturn, mesh_path.c_str(), model_path.c_str());
  } /*
    catch (pSimError err) {
    cerr<<"SimModSuite error caught:"<<endl;
    cerr<<"  Error code: "<<SimError_code(err)<<endl;
    cerr<<"  Error string: "<<SimError_toString(err)<<endl;
    SimError_delete(err);
  } */
  catch (...) {
    cerr<<"Unhandled exception caught"<<endl;
  }
}

void allType_gen(CommPtr comm, std::string const mesh_dir) {
  auto mesh = Mesh(comm->library());
  try {
    int numVerts = 12;
    double coords[12*3] = {0.000, 0.000, 0.000,
                           1.000, 0.000, 0.000,
                           0.500, 0.866, 0.000,
                           0.000, 0.000, 1.000, 
                           1.000, 0.000, 1.000,
                           0.500, 0.866, 1.000,
                           0.500, 0.289, 1.866,
                           1.000, 1.500, 0.500,
                           0.000, -1.000, 0.000,
                           1.000, -1.000, 0.000,
                           0.000, -1.000, 1.000,
                           1.000, -1.000, 1.000
                          };
    int numElems = 4;
    int elementType[4] = {12, 10, 11, 13};
    int elementData[6+4+5+8] = {0,1,2,3,4,5, 3,4,5,6, 5,4,1,2,7,
8,9,1,0,10,11,4,3};
    pVertex vReturn[12]; 
    pEntity eReturn[4]; 
    std::string mesh_path = mesh_dir + "/Example_allType.sms";
    std::string model_path = mesh_dir + "/Example_allType.smd";
    finalize_write(numVerts, coords, numElems, elementType, elementData,
                   vReturn, eReturn, mesh_path.c_str(), model_path.c_str());
  }/* 
  catch (pSimError err) {
    cerr<<"SimModSuite error caught:"<<endl;
    cerr<<"  Error code: "<<SimError_code(err)<<endl;
    cerr<<"  Error string: "<<SimError_toString(err)<<endl;
    SimError_delete(err);
  }*/ 
  catch (...) {
    cerr<<"Unhandled exception caught"<<endl;
  }
}

void tri_gen(CommPtr comm, std::string const mesh_dir) {
  auto mesh = Mesh(comm->library());
  try {
    int numVerts = 3;
    double coords[3*3] =  {0.000, 0.000, 0.000,
                           1.000, 0.000, 0.000,
                           0.000, 1.000, 0.000
                          };
    int numElems = 1;
    int elementType[1] = {5};
    int elementData[3] = {0,1,2};
    pVertex vReturn[3];
    pEntity eReturn[1];
    std::string mesh_path = mesh_dir + "/Example_tri.sms";
    std::string model_path = mesh_dir + "/Example_tri.smd";
    finalize_write(numVerts, coords, numElems, elementType, elementData,
                   vReturn, eReturn, mesh_path.c_str(), model_path.c_str());
  } /*
    catch (pSimError err) {
    cerr<<"SimModSuite error caught:"<<endl;
    cerr<<"  Error code: "<<SimError_code(err)<<endl;
    cerr<<"  Error string: "<<SimError_toString(err)<<endl;
    SimError_delete(err);
  }*/ 
  catch (...) {
    cerr<<"Unhandled exception caught"<<endl;
  }
}

void twotri_square_gen(CommPtr comm, std::string const mesh_dir) {
  auto mesh = Mesh(comm->library());
  try {
    int numVerts = 4;
    double coords[4*3] =  {0.000, 0.000, 0.000,
                           1.000, 0.000, 0.000,
                           0.000, 1.000, 0.000,
                           1.000, 1.000, 0.000
                          };
    int numElems = 2;
    int elementType[2] = {5, 5};
    int elementData[6] = {0,1,2, 2,1,3};
    pVertex vReturn[4];
    pEntity eReturn[2];
    std::string mesh_path = mesh_dir + "/Example_2tri_square.sms";
    std::string model_path = mesh_dir + "/Example_2tri_square.smd";
    finalize_write(numVerts, coords, numElems, elementType, elementData,
                   vReturn, eReturn, mesh_path.c_str(), model_path.c_str());
  } /*
    catch (pSimError err) {
    cerr<<"SimModSuite error caught:"<<endl;
    cerr<<"  Error code: "<<SimError_code(err)<<endl;
    cerr<<"  Error string: "<<SimError_toString(err)<<endl;
    SimError_delete(err);
  }*/ 
  catch (...) {
    cerr<<"Unhandled exception caught"<<endl;
  }
}

void tet_gen(CommPtr comm, std::string const mesh_dir) {
  auto mesh = Mesh(comm->library());
  try {
    int numVerts = 4;
    double coords[4*3] =  {0.000, 0.000, 0.000,
                           1.000, 0.000, 0.000,
                           0.000, 1.000, 0.000,
                           0.000, 0.000, 1.000
                          };
    int numElems = 1;
    int elementType[1] = {10};
    int elementData[4] = {0,1,2,3};
    pVertex vReturn[4];
    pEntity eReturn[1];
    std::string mesh_path = mesh_dir + "/Example_tet.sms";
    std::string model_path = mesh_dir + "/Example_tet.smd";
    finalize_write(numVerts, coords, numElems, elementType, elementData,
                   vReturn, eReturn, mesh_path.c_str(), model_path.c_str());
  }/*
  catch (pSimError err) {
    cerr<<"SimModSuite error caught:"<<endl;
    cerr<<"  Error code: "<<SimError_code(err)<<endl;
    cerr<<"  Error string: "<<SimError_toString(err)<<endl;
    SimError_delete(err);
  }*/ catch (...) {
    cerr<<"Unhandled exception caught"<<endl;
  }
}

int main(int argc, char *argv[]) {

  if (argc != 2) {
    Omega_h_fail("Usage: a.out <path to meshes directory>\n");
  }
  auto const mesh_dir = argv[1];
  auto lib = Library(&argc, &argv);
  auto comm = lib.world();

  hex_gen(comm, mesh_dir);
  wedge_gen(comm, mesh_dir);
  pyramid_gen(comm, mesh_dir);
  tetOnWedge_gen(comm, mesh_dir);
  pymOnHex_gen(comm, mesh_dir);
  allType_gen(comm, mesh_dir);
  tri_gen(comm, mesh_dir);
  tet_gen(comm, mesh_dir);
  twotri_square_gen(comm, mesh_dir);

  return 0;
}
