#include "dagr_dataset_capture.h"
#include "dagr_dataset.h"

#include <string>
#include <vector>

#if defined(DAGR_HAS_BOOST)
#include <boost/program_options.hpp>
#endif

using std::cerr;
using std::endl;

// --------------------------------------------------------------------------
dagr_dataset_capture::dagr_dataset_capture()
{
    this->set_number_of_input_connections(1);
    this->set_number_of_output_ports(1);
}

// --------------------------------------------------------------------------
dagr_dataset_capture::~dagr_dataset_capture()
{}

#if defined(DAGR_HAS_BOOST)
// --------------------------------------------------------------------------
void dagr_dataset_capture::get_properties_description(
    const std::string &prefix, options_description &global_opts)
{
    (void)prefix;
    (void)global_opts;
}

// --------------------------------------------------------------------------
void dagr_dataset_capture::set_properties(const std::string &prefix,
    variables_map &opts)
{
    (void)prefix;
    (void)opts;
}
#endif

// --------------------------------------------------------------------------
const_p_dagr_dataset dagr_dataset_capture::execute(unsigned int port,
    const std::vector<const_p_dagr_dataset> &input_data,
    const dagr_metadata &request)
{
#ifdef DAGR_DEBUG
    cerr << dagr_parallel_id()
        << "dagr_dataset_capture::execute" << endl;
#endif
    (void)port;
    (void)request;
    this->dataset = input_data[0];
    return this->dataset;
}
