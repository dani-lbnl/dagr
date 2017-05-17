#ifndef dagr_vorticity_h
#define dagr_vorticity_h

#include "dagr_shared_object.h"
#include "dagr_algorithm.h"
#include "dagr_metadata.h"

#include <string>
#include <vector>

DAGR_SHARED_OBJECT_FORWARD_DECL(dagr_vorticity)

/// an algorithm that computes vorticity
/**
Compute vorticity from a vector field.
*/
class dagr_vorticity : public dagr_algorithm
{
public:
    DAGR_ALGORITHM_STATIC_NEW(dagr_vorticity)
    ~dagr_vorticity();

    // report/initialize to/from Boost program options
    // objects.
    DAGR_GET_ALGORITHM_PROPERTIES_DESCRIPTION()
    DAGR_SET_ALGORITHM_PROPERTIES()

    // set the arrays that contain the vector components
    // to compute vorticity from
    DAGR_ALGORITHM_PROPERTY(std::string, component_0_variable)
    DAGR_ALGORITHM_PROPERTY(std::string, component_1_variable)

    // set the name of the array to store the result in.
    // the default is "vorticity"
    DAGR_ALGORITHM_PROPERTY(std::string, vorticity_variable)

protected:
    dagr_vorticity();

    std::string get_component_0_variable(const dagr_metadata &request);
    std::string get_component_1_variable(const dagr_metadata &request);
    std::string get_vorticity_variable(const dagr_metadata &request);

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
    std::string component_0_variable;
    std::string component_1_variable;
    std::string vorticity_variable;
};

#endif
