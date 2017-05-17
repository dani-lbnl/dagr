#include "dagr_table_region_mask.h"

#include "dagr_table.h"
#include "dagr_array_collection.h"
#include "dagr_variant_array.h"
#include "dagr_metadata.h"
#include "dagr_distance_function.h"
#include "dagr_geometry.h"
#include "dagr_geography.h"

#include <iostream>
#include <string>
#include <set>
#include <sstream>

#if defined(DAGR_HAS_BOOST)
#include <boost/program_options.hpp>
#endif
#if defined(DAGR_HAS_UDUNITS)
#include "calcalcs.h"
#endif
#if defined(DAGR_HAS_MPI)
#include <mpi.h>
#endif

using std::cerr;
using std::endl;

// --------------------------------------------------------------------------
dagr_table_region_mask::dagr_table_region_mask() :
    x_coordinate_column("lon"), y_coordinate_column("lat"),
    result_column("region_mask"), invert(0)
{
    this->set_number_of_input_connections(1);
    this->set_number_of_output_ports(1);
}

// --------------------------------------------------------------------------
dagr_table_region_mask::~dagr_table_region_mask()
{}

#if defined(DAGR_HAS_BOOST)
// --------------------------------------------------------------------------
void dagr_table_region_mask::get_properties_description(
    const std::string &prefix, options_description &global_opts)
{
    options_description opts("Options for "
        + (prefix.empty()?"dagr_table_region_mask":prefix));

    opts.add_options()
        DAGR_POPTS_GET(std::string, prefix, x_coordinate_column,
            "name of the column containing x cooridnates. default \"lon\"")
        DAGR_POPTS_GET(std::string, prefix, y_coordinate_column,
            "name of the column containing y cooridnates. default \"lat\"")
        DAGR_POPTS_GET(std::string, prefix, result_column,
            "name of the column to store the mask in. default \"region_mask\"")
        DAGR_POPTS_MULTI_GET(std::vector<unsigned long>, prefix, region_sizes,
            "the number of points in each region")
        DAGR_POPTS_MULTI_GET(std::vector<double>, prefix, region_x_coordinates,
            "list of x coordinates describing the regions.")
        DAGR_POPTS_MULTI_GET(std::vector<double>, prefix, region_y_coordinates,
            "list of y coordinates describing the regions")
        ;

    global_opts.add(opts);
}

// --------------------------------------------------------------------------
void dagr_table_region_mask::set_properties(
    const std::string &prefix, variables_map &opts)
{
    DAGR_POPTS_SET(opts, std::string, prefix, x_coordinate_column)
    DAGR_POPTS_SET(opts, std::string, prefix, y_coordinate_column)
    DAGR_POPTS_SET(opts, std::string, prefix, result_column)
    DAGR_POPTS_SET(opts, std::vector<unsigned long>, prefix, region_sizes)
    DAGR_POPTS_SET(opts, std::vector<double>, prefix, region_x_coordinates)
    DAGR_POPTS_SET(opts, std::vector<double>, prefix, region_y_coordinates)
}
#endif

// --------------------------------------------------------------------------
void dagr_table_region_mask::clear_regions()
{
    this->set_modified();
    this->region_sizes.clear();
    this->region_starts.clear();
    this->region_x_coordinates.clear();
    this->region_y_coordinates.clear();
}

// --------------------------------------------------------------------------
int dagr_table_region_mask::load_cyclone_basin(const std::string &name)
{
    std::vector<std::string> tmps;
    std::vector<int> tmpi;
    if (dagr_geography::get_cyclone_basin(name, this->region_sizes,
        this->region_starts, this->region_x_coordinates,
        this->region_y_coordinates, tmpi, tmps, tmps))
    {
        DAGR_ERROR("invalid basin name \"" << name << "\"")
        return -1;
    }
    return 0;
}

// --------------------------------------------------------------------------
const_p_dagr_dataset dagr_table_region_mask::execute(
    unsigned int port, const std::vector<const_p_dagr_dataset> &input_data,
    const dagr_metadata &request)
{
#ifdef DAGR_DEBUG
    cerr << dagr_parallel_id() << "dagr_table_region_mask::execute" << endl;
#endif
    (void)port;
    (void)request;

    // get the input table
    const_p_dagr_table in_table
        = std::dynamic_pointer_cast<const dagr_table>(input_data[0]);

    // only rank 0 is required to have data
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
            DAGR_ERROR("Input is empty or not a table")
        }
        return nullptr;
    }
    unsigned long n_rows = in_table->get_number_of_rows();

    unsigned long n_regions = this->region_sizes.size();
    if (!n_regions)
    {
        DAGR_ERROR("no regions to filter by specified")
        return nullptr;
    }

    // grab the event coorinates
    const_p_dagr_variant_array x =
        in_table->get_column(this->x_coordinate_column);
    if (!x)
    {
        DAGR_ERROR("x coordinate column \"" << this->x_coordinate_column
            << "\" is not in the table")
        return nullptr;
    }

    const_p_dagr_variant_array y =
        in_table->get_column(this->y_coordinate_column);
    if (!y)
    {
        DAGR_ERROR("y coordinate column \"" << this->y_coordinate_column
            << "\" is not in the table")
        return nullptr;
    }

    // generate starts array if it was not provided
    std::vector<unsigned long> rstarts(this->region_starts);
    if (rstarts.empty())
    {
        rstarts.reserve(n_regions);
        rstarts.push_back(0);
        for (unsigned int  i = 0; i < n_regions; ++i)
            rstarts.push_back(rstarts[i] + this->region_sizes[i]);
    }

    // allocate space for name and id
    short T = this->invert ? 0 : 1;
    short F = this->invert ? 1 : 0;

    p_dagr_short_array mask = dagr_short_array::New(n_rows, F);
    short *pmask = mask->get();
    unsigned int nhit = 0;

    TEMPLATE_DISPATCH_FP(const dagr_variant_array_impl,
        x.get(),

        const NT *px = static_cast<const TT*>(x.get())->get();
        const NT *py = static_cast<const TT*>(y.get())->get();

        for (unsigned long i = 0; i < n_rows; ++i)
        {
            double ptx = px[i];
            double pty = py[i];
            for (unsigned long j = 0; j < n_regions; ++j)
            {
                double *polyx = this->region_x_coordinates.data() + rstarts[j];
                double *polyy = this->region_y_coordinates.data() + rstarts[j];
                if (dagr_geometry::point_in_poly(ptx, pty,
                    polyx, polyy, this->region_sizes[j]))
                {
                    pmask[i] = T;
                    nhit += 1;
                    break;
                }
            }
        }
        )

    if (!nhit)
    {
        DAGR_WARNING("No rows satisfy the criteria")
    }

    // build the output.
    p_dagr_table out_table = dagr_table::New();

    out_table->shallow_copy(
        std::const_pointer_cast<dagr_table>(in_table));

    out_table->append_column(this->result_column, mask);

    return out_table;
}
