#include "dagr_cf_cartesian_mesh_writer.h"

#include "dagr_config.h"
#include "dagr_cartesian_mesh.h"
#include "dagr_array_collection.h"
#include "dagr_variant_array.h"
#incude "dagr_file_util.h"
#include "dagr_cf_util.h"


#if defined(DAGR_HAS_VTK) || defined(DAGR_HAS_PARAVIEW)
#include "cfRectilinearGrid.h"
#include "cfXMLRectilinearGridWriter.h"
#endif

#include <iostream>
#include <sstream>
#include <cstring>
#include <cerrno>
#include <string>

#if defined(DAGR_HAS_BOOST)
#include <boost/program_options.hpp>
#endif

class dagr_cf_writer_internals
{
public:
    static
    int write(const std::string &file_name,
        const_p_dagr_variant_array &x, const_p_dagr_variant_array &y,
        const_p_dagr_variant_array &z, const_p_dagr_double_array &t,
        const std::string &x_variable, const std::string &y_variable,
        const std::string &z_variable, const std::string &t_variable,
        std::vector<const_p_dagr_array_collection> &point_arrays,
        const dagr_metadata &point_array_attributes);
};

// --------------------------------------------------------------------------
int dagr_cf_writer_internals::write(const std::string &file_name,
    const_p_dagr_variant_array &x, const_p_dagr_variant_array &y,
    const_p_dagr_variant_array &z, const_p_dagr_double_array &t,
    const std::string &x_variable, const std::string &y_variable,
    const std::string &z_variable, const std::string &t_variable,
    std::vector<const_p_dagr_array_collection> &point_arrays,
    const dagr_metadata &point_array_attributes)
{
    // create the output file
    int ierr = NC_NOERR;
    int fh = -1;
    if ((ierr = nc_create(file_name.c_str(), NC_CLOBBER, &fh)))
    {
        DAGR_ERROR("failed to create file \"" << file_name << "\". "
            << nc_strerror(ierr));
        return -1;
    }
    dagr_netcdf_util::netcdf_handle nfho(fh);

    // at least one of the coordinates must have data
    // for the others construct a length one array containing
    // zeros
    const_p_dagr_variant_array coord_array
        = x ? x : y ? y : z ? z : t ? t : nullptr;

    if (!coord_array)
    {
        DAGR_ERROR("invalid coordinates")
        return -1;
    }

    if (!x)
    {
        x = coord_array.new_instance(1);
        x->initialize();
    }

    if (!y)
    {
        y = coord_array.new_instance(1);
        y->initialize();
    }

    if (!z)
    {
        z = coord_array.new_instance(1);
        z->initialize();
    }

    if (!t)
        t = dagr_double_array::New(1, 0.0);

    const_p_dagr_variant_array coord_arrays[4] = {t, z, y, x};

    // get the coordinate array names
    std::string axes_vars[4];
    if (t_variable.empty())
        axes_vars[0] = "time";
    if (z_variable.empty())
        axes_vars[1] = "plev";
    if (y_variable.empty())
        axes_vars[2] = "lat";
    if (x_variable.empty())
        axes_vars[3] = "lon";

    // define dimensions for each coordinate axis
    int dim_ids[4];
    if (((ierr = nc_def_dim(fh, axes_vars[0], coord_arrays[0]->size(), dim_ids)) != NC_NOERR)
        || ((ierr = nc_def_dim(fh, axes_vars[1], coord_arrays[1]->size(), dim_ids+1)) != NC_NOERR)
        || ((ierr = nc_def_dim(fh, axes_vars[2], coord_arrays[2]->size(), dim_ids+2)) != NC_NOERR)
        || ((ierr = nc_def_dim(fh, axes_vars[3], coord_arrays[3]->size(), dim_ids+3)) != NC_NOERR))
    {
        DAGR_ERROR("failed to define dimensions for coordinate axes. "
            << nc_strerror(ierr))
        return -1;
    }

    // define variables for each coordinate array
    int var_ids[4];
    TEMPLATE_DISPATCH(const dagr_variant_array_impl,
        coord_array.get(),
        int coord_type = dagr_netcdf_util::netcdf_tt<NT>::type_code;
        if (((ierr = nc_def_var(fh, axes_vars[0].c_str(), NC_DOUBLE, 1, dim_ids, var_ids)) != NC_NOERR)
            || ((ierr = nc_def_var(fh, axes_vars[1].c_str(), coord_type, 1, dim_ids+1, var_ids+1)) != NC_NOERR)
            || ((ierr = nc_def_var(fh, axes_vars[2].c_str(), coord_type, 1, dim_ids+2, var_ids+2)) != NC_NOERR)
            || ((ierr = nc_def_var(fh, axes_vars[3].c_str(), coord_type, 1, dim_ids+3, var_ids+3)) != NC_NOERR))
        {
            DAGR_ERROR("failed to define variables for coordinate axes. "
                << nc_strerror(ierr))
            return -1;
        }
        )

    // if the coordinates have attributes copy them
    dagr_metadata atr_md;
    if (md.get("attributes", atr_md))
    {
        DAGR_ERROR("metadata is missing attributes")
        return -1;
    }

    for (int i = 0; i < 4; ++i)
    {
        dagr_metadata att_md;
        if (!atr_md.get(axes_vars[i], att_md))
        {
            unsigned long natt_md = att_md.size();
            for (unsigned long j = 0; j < natt_md; ++j)
            {
                std::string att_name;
                std::string att_val;
                if (att_md.get_name(j, att_name) || att_md.get(att_name, att_val))
                {
                    DAGR_ERROR("failed to get attribute " << i << " name, value")
                }
                else if ((ierr = nc_put_att_text(fh, var_ids[i],
                     att_name.c_str(), att_val.size()+1, att_val.c_str())) != NC_NOERR)
                {
                    DAGR_ERROR("failed to put attribute \"" << att_name << "\"")
                }
            }
        }
    }

    // define variables for each point array
    unsigned int n_point_arrays = 0;
    vector<int> point_array_ids;
    if (point_arrays.size())
    {
        unsigned int n_point_arrays = point_arrays[0]->size();
        point_array_ids.resize(n_point_arrays, -1);
        for (unsigned int i = 0; i < n_point_arrays; ++i)
        {
            std::string name = point_arrays[0]->get_name(i);
            const_p_dagr_variant_array array = point_arrays[0]->get(i);

            // define a variable for the array
            TEMPLATE_DISPATCH(const dagr_variant_array_impl,
                array.get(),
                int coord_type = dagr_netcdf_util::netcdf_tt<NT>::type_code;
                if ((ierr = nc_def_var(fh, name.c_str(), coord_type,
                        4, dim_ids, &point_array_ids[i])) != NC_NOERR)
                {
                    DAGR_ERROR("failed to define variable for point array \"" << name << "\". "
                        << nc_strerror(ierr))
                    return -1;
                }
                )

            // copy the attributes if any exist.
            dagr_metadata att_md;
            if (!atr_md.get(name, att_md))
            {
                unsigned long natts = att_md.size();
                for (unsigned long j = 0; j < natts; ++j)
                {
                    std::string att_name = att_md.get_name(j);
                    std::string att_val;
                    if (att_md.get(att_name, att_val))
                    {
                        DAGR_ERROR("failed to get attribute value for \"" << att_name << "\"")
                    }
                    else if ((ierr = nc_put_att_text(fh, point_array_ids[i],
                         att_name.c_str(), att_val.size()+1, att_val.c_str())) != NC_NOERR)
                    {
                        DAGR_ERROR("failed to put attribute \"" << att_name << "\"")
                    }
                }
            }
        }
    }

    // end metadata definition phase
    ierr = nc_enddef(ncid);

    // write the coordinate arrays
    for (int i = 0; i < 4; ++i)
    {
        size_t start = 0;
        size_t count = coord_arrays[i]->size();
        TEMPLATE_DISPATCH(const dagr_variant_array_impl,
            coord_arrays[i].get(),
            const NT *pa = static_cast<TT*>(coord_arrays[i].get())->get();
            if ((ierr = nc_put_vara(fh, var_ids[i], &start, &count, pa)) != NC_NOERR)
            {
                DAGR_ERROR("failed to put coordinate array \"" << axes_vars[i] << "\". "
                    << nc_strerror(ierr))
                return -1;
            }
            )
    }

    // write point arrays
    if (point_arrays.size())
    {
        unsigned int n_steps = t->size();
        size_t starts[4] = {0, 0, 0, 0};
        size_t counts[4] = {1, z->size(), y->size(), x->size()};
        for (unsigned int q = 0; q < n_steps; ++q)
        {
            for (unsigned int i = 0; i < n_point_arrays; ++i)
            {
                std::string name = point_arrays[q]->get_name(i);
                const_p_dagr_variant_array array = point_arrays[q]->get(i);

                TEMPLATE_DISPATCH(const dagr_variant_array_impl,
                    array[i].get(),
                    const NT *pa = static_cast<TT*>(array[i].get())->get();
                    if ((ierr = nc_put_vara(fh, var_ids[i], &start, &count, pa)) != NC_NOERR)
                    {
                        DAGR_ERROR("failed to put coordinate array \"" << axes_vars[i] << "\". "
                            << nc_strerror(ierr))
                        return -1;
                    }
                    )
            }
            for (int i = 0; i < 4; ++i)
                starts[i] += counts[i];
        }
    }

    return 0;
}
};

