#include "dagr_programmable_algorithm.h"

#include "dagr_dataset.h"

using std::cerr;
using std::endl;

// --------------------------------------------------------------------------
dagr_programmable_algorithm::dagr_programmable_algorithm()
{
    this->set_number_of_input_connections(1);
    this->set_number_of_output_ports(1);

    // install default callbacks
    this->use_default_report_action();
    this->use_default_request_action();
    this->use_default_execute_action();
}

// --------------------------------------------------------------------------
dagr_programmable_algorithm::~dagr_programmable_algorithm()
{}

// --------------------------------------------------------------------------
void dagr_programmable_algorithm::use_default_report_action()
{
    this->set_report_callback(
        [](unsigned int, const std::vector<dagr_metadata> &input_md)
            -> dagr_metadata
        {
            // the default implementation passes meta data through
            if (input_md.size())
                return input_md[0];
            return dagr_metadata();
        });
}

// --------------------------------------------------------------------------
void dagr_programmable_algorithm::use_default_request_action()
{
    this->set_request_callback(
        [this](unsigned int, const std::vector<dagr_metadata> &,
            const dagr_metadata &request) -> std::vector<dagr_metadata>
        {
            // default implementation forwards request upstream
            return std::vector<dagr_metadata>(
                this->get_number_of_input_connections(), request);
        });
}

// --------------------------------------------------------------------------
void dagr_programmable_algorithm::use_default_execute_action()
{
    this->set_execute_callback(
        [] (unsigned int, const std::vector<const_p_dagr_dataset> &,
            const dagr_metadata &) -> const_p_dagr_dataset
        {
            // default implementation does nothing
            return p_dagr_dataset();
        });
}

// --------------------------------------------------------------------------
dagr_metadata dagr_programmable_algorithm::get_output_metadata(
    unsigned int port,
    const std::vector<dagr_metadata> &input_md)
{
#ifdef DAGR_DEBUG
    cerr << dagr_parallel_id()
        << "dagr_programmable_algorithm::get_output_metadata" << endl;
#endif

    return this->report_callback(port, input_md);
}

// --------------------------------------------------------------------------
std::vector<dagr_metadata>
dagr_programmable_algorithm::get_upstream_request(
    unsigned int port,
    const std::vector<dagr_metadata> &input_md,
    const dagr_metadata &request)
{
#ifdef DAGR_DEBUG
    cerr << dagr_parallel_id()
        << "dagr_programmable_algorithm::get_upstream_request" << endl;
#endif

    return this->request_callback(port, input_md, request);
}


// --------------------------------------------------------------------------
const_p_dagr_dataset dagr_programmable_algorithm::execute(
    unsigned int port,
    const std::vector<const_p_dagr_dataset> &input_data,
    const dagr_metadata &request)
{
#ifdef DAGR_DEBUG
    cerr << dagr_parallel_id()
        << "dagr_programmable_algorithm::execute" << endl;
#endif

    return this->execute_callback(port, input_data, request);
}
