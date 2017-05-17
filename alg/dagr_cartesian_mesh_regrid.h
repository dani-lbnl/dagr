#ifndef dagr_cartesian_mesh_regrid_h
#define dagr_cartesian_mesh_regrid_h

#include "dagr_shared_object.h"
#include "dagr_algorithm.h"
#include "dagr_metadata.h"
#include "dagr_variant_array_fwd.h"

#include <string>
#include <vector>

DAGR_SHARED_OBJECT_FORWARD_DECL(dagr_cartesian_mesh_regrid)

/// transfer data between overlapping meshes of potentially different resolution
/**
an algorithm that transfers data between cartesian meshes
defined in the same world coordinate system but potentially
different resolutions. nearest or linear interpolation are
supported. the first input is the target mesh. the second input
is the source mesh. the arrays to move from source to target
can be selected using add_array api or in the request
key regrid_source_arrays. this is a spatial regriding operation
for temporal regriding see dagr_mesh_temporal_regrid.
*/
class dagr_cartesian_mesh_regrid : public dagr_algorithm
{
public:
    DAGR_ALGORITHM_STATIC_NEW(dagr_cartesian_mesh_regrid)
    ~dagr_cartesian_mesh_regrid();

    // report/initialize to/from Boost program options
    // objects.
    DAGR_GET_ALGORITHM_PROPERTIES_DESCRIPTION()
    DAGR_SET_ALGORITHM_PROPERTIES()

    // set the list of arrays to move from the source
    // to the target
    DAGR_ALGORITHM_PROPERTY(std::vector<std::string>, source_arrays)

    // clear the list of arrays to move from the source
    // to the target
    void clear_source_arrays();

    // set names of arrays to move. any arrays
    // named here will be requested from the source
    // mesh(input 2) and moved onto the target mesh
    // (input 1) with the same name.
    void add_source_array(const std::string &array);

    // set the interpolation mode used in transfering
    // data between meshes of differing resolution.
    // in nearest mode value at the nearest grid point
    // is used, in linear mode bi/tri linear interpolation
    // is used.
    enum {nearest=0, linear=1};
    DAGR_ALGORITHM_PROPERTY(int, interpolation_mode)
    void set_interpolation_mode_nearest(){ interpolation_mode = nearest; }
    void set_interpolation_mode_linear(){ interpolation_mode = linear; }

protected:
    dagr_cartesian_mesh_regrid();

private:

    dagr_metadata get_output_metadata(
        unsigned int port,
        const std::vector<dagr_metadata> &input_md) override;

    std::vector<dagr_metadata> get_upstream_request(
        unsigned int port,
        const std::vector<dagr_metadata> &input_md,
        const dagr_metadata &request) override;

    const_p_dagr_dataset execute(
        unsigned int port,
        const std::vector<const_p_dagr_dataset> &input_data,
        const dagr_metadata &request) override;

private:
    std::vector<std::string> source_arrays;
    int interpolation_mode;
};

#endif
