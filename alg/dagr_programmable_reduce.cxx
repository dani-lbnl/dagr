#include "dagr_programmable_reduce.h"
#include "dagr_table.h"

#include <iostream>
#include <limits>

using std::cerr;
using std::endl;
using std::vector;

// --------------------------------------------------------------------------
dagr_programmable_reduce::dagr_programmable_reduce() :
    reduce_callback(nullptr), request_callback(nullptr),
    report_callback(nullptr)
{
    this->set_number_of_input_connections(1);
    this->set_number_of_output_ports(1);
}

// --------------------------------------------------------------------------
std::vector<dagr_metadata>
    dagr_programmable_reduce::initialize_upstream_request(unsigned int port,
    const std::vector<dagr_metadata> &input_md, const dagr_metadata &request)
{
#ifdef DAGR_DEBUG
    cerr << dagr_parallel_id()
        << "dagr_programmable_reduce::initialize_upstream_request" << endl;
#endif
    if (!this->request_callback)
    {
        vector<dagr_metadata> up_reqs(1, request);
        return up_reqs;
    }

    return this->request_callback(port, input_md, request);
}

// --------------------------------------------------------------------------
dagr_metadata dagr_programmable_reduce::initialize_output_metadata(
    unsigned int port, const std::vector<dagr_metadata> &input_md)
{
#ifdef DAGR_DEBUG
    cerr << dagr_parallel_id()
        << "dagr_programmable_reduce::intialize_output_metadata" << endl;
#endif
    if (!this->report_callback)
    {
        dagr_metadata output_md(input_md[0]);
        return output_md;
    }

    return this->report_callback(port, input_md);
}

// --------------------------------------------------------------------------
p_dagr_dataset dagr_programmable_reduce::reduce(
    const const_p_dagr_dataset &left_ds, const const_p_dagr_dataset &right_ds)
{
#ifdef DAGR_DEBUG
    cerr << dagr_parallel_id()
        << "dagr_programmable_reduce::reduce" << endl;
#endif

    if (!this->reduce_callback)
    {
        DAGR_ERROR("a reduce callback has not been provided")
        return nullptr;
    }

    return this->reduce_callback(left_ds, right_ds);
}
