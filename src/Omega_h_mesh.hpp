#ifndef OMEGA_H_MESH_HPP
#define OMEGA_H_MESH_HPP

#include <array>
#include <map>
#include <set>
#include <string>
#include <vector>

#include <Omega_h_adj.hpp>
#include <Omega_h_comm.hpp>
#include <Omega_h_dist.hpp>
#include <Omega_h_library.hpp>
#include <Omega_h_tag.hpp>

namespace Omega_h {

namespace inertia {
struct Rib;
}

struct ClassPair {
  inline ClassPair() = default;
  inline ClassPair(Int t_dim, LO t_id) : dim(t_dim), id(t_id) {}
  Int dim;
  LO id;
  OMEGA_H_INLINE bool operator<(ClassPair const& other) const {
    if (dim != other.dim) return dim < other.dim;
    if (id != other.id) return id < other.id;
    return false;
  }
};

using ClassSets = std::map<std::string, std::vector<ClassPair>>;

class Mesh {
 public:
  Mesh();
  Mesh(Library* library);
  void set_library(Library* library);
  void set_comm(CommPtr const& comm);
  void set_family(Omega_h_Family family);
  void set_dim(Int dim_in);
  void set_verts(LO nverts_in);
  void set_ents(Int ent_dim, Adj down);
  void set_parents(Int ent_dim, Parents parents);
  Library* library() const;
  CommPtr comm() const;
  Omega_h_Parting parting() const;
  inline Int dim() const {
    OMEGA_H_CHECK(0 <= dim_ && dim_ <= 3);
    return dim_;
  }
  inline Omega_h_Family family() const { return family_; }
  LO nents(Int ent_dim) const;
  LO nelems() const;
  LO nregions() const;
  LO nfaces() const;
  LO nedges() const;
  LO nverts() const;

  void set_verts_type(LO nverts_in);
  LO npyrams() const;
  LO nwedges() const;
  LO nhexs() const;
  LO ntets() const;
  LO nquads() const;
  LO ntris() const;
  LO nregions_mix() const;
  LO nfaces_mix() const;
  LO nedges_mix() const;
  LO nverts_mix() const;

  GO nglobal_ents(Int dim);
  template <typename T>
  void add_tag(Int dim, std::string const& name, Int ncomps);
  template <typename T>
  void add_tag(Int dim, std::string const& name, Int ncomps, Read<T> array,
      bool internal = false);
  template <typename T>
  void set_tag(
      Int dim, std::string const& name, Read<T> array, bool internal = false);
  TagBase const* get_tagbase(Int dim, std::string const& name) const;
  template <typename T>
  Tag<T> const* get_tag(Int dim, std::string const& name) const;
  template <typename T>
  Read<T> get_array(Int dim, std::string const& name) const;
  void remove_tag(Int dim, std::string const& name);
  bool has_tag(Int dim, std::string const& name) const;
  Int ntags(Int dim) const;
  TagBase const* get_tag(Int dim, Int i) const;
  bool has_ents(Int dim) const;
  bool has_adj(Int from, Int to) const;
  Adj get_adj(Int from, Int to) const;
  Adj ask_down(Int from, Int to);
  LOs ask_verts_of(Int dim);
  LOs ask_elem_verts();
  Adj ask_up(Int from, Int to);
  Graph ask_star(Int dim);
  Graph ask_dual();

  void set_ents(Topo_type high_type, Topo_type low_type, Adj h2l);
  LO nents(Topo_type ent_type) const;
  Int ent_dim(Topo_type ent_type) const;
  template <typename T>
  void add_tag(Topo_type ent_type, std::string const& name, Int ncomps);
  template <typename T>
  void add_tag(Topo_type ent_type, std::string const& name, Int ncomps,
               Read<T> array, bool internal = false);
  template <typename T>
  void set_tag(Topo_type ent_type, std::string const& name, Read<T> array,
               bool internal = false);
  TagBase const* get_tagbase(Topo_type ent_type, std::string const& name) const;
  template <typename T>
  Tag<T> const* get_tag(Topo_type ent_type, std::string const& name) const;
  template <typename T>
  Read<T> get_array(Topo_type ent_type, std::string const& name) const;
  void remove_tag(Topo_type ent_type, std::string const& name);
  bool has_tag(Topo_type ent_type, std::string const& name) const;
  Int ntags(Topo_type ent_type) const;
  TagBase const* get_tag(Topo_type ent_type, Int i) const;
  bool has_ents(Topo_type ent_type) const;
  bool has_adj(Topo_type from_type, Topo_type to_type) const;
  Adj get_adj(Topo_type from_type, Topo_type to_type) const;
  Adj ask_down(Topo_type from_type, Topo_type to_type);
  Adj ask_up(Topo_type from_type, Topo_type to_type);
  LOs ask_verts_of(Topo_type ent_type);

