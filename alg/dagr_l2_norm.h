#ifndef dagr_l2_norm_h
#define dagr_l2_norm_h

#include "dagr_shared_object.h"
#include "dagr_algorithm.h"
#include "dagr_metadata.h"

#include <string>
#include <vector>

DAGR_SHARED_OBJECT_FORWARD_DECL(dagr_l2_norm)

/// an algorithm that computes L2 norm
/**
Compute L2 norm
*/
class dagr_l2_norm : public dagr_algorithm
{
public:
    DAGR_ALGORITHM_STATIC_NEW(dagr_l2_norm)
    ~dagr_l2_norm();

    // report/initialize to/from Boost program options
    // objects.
    DAGR_GET_ALGORITHM_PROPERTIES_DESCRIPTION()
    DAGR_SET_ALGORITHM_PROPERTIES()

    // set the arrays that contain the vector components
    // to compute norm from
    DAGR_ALGORITHM_PROPERTY(std::string, component_0_variable)
    DAGR_ALGORITHM_PROPERTY(std::string, component_1_variable)
    DAGR_ALGORITHM_PROPERTY(std::string, component_2_variable)

    // set the name of the array to store the result in.
    // the default is "l2_norm"
    DAGR_ALGORITHM_PROPERTY(std::string, l2_norm_variable)

protected:
    dagr_l2_norm();

    std::string get_component_0_variable(const dagr_metadata &request);
    std::string get_component_1_variable(const dagr_metadata &request);
    std::string get_component_2_variable(const dagr_metadata &request);
    std::string get_l2_norm_variable(const dagr_metadata &request);

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
    std::string component_2_variable;
    std::string l2_norm_variable;
};

#endif
