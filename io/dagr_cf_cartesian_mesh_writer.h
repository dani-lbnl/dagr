#ifndef dagr_cf_cartesian_mesh_writer_h
#define dagr_cf_cartesian_mesh_writer_h

#include "dagr_shared_object.h"
#include "dagr_algorithm.h"
#include "dagr_metadata.h"

#include <vector>
#include <string>

DAGR_SHARED_OBJECT_FORWARD_DECL(dagr_cf_cartesian_mesh_writer)

/**
an algorithm that writes cartesian meshes in NetCDF CF format.
*/
class dagr_cf_cartesian_mesh_writer : public dagr_algorithm
{
public:
    DAGR_ALGORITHM_STATIC_NEW(dagr_cf_cartesian_mesh_writer)
    ~dagr_cf_cartesian_mesh_writer();

    // report/initialize to/from Boost program options
    // objects.
    DAGR_GET_ALGORITHM_PROPERTIES_DESCRIPTION()
    DAGR_SET_ALGORITHM_PROPERTIES()

    // set the output filename. for time series the substring
    // %t% is replaced with the current time step.
    DAGR_ALGORITHM_PROPERTY(std::string, file_name)

    // control how many time steps are written to each file
    DAGR_ALGORITHM_PROPERTY(unsigned int, steps_per_file)

protected:
    dagr_cf_cartesian_mesh_writer();

private:
    const_p_dagr_dataset execute(
        unsigned int port,
        const std::vector<const_p_dagr_dataset> &input_data,
        const dagr_metadata &request) override;

private:
    std::string file_name;
    unsigned int steps_per_file;
};

#endif
