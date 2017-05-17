#include "dagr_cartesian_mesh_subset.h"

#include "dagr_cartesian_mesh.h"
#include "dagr_array_collection.h"
#include "dagr_variant_array.h"
#include "dagr_metadata.h"
#include "dagr_coordinate_util.h"

#include <algorithm>
#include <iostream>

#if defined(DAGR_HAS_BOOST)
#include <boost/program_options.hpp>
#endif

using std::string;
using std::vector;
using std::cerr;
using std::endl;

//#define DAGR_DEBUG

// --------------------------------------------------------------------------
dagr_cartesian_mesh_subset::dagr_cartesian_mesh_subset()
    : bounds({0.0,0.0,0.0,0.0,0.0,0.0}), cover_bounds(false)
{
    this->set_number_of_input_connections(1);
    this->set_number_of_output_ports(1);
}

// --------------------------------------------------------------------------
dagr_cartesian_mesh_subset::~dagr_cartesian_mesh_subset()
{}

#if defined(DAGR_HAS_BOOST)
// --------------------------------------------------------------------------
void dagr_cartesian_mesh_subset::get_properties_description(
    const string &prefix, options_description &global_opts)
{
    options_description opts("Options for "
        + (prefix.empty()?"dagr_cartesian_mesh_subset":prefix));

    opts.add_options()
        DAGR_POPTS_GET(vector<double>, prefix, bounds,
            "bounding box given by x0,x1,y0,y1,z0,z1")
        DAGR_POPTS_GET(bool, prefix, cover_bounds,
            "(T)use smallest subset covering or (F)largest "
            "subset contained by bounds")
        ;

    global_opts.add(opts);
}

// --------------------------------------------------------------------------
void dagr_cartesian_mesh_subset::set_properties(
    const string &prefix, variables_map &opts)
{
    DAGR_POPTS_SET(opts, vector<double>, prefix, bounds)
    DAGR_POPTS_SET(opts, bool, prefix, cover_bounds)
}
#endif

// --------------------------------------------------------------------------
dagr_metadata dagr_cartesian_mesh_subset::get_output_metadata(
    unsigned int port,
    const std::vector<dagr_metadata> &input_md)
{
#ifdef DAGR_DEBUG
    cerr << dagr_parallel_id()
        << "dagr_cartesian_mesh_subset::get_output_metadata" << endl;
#endif
    (void)port;

    dagr_metadata coords;
    const_p_dagr_variant_array x;
    const_p_dagr_variant_array y;
    const_p_dagr_variant_array z;

    if (input_md[0].get("coordinates", coords)
        || !(x = coords.get("x")) || !(y = coords.get("y"))
        || !(z = coords.get("z")))
    {
        DAGR_ERROR("Input metadata has invalid coordinates")
        return dagr_metadata();
    }

    this->extent.resize(6, 0UL);
    if (dagr_coordinate_util::bounds_to_extent(
        this->bounds.data(), x, y, z, this->extent.data()))
    {
        DAGR_ERROR("Failed to convert bounds to extent")
        return dagr_metadata();
    }

    dagr_metadata out_md(input_md[0]);
    out_md.insert("whole_extent", this->extent);
    return out_md;
}

// --------------------------------------------------------------------------
std::vector<dagr_metadata> dagr_cartesian_mesh_subset::get_upstream_request(
    unsigned int port, const std::vector<dagr_metadata> &input_md,
    const dagr_metadata &request)
{
    (void)port;
    (void)input_md;

    vector<dagr_metadata> up_reqs(1, request);

    up_reqs[0].insert("extent", this->extent);

    return up_reqs;
}

// --------------------------------------------------------------------------
const_p_dagr_dataset dagr_cartesian_mesh_subset::execute(
    unsigned int port, const std::vector<const_p_dagr_dataset> &input_data,
    const dagr_metadata &request)
{
#ifdef DAGR_DEBUG
    cerr << dagr_parallel_id()
        << "dagr_cartesian_mesh_subset::execute" << endl;
#endif
    (void)port;
    (void)request;

    p_dagr_cartesian_mesh in_target
        = std::dynamic_pointer_cast<dagr_cartesian_mesh>(
            std::const_pointer_cast<dagr_dataset>(input_data[0]));

    if (!in_target)
    {
        DAGR_ERROR("invalid input dataset")
        return nullptr;
    }

    // pass input through via shallow copy
    p_dagr_cartesian_mesh target = dagr_cartesian_mesh::New();
    target->shallow_copy(in_target);

    return target;
}
