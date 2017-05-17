#ifndef dagr_mask_h
#define dagr_mask_h

#include "dagr_shared_object.h"
#include "dagr_algorithm.h"
#include "dagr_metadata.h"

#include <string>
#include <vector>

DAGR_SHARED_OBJECT_FORWARD_DECL(dagr_mask)

/// an algorithm that masks a range of values
/**
An algorithm to mask a range of values in an array. Values
in the range are replaced with the mask value.
*/
class dagr_mask : public dagr_algorithm
{
public:
    DAGR_ALGORITHM_STATIC_NEW(dagr_mask)
    ~dagr_mask();

    // set the names of the arrays to apply the mask to
    DAGR_ALGORITHM_VECTOR_PROPERTY(std::string, mask_variable)

    // Set the range identifying values to mask. Values inside
    // this range are masked.  The defaults are (-infinity, infinity].
    DAGR_ALGORITHM_PROPERTY(double, low_threshold_value)
    DAGR_ALGORITHM_PROPERTY(double, high_threshold_value)

    // Set the value used to replace input values that
    // are inside the specified range.
    DAGR_ALGORITHM_PROPERTY(double, mask_value)

protected:
    dagr_mask();

    std::vector<std::string> get_mask_variables(
        const dagr_metadata &request);

private:
    std::vector<dagr_metadata> get_upstream_request(
        unsigned int port,
        const std::vector<dagr_metadata> &input_md,
        const dagr_metadata &request) override;

    const_p_dagr_dataset execute(
        unsigned int port,
        const std::vector<const_p_dagr_dataset> &input_data,
        const dagr_metadata &request) override;

private:
    std::vector<std::string> mask_variables;
    double low_threshold_value;
    double high_threshold_value;
    double mask_value;
};

#endif