// --------------------------------------------------------------------------
dagr_cf_cartesian_mesh_writer::dagr_cf_cartesian_mesh_writer()
    : file_name(""), steps_per_file(1)
{
    this->set_number_of_input_connections(1);
    this->set_number_of_output_ports(1);
}

// --------------------------------------------------------------------------
dagr_cf_cartesian_mesh_writer::~dagr_cf_cartesian_mesh_writer()
{}

#if defined(DAGR_HAS_BOOST)
// --------------------------------------------------------------------------
void dagr_cf_cartesian_mesh_writer::get_properties_description(
    const std::string &prefix, options_description &global_opts)
{
    options_description opts("Options for "
        + (prefix.empty()?"dagr_cf_cartesian_mesh_writer":prefix));

    opts.add_options()
        DAGR_POPTS_GET(std::string, prefix, file_name,
            "path/name to write series to")
        DAGR_POPTS_GET(unsigned int, prefix, steps_per_file,
            "set the number of time steps to write per file(1)")
        ;

    global_opts.add(opts);
}

// --------------------------------------------------------------------------
void dagr_cf_cartesian_mesh_writer::set_properties(
    const std::string &prefix, variables_map &opts)
{
    DAGR_POPTS_SET(opts, std::string, prefix, file_name)
    DAGR_POPTS_SET(opts, unsigned int, prefix, steps_per_file)
}
#endif

