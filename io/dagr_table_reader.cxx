#include "dagr_table_reader.h"
#include "dagr_table.h"
#include "dagr_binary_stream.h"
#include "dagr_coordinate_util.h"
#include "dagr_file_util.h"

#include <algorithm>
#include <cstring>
#include <cstdio>
#include <errno.h>

using std::string;
using std::endl;
using std::cerr;

#if defined(DAGR_HAS_BOOST)
#include <boost/program_options.hpp>
#endif

#if defined(DAGR_HAS_MPI)
#include <mpi.h>
#endif

// PIMPL idiom
struct dagr_table_reader::dagr_table_reader_internals
{
    dagr_table_reader_internals()
        : number_of_steps(0) {}

    void clear();

    static p_dagr_table read_table(
        const std::string &file_name, bool distribute);

    p_dagr_table table;
    dagr_metadata metadata;
    unsigned long number_of_steps;
    std::vector<unsigned long> step_counts;
    std::vector<unsigned long> step_offsets;
    std::vector<unsigned long> step_ids;
};

// --------------------------------------------------------------------------
void dagr_table_reader::dagr_table_reader_internals::clear()
{
    this->table = nullptr;
    this->number_of_steps = 0;
    this->metadata.clear();
    this->step_counts.clear();
    this->step_offsets.clear();
    this->step_ids.clear();
}

// --------------------------------------------------------------------------
p_dagr_table
dagr_table_reader::dagr_table_reader_internals::read_table(
    const std::string &file_name, bool distribute)
{
    dagr_binary_stream stream;
#if !defined(DAGR_HAS_MPI)
    (void)distribute;
#else
    int init = 0;
    int rank = 0;
    MPI_Initialized(&init);
    if (init)
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // rank 0 will read the data, must be rank 0 for the
    // case where using as a serial reader, but running in
    // parallel. rank 0 is assumed to have the data in the
    // serial table based algorithms.
    const int root_rank = 0;
    if (rank == root_rank)
    {
#endif
        if (dagr_file_util::read_stream(file_name.c_str(),
            "dagr_table", stream))
        {
            DAGR_ERROR("Failed to read dagr_table from \""
                << file_name << "\"")
            return nullptr;

        }
#if defined(DAGR_HAS_MPI)
        if (init && distribute)
            stream.broadcast();
    }
    else
    if (init && distribute)
    {
        stream.broadcast();
    }
    else
    {
        return nullptr;
    }
#endif
    // deserialize the binary rep
    p_dagr_table table = dagr_table::New();
    table->from_stream(stream);
    return table;
}


// --------------------------------------------------------------------------
dagr_table_reader::dagr_table_reader() : generate_original_ids(0)
{
    this->internals = new dagr_table_reader_internals;
}

// --------------------------------------------------------------------------
dagr_table_reader::~dagr_table_reader()
{
    delete this->internals;
}

#if defined(DAGR_HAS_BOOST)
// --------------------------------------------------------------------------
void dagr_table_reader::get_properties_description(
    const string &prefix, options_description &global_opts)
{
    options_description opts("Options for "
        + (prefix.empty()?"dagr_table_reader":prefix));

    opts.add_options()
        DAGR_POPTS_GET(string, prefix, file_name,
            "a file name to read")
        DAGR_POPTS_GET(string, prefix, index_column,
            "name of the column containing index values (\"\")")
        DAGR_POPTS_GET(int, prefix, generate_original_ids,
            "add original row ids into the output. default off.")
        DAGR_POPTS_MULTI_GET(std::vector<std::string>, prefix, metadata_column_names,
             "names of the columns to copy directly into metadata")
        DAGR_POPTS_MULTI_GET(std::vector<std::string>, prefix, metadata_column_keys,
             "names of the metadata keys to create from the named columns")
        ;

    global_opts.add(opts);
}

// --------------------------------------------------------------------------
void dagr_table_reader::set_properties(const string &prefix, variables_map &opts)
{
    DAGR_POPTS_SET(opts, string, prefix, file_name)
    DAGR_POPTS_SET(opts, string, prefix, index_column)
    DAGR_POPTS_SET(opts, int, prefix, generate_original_ids)
    DAGR_POPTS_SET(opts, std::vector<std::string>, prefix, metadata_column_names)
    DAGR_POPTS_SET(opts, std::vector<std::string>, prefix, metadata_column_keys)
}
#endif

// --------------------------------------------------------------------------
void dagr_table_reader::set_modified()
{
    // clear cached metadata before forwarding on to
    // the base class.
    this->clear_cached_metadata();
    dagr_algorithm::set_modified();
}

