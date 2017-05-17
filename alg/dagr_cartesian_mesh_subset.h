#ifndef dagr_cartesian_mesh_subset_h
#define dagr_cartesian_mesh_subset_h

#include "dagr_shared_object.h"
#include "dagr_algorithm.h"
#include "dagr_metadata.h"
#include "dagr_variant_array_fwd.h"

#include <string>
#include <vector>

DAGR_SHARED_OBJECT_FORWARD_DECL(dagr_cartesian_mesh_subset)

/// applies a subset given in world coordinates to the upstream request
/**
an algorithm that applies a subset specified in
world coordinates to upstream requests. the subset
is specified as bounding box of the form [x_low to x_high,
y_low to y_high, z_low to z_high]. The subset can be either
the smallest subset containing the bounding box or the
largest set contained by the bounding box, and is controled
by the cover_bounds property.
*/
class dagr_cartesian_mesh_subset : public dagr_algorithm
{
public:
    DAGR_ALGORITHM_STATIC_NEW(dagr_cartesian_mesh_subset)
    ~dagr_cartesian_mesh_subset();

    // report/initialize to/from Boost program options
    // objects.
    DAGR_GET_ALGORITHM_PROPERTIES_DESCRIPTION()
    DAGR_SET_ALGORITHM_PROPERTIES()

    // define the bounding box of the subset
    // this algorithm converts this into an
    // extent into the upstream dataset.
    DAGR_ALGORITHM_PROPERTY(std::vector<double>, bounds);

    void set_bounds(double low_x, double high_x,
        double low_y, double high_y, double low_z, double high_z)
    { this->set_bounds({low_x, high_x, low_y, high_y, low_z, high_z}); }

    // control how bounds are converted. if true
    // smallest subset covering the bounding box is
    // used. if false the largest subset contained
    // by the bounding box is used.
    DAGR_ALGORITHM_PROPERTY(bool, cover_bounds)

protected:
    dagr_cartesian_mesh_subset();

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
    std::vector<double> bounds;
    bool cover_bounds;

    // internals
    std::vector<unsigned long> extent;
};

#endif