  void set_matched(I8 is_matched);
  inline I8 is_matched() const { return matched_; }

/** ask_revClass (Int edim, LOs class_ids): takes input of entity dimension
 * 'edim', and an 1d array of model entity IDs to return
 * a CSR structure (Adj) containing IDs of mesh entities classified on the 
 * requested input model entities. Note here that 'edim' is equal to the
 * model entity dimension as well as the dimension of returned mesh entities
 * NOTE: if the model entity is a region, the memory usage is high
 */
  Adj ask_revClass (Int edim, LOs class_ids);

/** ask_revClass (Int edim): see ask_revClass (Int edim, LOs class_ids) above.
 * Here, the output is for all model entities of dimension 'edim' instead
 * of a input list
 */
  Adj ask_revClass (Int edim);

/** ask_revClass_downAdj (Int from, Int to): takes input of a higher
 * dimension 'from' and a lower dimension 'to'. The value of 'from' is equal
 * to the mesh and model entity dimensions used to get reverse class.
 * (similar to 'edim' for ask_revClass functions above.) This function can be
 * understood as a two step process. Firstly, for all the model entities
 * of dimension 'from', we get the reverse classified mesh entities.
 * Then combine the reverse classification, and downward adjacency information.
 * The final output is a CSR containing downward adjacent
 * mesh entities of dimension 'to' which bound the reverse classified mesh
 * entities of dimension 'from'.
 */
  Adj ask_revClass_downAdj (Int from, Int to);

/** has_revClass (Int edim): Input is a entity dimension 'edim'. This function
 * checks if the reverse classification for that dimension is present in
 * memory or not.
 */
  bool has_revClass (Int edim) const;

/** Takes input of model entity IDs, entity dimension, name of field and number
 * of components, to create a the rcField. This function
 * is used when fields are to be stored with mesh entities returned by
 * ask_revClass (Int edim, LOs class_ids)
 */
  template <typename T>
  void add_rcField(LOs class_ids, Int ent_dim, std::string const& name,
                   Int ncomps);

/** Takes input of entity dimension, name of rcField, values of rcField, and
 * stores the values in memory.
 */
  template <typename T>
  void set_rcField_array(Int ent_dim, std::string const& name,
                         Read<T> array);

/** Takes input of entity dimension, name of field and deletes the field
 * information from memory
 */
  void remove_rcField(Int ent_dim, std::string const& name);

  Adj get_revClass (Int edim) const;

/** Takes input of entity dimension, name of field and number of components.
 * to create a space where the rcField values can be stored. This function
 * is used when fields are to be stored with mesh entities returned by
 * ask_revClass (Int edim)
 */
  template <typename T>
  void add_rcField(Int ent_dim, std::string const& name, Int ncomps);

  template <typename T>
  Read<T> get_rcField_array(Int ent_dim, std::string const& name) const;
  void reduce_rcField(Int ent_dim, std::string const& name,
                            Omega_h_Op op);
  template <typename T>
  void add_rcField(Int ent_dim, std::string const& name, Int ncomps,
                   Read<T> array);
  void sync_rcField(Int ent_dim, std::string const& name);
  bool has_rcField(Int ent_dim, std::string const& name) const;

  template <typename T>
  void change_tagTorc(Int ent_dim, Int ncomps, std::string const& name,
                      LOs class_ids);
  template <typename T>
  void change_tagToMesh(Int ent_dim, Int ncomps, std::string const& name,
                        LOs class_ids);

  void change_all_rcFieldsToMesh();
  void change_all_rcFieldsTorc();
  bool has_anyrcField();
  bool has_allMeshTags();

  void set_curved(I8 is_curved);
  void set_max_order(Int max_order);
  void change_max_order(Int max_order);
  inline I8 is_curved() const { return curved_; }
  inline Int get_max_order() const { return max_order_; }
  Int n_internal_ctrlPts(Int edim);
  void add_tags_for_ctrlPts();
  void remove_tags_for_ctrlPts();
  void set_tag_for_ctrlPts(Int ent_dim, Reals ctrlPts);
  Reals get_ctrlPts(Int ent_dim);
  void set_down(Int high_dim, Int low_dim, LOs hl2l);

 public:
  typedef std::shared_ptr<TagBase> TagPtr;
  typedef std::shared_ptr<Adj> AdjPtr;
  typedef std::shared_ptr<Dist> DistPtr;
  typedef std::shared_ptr<inertia::Rib> RibPtr;
  typedef std::shared_ptr<Parents> ParentPtr;
  typedef std::shared_ptr<Children> ChildrenPtr;

