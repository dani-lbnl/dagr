#ifndef dagr_connected_components_h
#define dagr_connected_components_h

#include "dagr_shared_object.h"
#include "dagr_algorithm.h"
#include "dagr_metadata.h"

#include <string>
#include <vector>

DAGR_SHARED_OBJECT_FORWARD_DECL(dagr_connected_components)

/// an algorithm that computes connected component labeling
/**
an algorithm that computes connected component labeling
for 1D, 2D, and 3D data. The labels are computed form a
binary segmentation which is computed using threshold
operation where values in a range (low, high] are in the
segmentation.
*/
class dagr_connected_components : public dagr_algorithm
{
public:
    DAGR_ALGORITHM_STATIC_NEW(dagr_connected_components)
    ~dagr_connected_components();

    // set the name of the output array
    DAGR_ALGORITHM_PROPERTY(std::string, label_variable)

    // set the array to threshold
    DAGR_ALGORITHM_PROPERTY(std::string, threshold_variable)

    // Set the threshold range. The defaults are
    // (-infinity, infinity].
    DAGR_ALGORITHM_PROPERTY(double, low_threshold_value)
    DAGR_ALGORITHM_PROPERTY(double, high_threshold_value)

protected:
    dagr_connected_components();

    std::string get_label_variable(const dagr_metadata &request);
    std::string get_threshold_variable(const dagr_metadata &request);

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
    std::string label_variable;
    std::string threshold_variable;
    double low_threshold_value;
    double high_threshold_value;
};

#endif
