#ifndef dagr_dataset_capture_h
#define dagr_dataset_capture_h

#include "dagr_algorithm.h"
#include "dagr_metadata.h"
#include "dagr_dataset_fwd.h"
#include "dagr_shared_object.h"

DAGR_SHARED_OBJECT_FORWARD_DECL(dagr_dataset_capture)

/**
An algorithm that takes a reference to dataset produced
by the upstream algorithm it is connected to. The dataset
is passed through so that this can be inserted anywhere
giving one access to the intermediate data.
*/
class dagr_dataset_capture : public dagr_algorithm
{
public:
    DAGR_ALGORITHM_STATIC_NEW(dagr_dataset_capture)
    ~dagr_dataset_capture();

    // report/initialize to/from Boost program options
    // objects.
    DAGR_GET_ALGORITHM_PROPERTIES_DESCRIPTION()
    DAGR_SET_ALGORITHM_PROPERTIES()

    // set/get the dataset from the last pipeline update
    DAGR_ALGORITHM_PROPERTY(const_p_dagr_dataset, dataset)

protected:
    dagr_dataset_capture();

private:
    const_p_dagr_dataset execute(unsigned int port,
        const std::vector<const_p_dagr_dataset> &input_data,
        const dagr_metadata &request) override;

private:
    const_p_dagr_dataset dataset;
};

#endif
