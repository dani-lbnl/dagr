#ifndef dagr_binary_segmentation_h
#define dagr_binary_segmentation_h

#include "dagr_shared_object.h"
#include "dagr_algorithm.h"
#include "dagr_metadata.h"

#include <string>
#include <vector>

DAGR_SHARED_OBJECT_FORWARD_DECL(dagr_binary_segmentation)

/// an algorithm that computes a binary segmentation
/**
an algorithm that computes a binary segmentation for 1D, 2D,
and 3D data. The segmentation is computed using threshold
operation where values in a range (low, high] are assigned
1 else 0.
*/
class dagr_binary_segmentation : public dagr_algorithm
{
public:
    DAGR_ALGORITHM_STATIC_NEW(dagr_binary_segmentation)
    ~dagr_binary_segmentation();

    // set the name of the output array
    DAGR_ALGORITHM_PROPERTY(std::string, segmentation_variable)

    // set the array to threshold
    DAGR_ALGORITHM_PROPERTY(std::string, threshold_variable)

    // Set the threshold range. The defaults are
    // (-infinity, infinity].
    DAGR_ALGORITHM_PROPERTY(double, low_threshold_value)
    DAGR_ALGORITHM_PROPERTY(double, high_threshold_value)

protected:
    dagr_binary_segmentation();

    std::string get_segmentation_variable(const dagr_metadata &request);
    std::string get_threshold_variable(const dagr_metadata &request);

private:
    dagr_metadata get_output_metadata(unsigned int port,
        const std::vector<dagr_metadata> &input_md) override;

    std::vector<dagr_metadata> get_upstream_request(
        unsigned int port, const std::vector<dagr_metadata> &input_md,
        const dagr_metadata &request) override;

    const_p_dagr_dataset execute(unsigned int port,
        const std::vector<const_p_dagr_dataset> &input_data,
        const dagr_metadata &request) override;

private:
    std::string segmentation_variable;
    std::string threshold_variable;
    double low_threshold_value;
    double high_threshold_value;
};

#endif