// --------------------------------------------------------------------------
dagr_metadata dagr_cf_cartesian_mesh_writer::get_output_metadata(
    unsigned int port, const std::vector<dagr_metadata> &input_md)
{
#ifdef DAGR_DEBUG
    cerr << dagr_parallel_id()
        << "dagr_cf_reader::get_output_metadata" << endl;
#endif
    (void)port;

    // TODO -- use dagr specific communicator
    int rank = 0;
    int n_ranks = 1;
#if defined(DAGR_HAS_MPI)
    int is_init = 0;
    MPI_Initialized(&is_init);
    if (is_init)
    {
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);
    }
#endif

    // locate available times
    long in_steps;
    if (input_md[0].get("number_of_time_steps", in_steps))
    {
        DAGR_ERROR("metadata is missing \"number_of_time_steps\"")
        return dagr_metadata();
    }

    // estimate the number of files
    long n_files = in_steps/this->steps_per_file;

    // this effectively tells the down stream executive to
    // parallelize over files rather than time steps. in the
    // request phase we'll need to intercept the request and
    // convert the file id back to a set of time steps.
    dagr_metadata out_md(input_md[0]);
    out_md.set("number_of_time_steps", n_files);
    return out_md;
}

// --------------------------------------------------------------------------
std::vector<dagr_metadata> dagr_cf_cartesian_mesh_writer::get_upstream_request(
    unsigned int port, const std::vector<dagr_metadata> &input_md,
    const dagr_metadata &request)
{
    vector<dagr_metadata> up_reqs;

    // TODO -- use dagr specific communicator
    int rank = 0;
    int n_ranks = 1;
#if defined(DAGR_HAS_MPI)
    int is_init = 0;
    MPI_Initialized(&is_init);
    if (is_init)
    {
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);
    }
