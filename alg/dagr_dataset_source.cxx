#include "dagr_dataset_source.h"
#include "dagr_dataset.h"

#include <string>
#include <vector>

#if defined(DAGR_HAS_BOOST)
#include <boost/program_options.hpp>
#endif

using std::cerr;
using std::endl;
using std::string;
using std::vector;

// --------------------------------------------------------------------------
dagr_dataset_source::dagr_dataset_source()
{
    this->set_number_of_input_connections(0);
    this->set_number_of_output_ports(1);
}

// --------------------------------------------------------------------------
dagr_dataset_source::~dagr_dataset_source()
{}

#if defined(DAGR_HAS_BOOST)
// --------------------------------------------------------------------------
void dagr_dataset_source::get_properties_description(
    const std::string &prefix, options_description &global_opts)
{
    (void)prefix;
    (void)global_opts;
}

// --------------------------------------------------------------------------
void dagr_dataset_source::set_properties(const std::string &prefix,
    variables_map &opts)
{
    (void)prefix;
    (void)opts;
}
#endif

// --------------------------------------------------------------------------
dagr_metadata dagr_dataset_source::get_output_metadata(unsigned int port,
    const std::vector<dagr_metadata> &input_md)
{
#ifdef DAGR_DEBUG
    cerr << dagr_parallel_id()
        << "dagr_dataset_source::get_output_metadata" << endl;
#endif
    (void)port;
    (void)input_md;

    return this->metadata;
}

// --------------------------------------------------------------------------
const_p_dagr_dataset dagr_dataset_source::execute(unsigned int port,
    const std::vector<const_p_dagr_dataset> &input_data,
    const dagr_metadata &request)
{
#ifdef DAGR_DEBUG
    cerr << dagr_parallel_id()
        << "dagr_dataset_source::execute" << endl;
#endif
    (void)port;
    (void)input_data;
    (void)request;

    return this->dataset;
}
