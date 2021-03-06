#include "dagr_table_sort.h"

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

namespace internal
{
template<typename num_t>
class less
{
public:
    less() : m_data(nullptr) {}
    less(const num_t *data) : m_data(data) {}

    bool operator()(const size_t &l, const size_t &r)
    {
        return m_data[l] < m_data[r];
    }
private:
    const num_t *m_data;
};
};


// --------------------------------------------------------------------------
dagr_table_sort::dagr_table_sort() :
    index_column(""), index_column_id(0), stable_sort(0)
{
    this->set_number_of_input_connections(1);
    this->set_number_of_output_ports(1);
}

// --------------------------------------------------------------------------
dagr_table_sort::~dagr_table_sort()
{}

#if defined(DAGR_HAS_BOOST)
// --------------------------------------------------------------------------
void dagr_table_sort::get_properties_description(
    const string &prefix, options_description &global_opts)
{
    options_description opts("Options for "
        + (prefix.empty()?"dagr_table_sort":prefix));

    opts.add_options()
        DAGR_POPTS_GET(std::string, prefix, index_column,
            "name of the column to sort the table by")
        DAGR_POPTS_GET(int, prefix, index_column_id,
            "column number to sort the table by. can be used in "
            "place of an index_column name")
        DAGR_POPTS_GET(int, prefix, stable_sort,
            "if set a stable sort will be used")
        ;

    global_opts.add(opts);
}

// --------------------------------------------------------------------------
void dagr_table_sort::set_properties(
    const string &prefix, variables_map &opts)
{
    DAGR_POPTS_SET(opts, std::string, prefix, index_column)
    DAGR_POPTS_SET(opts, int, prefix, index_column_id)
    DAGR_POPTS_SET(opts, int, prefix, stable_sort)
}
#endif

// --------------------------------------------------------------------------
const_p_dagr_dataset dagr_table_sort::execute(
    unsigned int port,
    const std::vector<const_p_dagr_dataset> &input_data,
    const dagr_metadata &request)
{
#ifdef DAGR_DEBUG
    cerr << dagr_parallel_id() << "dagr_table_sort::execute" << endl;
#endif
    (void)port;
    (void)request;

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

    // get the array to sort by
    const_p_dagr_variant_array index_col;
    if (this->index_column.empty())
        index_col = in_table->get_column(this->index_column_id);
    else
        index_col = in_table->get_column(this->index_column);
    if (!index_col)
    {
        DAGR_ERROR("Failed to locate column to sort by \""
            <<  this->index_column << "\"")
        return nullptr;
    }

    // create the index
    unsigned long n_rows = index_col->size();
    unsigned long *index = static_cast<unsigned long*>(
        malloc(n_rows*sizeof(unsigned long)));
    for (unsigned long i = 0; i < n_rows; ++i)
        index[i] = i;
    TEMPLATE_DISPATCH(const dagr_variant_array_impl,
        index_col.get(),
        const NT *col = static_cast<TT*>(index_col.get())->get();
        if (this->stable_sort)
            std::stable_sort(index, index+n_rows, internal::less<NT>(col));
        else
            std::sort(index, index+n_rows, internal::less<NT>(col));
        )

    // transfer data and reorder
    p_dagr_table out_table = dagr_table::New();
    out_table->copy_metadata(in_table);
    out_table->copy_structure(in_table);
    unsigned int n_cols = out_table->get_number_of_columns();
    for (unsigned int j = 0; j < n_cols; ++j)
    {
        const_p_dagr_variant_array in_col = in_table->get_column(j);
        p_dagr_variant_array out_col = out_table->get_column(j);
        out_col->resize(n_rows);
        TEMPLATE_DISPATCH(dagr_variant_array_impl,
            out_col.get(),
            const NT *p_in_col = static_cast<const TT*>(in_col.get())->get();
            NT *p_out_col = static_cast<TT*>(out_col.get())->get();
            for (unsigned long i = 0; i < n_rows; ++i)
                p_out_col[i] = p_in_col[index[i]];
            )
    }

    free(index);

    return out_table;
}
