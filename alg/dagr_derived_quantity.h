#ifndef dagr_derived_quantity_h
#define dagr_derived_quantity_h

#include "dagr_programmable_algorithm.h"
#include "dagr_metadata.h"
#include "dagr_dataset_fwd.h"
#include "dagr_shared_object.h"

#include <string>
#include <vector>

DAGR_SHARED_OBJECT_FORWARD_DECL(dagr_derived_quantity)

/// a programmable algorithm specialized for simple array based computations
/**
A programmable algorithm specialized for simple array based
computations. A user provided callable(see set execute_callback)
which operates on one or more arrays(the dependent variables) to
produce a new array (the derived quantity). The purpose of this
class is to implement the request and report phases of the pipeline
consistently for this common use case. An implementation specific
context(operation_name) differentiates between multiple instances
in the same pipeline.
*/
class dagr_derived_quantity : public dagr_programmable_algorithm
{
public:
    DAGR_ALGORITHM_STATIC_NEW(dagr_derived_quantity)
    ~dagr_derived_quantity();

    // report/initialize to/from Boost program options
    // objects.
    DAGR_GET_ALGORITHM_PROPERTIES_DESCRIPTION()
    DAGR_SET_ALGORITHM_PROPERTIES()

    // set/get the list of arrays that are needed to produce
    // the derived quantity
    DAGR_ALGORITHM_VECTOR_PROPERTY(std::string, dependent_variable)

    // set/get the array that is produced
    DAGR_ALGORITHM_PROPERTY(std::string, derived_variable)

    // set/get the contextual name that differentiates this
    // instance from others in the same pipeline.
    DAGR_ALGORITHM_PROPERTY(std::string, operation_name)

    // set the callable that implements to derived quantity
    // computation
    using dagr_programmable_algorithm::set_execute_callback;
    using dagr_programmable_algorithm::get_execute_callback;

protected:
    dagr_derived_quantity();

private:
    // specialized report and request implementations that
    // process the input and output array lists in a standardized
    // manner.
    dagr_metadata get_output_metadata(unsigned int port,
        const std::vector<dagr_metadata> &input_md) override;

    std::vector<dagr_metadata> get_upstream_request(
        unsigned int port, const std::vector<dagr_metadata> &input_md,
        const dagr_metadata &request) override;

    // hide the programmable algorithm report request callbacks
    using dagr_programmable_algorithm::set_report_callback;
    using dagr_programmable_algorithm::get_report_callback;

    using dagr_programmable_algorithm::set_request_callback;
    using dagr_programmable_algorithm::get_request_callback;

    // extracts variable from incoming request if the property
    // is not set
    std::string get_derived_variable(const dagr_metadata &request);

    // extracts dependent variables from the incoming request
    // if the coresponding property is not set
    void get_dependent_variables(const dagr_metadata &request,
        std::vector<std::string> &dep_vars);

private:
    std::string operation_name;   
    std::vector<std::string> dependent_variables;
    std::string derived_variable;
};

#endif
