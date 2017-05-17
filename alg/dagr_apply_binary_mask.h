#ifndef dagr_apply_binary_mask_h
#define dagr_apply_binary_mask_h

#include "dagr_shared_object.h"
#include "dagr_algorithm.h"
#include "dagr_metadata.h"

#include <string>
#include <vector>

DAGR_SHARED_OBJECT_FORWARD_DECL(dagr_apply_binary_mask)

/// an algorithm that applies a binary mask multiplicatively
/**
an algorithm that applies a binary mask multiplicatively to all
arrays in the input dataset. where mask is 1 values are passed
through, where mask is 0 values are removed.
*/
class dagr_apply_binary_mask : public dagr_algorithm
{
public:
    DAGR_ALGORITHM_STATIC_NEW(dagr_apply_binary_mask)
    ~dagr_apply_binary_mask();

    // set the name of the output array
    DAGR_ALGORITHM_PROPERTY(std::string, mask_variable)

    // set the arrays to mask. if empty no arrays will be
    // requested, but all present will be masked
    DAGR_ALGORITHM_VECTOR_PROPERTY(std::string, mask_array)

protected:
    dagr_apply_binary_mask();

private:
    //dagr_metadata get_output_metadata(unsigned int port,
    //    const std::vector<dagr_metadata> &input_md) override;

    std::vector<dagr_metadata> get_upstream_request(
        unsigned int port, const std::vector<dagr_metadata> &input_md,
        const dagr_metadata &request) override;

    const_p_dagr_dataset execute(unsigned int port,
        const std::vector<const_p_dagr_dataset> &input_data,
        const dagr_metadata &request) override;

private:
    std::string mask_variable;
    std::vector<std::string> mask_arrays;
};

#endif
