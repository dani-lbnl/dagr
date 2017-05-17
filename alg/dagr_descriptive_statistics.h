#ifndef dagr_descriptive_statistics_h
#define dagr_descriptive_statistics_h

#include "dagr_shared_object.h"
#include "dagr_algorithm.h"
#include "dagr_metadata.h"

#include <string>
#include <vector>

DAGR_SHARED_OBJECT_FORWARD_DECL(dagr_descriptive_statistics)

/// compute descriptive statistics over a set of arrays.
/**
compute the min, max, avg, median, standard deviation of a
set of named arrays. the results are returned in a table.
*/
class dagr_descriptive_statistics : public dagr_algorithm
{
public:
    DAGR_ALGORITHM_STATIC_NEW(dagr_descriptive_statistics)
    ~dagr_descriptive_statistics();

    // report/initialize to/from Boost program options
    // objects.
    DAGR_GET_ALGORITHM_PROPERTIES_DESCRIPTION()
    DAGR_SET_ALGORITHM_PROPERTIES()

    // set/get the list of arrays that are needed to produce
    // the derived quantity
    DAGR_ALGORITHM_VECTOR_PROPERTY(std::string, dependent_variable)

protected:
    dagr_descriptive_statistics();

private:
    std::vector<dagr_metadata> get_upstream_request(
        unsigned int port, const std::vector<dagr_metadata> &input_md,
        const dagr_metadata &request) override;

    const_p_dagr_dataset execute(
        unsigned int port, const std::vector<const_p_dagr_dataset> &input_data,
        const dagr_metadata &request) override;

    void get_dependent_variables(const dagr_metadata &request,
        std::vector<std::string> &dep_vars);
private:
    std::vector<std::string> dependent_variables;
};

#endif