 private:
  typedef std::vector<TagPtr> TagVector;
  typedef TagVector::iterator TagIter;
  typedef TagVector::const_iterator TagCIter;
  TagIter tag_iter(Int dim, std::string const& name);
  TagCIter tag_iter(Int dim, std::string const& name) const;
  void check_dim(Int dim) const;
  void check_dim2(Int dim) const;
  void add_adj(Int from, Int to, Adj adj);
  Adj derive_adj(Int from, Int to);
  Adj ask_adj(Int from, Int to);
  void react_to_set_tag(Int dim, std::string const& name);
  Omega_h_Family family_;
  Int dim_;
  CommPtr comm_;
  Int parting_;
  Int nghost_layers_;
  LO nents_[DIMS];
  TagVector tags_[DIMS];
  AdjPtr adjs_[DIMS][DIMS];
  Remotes owners_[DIMS];
  DistPtr dists_[DIMS];
  RibPtr rib_hints_;
  ParentPtr parents_[DIMS];
  ChildrenPtr children_[DIMS][DIMS];
  Library* library_;

  TagIter tag_iter(Topo_type ent_type, std::string const& name);
  TagCIter tag_iter(Topo_type ent_type, std::string const& name) const;
  void check_type(Topo_type ent_type) const;
  void check_type2(Topo_type ent_type) const;
  void add_adj(Topo_type from_type, Topo_type to_type, Adj adj);
  Adj derive_adj(Topo_type from_type, Topo_type to_type);
  Adj ask_adj(Topo_type from_type, Topo_type to_type);
  void react_to_set_tag(Topo_type ent_type, std::string const& name);
  LO nents_type_[TOPO_TYPES];
  TagVector tags_type_[TOPO_TYPES];
  AdjPtr adjs_type_[TOPO_TYPES][TOPO_TYPES];

  I8 matched_ = -1;
  Remotes match_owners_[DIMS];
  LOs model_ents_[DIMS];
  LOs model_matches_[DIMS-1];

  AdjPtr revClass_[DIMS];

  I8 curved_ = -1;
  Int max_order_ = -1;

 public:
  void add_coords(Reals array);
  Reals coords() const;
  void set_coords(Reals const& array);
  void add_coords_mix(Reals array);
  Reals coords_mix() const;
  Read<GO> globals(Int dim) const;
  Reals ask_lengths();
  Reals ask_qualities();
  Reals ask_sizes();
  Bytes ask_levels(Int dim);
  Bytes ask_leaves(Int dim);
  Parents ask_parents(Int child_dim);
  Children ask_children(Int parent_dim, Int child_dim);
  bool has_any_parents() const;
  void set_owners(Int dim, Remotes owners);
  Remotes ask_owners(Int dim);
  Read<I8> owned(Int dim);
  Dist ask_dist(Int dim);
  Int nghost_layers() const;
  void set_parting(Omega_h_Parting parting_in, Int nlayers, bool verbose);
  void set_parting(Omega_h_Parting parting_in, bool verbose = false);
  void balance(bool predictive = false);
  Graph ask_graph(Int from, Int to);
  template <typename T>
  Read<T> sync_array(Int ent_dim, Read<T> a, Int width);
  template <typename T>
  Future<T> isync_array(Int ent_dim, Read<T> a, Int width);
  template <typename T>
  Read<T> sync_subset_array(
      Int ent_dim, Read<T> a_data, LOs a2e, T default_val, Int width);
  template <typename T>
  Read<T> reduce_array(Int ent_dim, Read<T> a, Int width, Omega_h_Op op);
  template <typename T>
  Read<T> owned_array(Int ent_dim, Read<T> a, Int width);
  void sync_tag(Int dim, std::string const& name);
  void reduce_tag(Int dim, std::string const& name, Omega_h_Op op);
  bool operator==(Mesh& other);
  Real min_quality();
  Real max_length();
  bool could_be_shared(Int ent_dim) const;
  bool owners_have_all_upward(Int ent_dim) const;
  bool have_all_upward() const;
  Mesh copy_meta() const;
  RibPtr rib_hints() const;
  void set_rib_hints(RibPtr hints);
  Real imbalance(Int ent_dim = -1) const;

  void balance(Reals weights);

  void set_model_ents(Int ent_dim, LOs Ids); 
  void set_model_matches(Int ent_dim, LOs matches); 
  LOs get_model_ents(Int ent_dim); 
  LOs get_model_matches(Int ent_dim); 
  void set_match_owners(Int dim, Remotes owners);
  Remotes ask_match_owners(Int dim);
  c_Remotes matches_[DIMS];
  void set_matches(Int dim, c_Remotes matches);
  c_Remotes get_matches(Int dim);
  void swap_root_owner(Int dim);
  void sync_tag_matched(Int dim, std::string const& name);
  template <typename T>
  Read<T> sync_array_matched(Int ent_dim, Read<T> a, Int width);

