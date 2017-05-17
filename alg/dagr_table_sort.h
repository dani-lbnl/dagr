#ifndef dagr_table_sort_h
#define dagr_table_sort_h

#include "dagr_shared_object.h"
#include "dagr_algorithm.h"
#include "dagr_metadata.h"

#include <string>
#include <vector>

DAGR_SHARED_OBJECT_FORWARD_DECL(dagr_table_sort)

/// an algorithm that sorts a table in ascending order
class dagr_table_sort : public dagr_algorithm
{
public:
    DAGR_ALGORITHM_STATIC_NEW(dagr_table_sort)
    ~dagr_table_sort();

    // report/initialize to/from Boost program options
    // objects.
    DAGR_GET_ALGORITHM_PROPERTIES_DESCRIPTION()
    DAGR_SET_ALGORITHM_PROPERTIES()

    // set the column to sort by
    DAGR_ALGORITHM_PROPERTY(std::string, index_column)
    DAGR_ALGORITHM_PROPERTY(int, index_column_id)

    // enable/disable stable sorting. default 0
    DAGR_ALGORITHM_PROPERTY(int, stable_sort)

    void enable_stable_sort(){ set_stable_sort(1); }
    void disable_stable_sort(){ set_stable_sort(0); }

protected:
    dagr_table_sort();

private:
    const_p_dagr_dataset execute(
        unsigned int port,
        const std::vector<const_p_dagr_dataset> &input_data,
        const dagr_metadata &request) override;

private:
    std::string index_column;
    int index_column_id;
    int stable_sort;
};

#endif
