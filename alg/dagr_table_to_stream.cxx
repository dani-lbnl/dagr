#include "dagr_table_to_stream.h"

#include "dagr_table.h"
#include "dagr_array_collection.h"
#include "dagr_variant_array.h"
#include "dagr_metadata.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <set>
#include <cmath>

#if defined(DAGR_HAS_BOOST)
#include <boost/program_options.hpp>
#endif

#if defined(DAGR_HAS_MPI)
#include <mpi.h>
#endif

using std::string;
using std::vector;
using std::set;
using std::cerr;
using std::endl;

//#define DAGR_DEBUG

// --------------------------------------------------------------------------
dagr_table_to_stream::dagr_table_to_stream() : stream(&std::cerr)
{
    this->set_number_of_input_connections(1);
    this->set_number_of_output_ports(1);
}

// --------------------------------------------------------------------------
dagr_table_to_stream::~dagr_table_to_stream()
{}

#if defined(DAGR_HAS_BOOST)
// --------------------------------------------------------------------------
void dagr_table_to_stream::get_properties_description(
    const string &prefix, options_description &global_opts)
{
    options_description opts("Options for "
        + (prefix.empty()?"dagr_table_to_stream":prefix));

    opts.add_options()
        DAGR_POPTS_GET(std::string, prefix, header,
            "text to precede table output")
        DAGR_POPTS_GET(std::string, prefix, footer,
            "text to follow table output")
        DAGR_POPTS_GET(std::string, prefix, stream,
            "name of stream to send output to. stderr, stdout")
        ;
    global_opts.add(opts);
}

// --------------------------------------------------------------------------
void dagr_table_to_stream::set_properties(
    const string &prefix, variables_map &opts)
{
    DAGR_POPTS_SET(opts, std::string, prefix, header)
    DAGR_POPTS_SET(opts, std::string, prefix, footer)
    DAGR_POPTS_SET(opts, std::string, prefix, stream)
}
#endif

// --------------------------------------------------------------------------
void dagr_table_to_stream::set_stream(std::ostream &s)
{
    this->stream = &s;
}

// --------------------------------------------------------------------------
void dagr_table_to_stream::set_stream(const std::string &s)
{
    if ((s == "stderr") || (s == "err") || (s == "cerr") || (s == "std::cerr"))
    {
        this->set_stream_to_stderr();
    }
    else
    if ((s == "stdout") || (s == "out") || (s == "cout") || (s == "std::cout"))
    {
        this->set_stream_to_stdout();
    }
    else
    {
        DAGR_ERROR("unknown stream requested \"" << s << "\"")
    }
}

// --------------------------------------------------------------------------
void dagr_table_to_stream::set_stream_to_stderr()
{
    this->stream = &std::cerr;
}

// --------------------------------------------------------------------------
void dagr_table_to_stream::set_stream_to_stdout()
{
    this->stream = &std::cout;
}

// --------------------------------------------------------------------------
const_p_dagr_dataset dagr_table_to_stream::execute(
    unsigned int port,
    const std::vector<const_p_dagr_dataset> &input_data,
    const dagr_metadata &request)
{
#ifdef DAGR_DEBUG
    cerr << dagr_parallel_id() << "dagr_table_to_stream::execute" << endl;
#endif
    (void)port;
    (void)request;

    if (!this->stream)
    {
        DAGR_ERROR("output stream not set")
        return nullptr;
    }

    // get the input
    const_p_dagr_table in_table
        = std::dynamic_pointer_cast<const dagr_table>(input_data[0]);

    // in parallel only rank 0 is required to have data
    int rank = 0;
#if defined(DAGR_HAS_MPI)
    int init = 0;
    MPI_Initialized(&init);
    if (init)
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif
    if (!in_table)
    {
        if (rank == 0)
        {
            DAGR_ERROR("empty input")
        }
        return nullptr;
    }

    // pass the data through so this can sit anywhere in the pipeline
    p_dagr_table out_table = dagr_table::New();
    out_table->shallow_copy(std::const_pointer_cast<dagr_table>(in_table));

    if (!this->header.empty())
        *this->stream << this->header << std::endl;
    out_table->to_stream(*this->stream);
    if (!this->footer.empty())
        *this->stream << this->footer << std::endl;

    return out_table;
}