  Adj derive_revClass(Int edim);

 public:
  ClassSets class_sets;
};

bool can_print(Mesh* mesh);

Real repro_sum_owned(Mesh* mesh, Int dim, Reals a);

Reals average_field(Mesh* mesh, Int dim, LOs a2e, Int ncomps, Reals v2x);
Reals average_field(Mesh* mesh, Int dim, Int ncomps, Reals v2x);

using TagSet = std::array<std::set<std::string>, DIMS>;

void get_all_dim_tags(Mesh* mesh, Int dim, TagSet* tags);
TagSet get_all_mesh_tags(Mesh* mesh);
void ask_for_mesh_tags(Mesh* mesh, TagSet const& tags);

void reorder_by_hilbert(Mesh* mesh);
void reorder_by_globals(Mesh* mesh);

LOs ents_on_closure(
    Mesh* mesh, std::set<std::string> const& class_names, Int ent_dim);

LOs nodes_on_closure(
    Mesh* mesh, std::set<std::string> const& class_names, Graph nodes2ents[4]);

void get_all_type_tags(Mesh* mesh, Int dim, Topo_type ent_type, TagSet* tags);

// workaround CUDA compiler bug
#ifdef OMEGA_H_USE_CUDA
__host__
#endif
    void
    assign(Mesh& a, Mesh const& b);

#define OMEGA_H_EXPL_INST_DECL(T)                                              \
  extern template Tag<T> const* Mesh::get_tag<T>(                              \
      Int dim, std::string const& name) const;                                 \
  extern template Read<T> Mesh::get_array<T>(Int dim, std::string const& name) \
      const;                                                                   \
  extern template void Mesh::add_tag<T>(                                       \
      Int dim, std::string const& name, Int ncomps);                           \
  extern template void Mesh::add_tag<T>(Int dim, std::string const& name,      \
      Int ncomps, Read<T> array, bool internal);                               \
  extern template void Mesh::set_tag(                                          \
      Int dim, std::string const& name, Read<T> array, bool internal);         \
  extern template Read<T> Mesh::sync_array(Int ent_dim, Read<T> a, Int width); \
  extern template Future<T> Mesh::isync_array(                                 \
      Int ent_dim, Read<T> a, Int width);                                      \
  extern template Read<T> Mesh::owned_array(                                   \
      Int ent_dim, Read<T> a, Int width);                                      \
  extern template Read<T> Mesh::sync_subset_array(                             \
      Int ent_dim, Read<T> a_data, LOs a2e, T default_val, Int width);         \
  extern template Read<T> Mesh::reduce_array(                                  \
      Int ent_dim, Read<T> a, Int width, Omega_h_Op op);                       \
  extern template Read<T> Mesh::get_array<T>(Topo_type ent_type, std::string const& name) \
      const;                                                                   \
  extern template void Mesh::add_tag<T>(                                       \
      Topo_type ent_type, std::string const& name, Int ncomps);                           \
  extern template void Mesh::add_tag<T>(Topo_type ent_type, std::string const& name,      \
      Int ncomps, Read<T> array, bool internal);                               \
  extern template void Mesh::set_tag(                                          \
      Topo_type ent_type, std::string const& name, Read<T> array, bool internal);         \
  extern template void Mesh::change_tagTorc<T>(                                \
      Int ent_dim, Int ncomps, std::string const& name, LOs class_ids);        \
  extern template void Mesh::change_tagToMesh<T>(                              \
      Int ent_dim, Int ncomps, std::string const& name, LOs class_ids);        \
  extern template Read<T> Mesh::get_rcField_array<T>(                          \
      Int dim, std::string const& name) const;                                 \
  extern template void Mesh::add_rcField<T>(                                   \
      Int dim, std::string const& name, Int ncomps);                           \
  extern template void Mesh::add_rcField<T>(                                   \
      LOs class_ids, Int dim, std::string const& name, Int ncomps);            \
  extern template void Mesh::add_rcField<T>(                                   \
      Int dim, std::string const& name, Int ncomps, Read<T> array);            \
  extern template void Mesh::set_rcField_array(                                \
      Int dim, std::string const& name, Read<T> array);
OMEGA_H_EXPL_INST_DECL(I8)
OMEGA_H_EXPL_INST_DECL(I32)
OMEGA_H_EXPL_INST_DECL(I64)
OMEGA_H_EXPL_INST_DECL(Real)
#undef OMEGA_H_EXPL_INST_DECL

}  // namespace Omega_h

#endif
