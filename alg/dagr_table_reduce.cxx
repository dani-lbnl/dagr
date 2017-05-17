#include "dagr_table_reduce.h"
#include "dagr_table.h"

#include <iostream>
#include <limits>

using std::cerr;
using std::endl;
using std::vector;

// --------------------------------------------------------------------------
dagr_table_reduce::dagr_table_reduce()
{
    this->set_number_of_input_connections(1);
    this->set_number_of_output_ports(1);
}

// --------------------------------------------------------------------------
std::vector<dagr_metadata> dagr_table_reduce::initialize_upstream_request(
    unsigned int port,
    const std::vector<dagr_metadata> &input_md,
    const dagr_metadata &request)
{
#ifdef DAGR_DEBUG
    cerr << dagr_parallel_id()
        << "dagr_table_reduce::initialize_upstream_request" << endl;
#endif
    (void) port;
    (void) input_md;

    vector<dagr_metadata> up_reqs(1, request);
    return up_reqs;
}

// --------------------------------------------------------------------------
dagr_metadata dagr_table_reduce::initialize_output_metadata(
    unsigned int port,
    const std::vector<dagr_metadata> &input_md)
{
#ifdef DAGR_DEBUG
    cerr << dagr_parallel_id()
        << "dagr_table_reduce::intialize_output_metadata" << endl;
#endif
    (void) port;

    dagr_metadata output_md(input_md[0]);
    return output_md;
}

// --------------------------------------------------------------------------
p_dagr_dataset dagr_table_reduce::reduce(
    const const_p_dagr_dataset &left_ds,
    const const_p_dagr_dataset &right_ds)
{
#ifdef DAGR_DEBUG
    cerr << dagr_parallel_id()
        << "dagr_table_reduce::reduce" << endl;
#endif
    const_p_dagr_table left_table
        = std::dynamic_pointer_cast<const dagr_table>(left_ds);

    const_p_dagr_table right_table
        = std::dynamic_pointer_cast<const dagr_table>(right_ds);

    p_dagr_table output_table;

    bool left = left_table && *left_table;
    bool right = right_table && *right_table;

    if (left && right)
    {
        output_table
            = std::dynamic_pointer_cast<dagr_table>(left_table->new_copy());

        output_table->concatenate_rows(right_table);
    }
    else
    if (left)
    {
        output_table
            = std::dynamic_pointer_cast<dagr_table>(left_table->new_copy());
    }
    else
    if (right)
    {
        output_table
            = std::dynamic_pointer_cast<dagr_table>(right_table->new_copy());
    }

    return output_table;
}