// --------------------------------------------------------------------------
void dagr_table_reader::clear_cached_metadata()
{
    this->internals->clear();
}

// --------------------------------------------------------------------------
dagr_metadata dagr_table_reader::get_output_metadata(unsigned int port,
    const std::vector<dagr_metadata> &input_md)
{
#ifdef DAGR_DEBUG
    cerr << dagr_parallel_id()
        << "dagr_cf_reader::get_output_metadata" << endl;
#endif
    (void)port;
    (void)input_md;

    // if result is cached use that
    if (this->internals->table)
        return this->internals->metadata;

    // read the data
    bool distribute = !this->index_column.empty();

    this->internals->table =
        dagr_table_reader::dagr_table_reader_internals::read_table(
            this->file_name, distribute);

    // when no index column is specified  act like a serial reader
    if (!this->internals->table || !distribute)
        return dagr_metadata();

    // build the data structures for random access
    p_dagr_variant_array index =
        this->internals->table->get_column(this->index_column);

    if (!index)
    {
        this->clear_cached_metadata();
        DAGR_ERROR("Table is missing the index array \""
            << this->index_column << "\"")
        return dagr_metadata();
    }

    TEMPLATE_DISPATCH_I(dagr_variant_array_impl,
        index.get(),

        NT *pindex = dynamic_cast<TT*>(index.get())->get();

        dagr_coordinate_util::get_table_offsets(pindex,
            this->internals->table->get_number_of_rows(),
            this->internals->number_of_steps, this->internals->step_counts,
            this->internals->step_offsets, this->internals->step_ids);
        )

    // must have at least one time step
    if (this->internals->number_of_steps < 1)
    {
        this->clear_cached_metadata();
        DAGR_ERROR("Invalid index \"" << this->index_column << "\"")
        return dagr_metadata();
    }

    // report about the number of steps, this is all that
    // is needed to run in parallel over time steps.
    dagr_metadata md;
    md.insert("number_of_time_steps", this->internals->number_of_steps);

    // optionally pass columns directly into metadata
    size_t n_metadata_columns = this->metadata_column_names.size();
    if (n_metadata_columns)
    {
        for (size_t i = 0; i < n_metadata_columns; ++i)
        {
            std::string md_col_name = this->metadata_column_names[i];
            p_dagr_variant_array md_col = this->internals->table->get_column(md_col_name);
            if (!md_col)
            {
                DAGR_ERROR("metadata column \"" << md_col_name << "\" not found")
                continue;
            }
            md.insert(this->metadata_column_keys[i], md_col);
        }
    }

    // cache it
    this->internals->metadata = md;

    return md;
}


// --------------------------------------------------------------------------
const_p_dagr_dataset dagr_table_reader::execute(unsigned int port,
    const std::vector<const_p_dagr_dataset> &input_data,
    const dagr_metadata &request)
{
    (void)port;
    (void)input_data;

#if defined(DAGR_HAS_MPI)
    int rank = 0;
    int init = 0;
    MPI_Initialized(&init);
    if (init)
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if ((rank == 0) && !this->internals->table)
    {
        DAGR_ERROR("Failed to read data")
        return nullptr;
    }
#endif

    bool distribute = !this->index_column.empty();

    // not running in subsetting/parallel mode retrun
    // the complete table, it is empty off rank 0
    if (!distribute)
        return this->internals->table;

    // subset the table, pull out only rows for the requested step
    unsigned long step = 0;
    request.get("time_step", step);

    p_dagr_table out_table = dagr_table::New();
    out_table->copy_structure(this->internals->table);
    out_table->copy_metadata(this->internals->table);

    int ncols = out_table->get_number_of_columns();
    unsigned long nrows = this->internals->step_counts[step];
    unsigned long first_row = this->internals->step_offsets[step];

    for (int j = 0; j < ncols; ++j)
    {
        p_dagr_variant_array in_col =
            this->internals->table->get_column(j);

        p_dagr_variant_array out_col =
            out_table->get_column(j);

        out_col->resize(nrows);

        TEMPLATE_DISPATCH(dagr_variant_array_impl,
            out_col.get(),
            NT *pin_col = static_cast<TT*>(in_col.get())->get();
            NT *pout_col = static_cast<TT*>(out_col.get())->get();
            memcpy(pout_col, pin_col+first_row, nrows*sizeof(NT));
            )
    }

    if (this->generate_original_ids)
    {
        p_dagr_unsigned_long_array ids = dagr_unsigned_long_array::New(nrows);
        unsigned long *pids = ids->get();
        for (unsigned long i = 0, q = first_row; i < nrows; ++i, ++q)
            pids[i] = q;
        out_table->append_column("original_ids", ids);
    }

    return out_table;
}