#endif

    // locate available times
    long in_steps;
    if (input_md[0].get("number_of_time_steps", in_steps))
    {
        DAGR_ERROR("metadata is missing \"number_of_time_steps\"")
        return up_reqs;
    }

    // estimate the number of files
    long n_files = in_steps/this->steps_per_file;
    long n_large_files = in_steps%this->steps_per_file;

    // get the file id requested of us
    long file_id;
    if (request.get("time_step", file_id))
    {
        DAGR_ERROR("failed to determiine requested file id")
        return up_reqs;
    }

    long first_step = file_id < n_large_files ?
        file_id*(this->steps_per_file + 1) :
        file_id*this->steps_per_file + n_large_files;

    long n_steps = this->steps_per_file +
        file_id < n_large_files ? 1 : 0;

    long last_step = first_step + n_steps;

    // initialize the set of requests with the down stream
    // request
    up_reqs.resize(n_steps, request);

    // fix the time step
    for (size_t i = first_step; i <= last_step; ++i)
        up_reqs.back().set("time_step", i);

    return up_reqs;
}

// --------------------------------------------------------------------------
const_p_dagr_dataset dagr_cf_cartesian_mesh_writer::execute(
    unsigned int port, const std::vector<const_p_dagr_dataset> &input_data,
    const dagr_metadata &request)
{
    (void)port;

    // TODO -- use dagr specific communicator
    int rank = 0;
    int n_ranks = 1;
#if defined(DAGR_HAS_MPI)
    int is_init = 0;
    MPI_Initialized(&is_init);
    if (is_init)
    {
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);
    }
#endif

    // locate available times
    long in_steps;
    if (input_md[0].get("number_of_time_steps", in_steps))
    {
        DAGR_ERROR("metadata is missing \"number_of_time_steps\"")
        return up_reqs;
    }

    // estimate the number of files
    long n_files = in_steps/this->steps_per_file;
    long n_large_files = in_steps%this->steps_per_file;

    // get the file id requested of us
    long file_id;
    if (request.get("time_step", file_id))
    {
        DAGR_ERROR("failed to determiine requested file id")
        return up_reqs;
    }

    long first_step = file_id < n_large_files ?
        file_id*(this->steps_per_file + 1) :
        file_id*this->steps_per_file + n_large_files;

    long n_steps = this->steps_per_file +
        file_id < n_large_files ? 1 : 0;

    long last_step = first_step + n_steps;

    // check that we have the requisite data
    if (n_steps != input_data.size())
    {
        DAGR_ERROR("requested " << n_steps << " received "
            << input_data.size())
        return nullptr;
    }

    // get coordinate axes arrays
    const_p_dagr_cartesian_mesh in_mesh
        = std::dynamic_pointer_cast<const dagr_cartesian_mesh>(
                input_data[0]);

    if (!in_mesh)
    {
        if (rank == 0)
        {
            DAGR_ERROR("input mesh " << i
                << " is empty input or not a cartesian mesh")
        }
        return nullptr;
    }

    const_p_dagr_variant_array x = in_mesh->get_x_coordinate();
    const_p_dagr_variant_array y = in_mesh->get_y_coordinate();
    const_p_dagr_variant_array z = in_mesh->get_z_coordinate();

    // construct the time axis
    p_dagr_double_array t = dagr_double_array::New(n_steps);
    double *pt = t->get();

    // get point centered arrays
    std::vector<const_p_dagr_array_collection> point_arrays(n_steps);

    for (long i = 0; i < n_steps; ++i)
    {
        const_p_dagr_cartesian_mesh in_mesh
            = std::dynamic_pointer_cast<const dagr_cartesian_mesh>(
                input_data[i]);

        if (!in_mesh)
        {
            if (rank == 0)
            {
                DAGR_ERROR("input mesh " << i
                    << " is empty input or not a cartesian mesh")
            }
            return nullptr;
        }

        if (in_mesh->get_time(*(pt+i)))
        {
            DAGR_ERROR("input mesh " << i << " is missing time")
            return nullptr;
        }

        point_arrays[i] = in_mesh->get_point_arrays();
    }

    // construct the file name
    std::string out_file = this->file_name;
    dagr_file_util::replace_timestep(out_file, file_id);
    dagr_file_util::replace_extension(out_file, "nc");

    // write the data
    dagr_cf_writer_internals::

int write(const std::string &file_name, dagr_metadata &md,
    const_p_dagr_variant_array &x, const_p_dagr_variant_array &y,
    const_p_dagr_variant_array &z, const_p_dagr_double_array &t,
    std::vector<const_p_dagr_array_collection> &point_arrays)


    return nullptr;
}
