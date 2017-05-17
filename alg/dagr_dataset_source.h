#ifndef dagr_dataset_source_h
#define dagr_dataset_source_h

#include "dagr_algorithm.h"
#include "dagr_metadata.h"
#include "dagr_dataset_fwd.h"
#include "dagr_shared_object.h"

DAGR_SHARED_OBJECT_FORWARD_DECL(dagr_dataset_source)

/**
An algorithm that serves up user provided data and metadata.
This algorithm can be used to inject a dataset constructed
on outside of DAGR into a DAGR pipleine.
*/
class dagr_dataset_source : public dagr_algorithm
{
public:
    DAGR_ALGORITHM_STATIC_NEW(dagr_dataset_source)
    ~dagr_dataset_source();

    // report/initialize to/from Boost program options
    // objects.
    DAGR_GET_ALGORITHM_PROPERTIES_DESCRIPTION()
    DAGR_SET_ALGORITHM_PROPERTIES()

    // set/get the dataset to insert into the pipeline
    DAGR_ALGORITHM_PROPERTY(p_dagr_dataset, dataset)

    // set/get the metadata to insert into the pipeline
    DAGR_ALGORITHM_PROPERTY(dagr_metadata, metadata)

protected:
    dagr_dataset_source();

private:
    dagr_metadata get_output_metadata(unsigned int port,
        const std::vector<dagr_metadata> &input_md) override;

    const_p_dagr_dataset execute(unsigned int port,
        const std::vector<const_p_dagr_dataset> &input_data,
        const dagr_metadata &request) override;

private:
    p_dagr_dataset dataset;
    dagr_metadata metadata;
};

#endif
