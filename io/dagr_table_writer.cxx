#include "dagr_table_writer.h"

#include "dagr_table.h"
#include "dagr_database.h"
#include "dagr_metadata.h"
#include "dagr_binary_stream.h"
#include "dagr_file_util.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#if defined(DAGR_HAS_BOOST)
#include <boost/program_options.hpp>
#endif

#if defined(DAGR_HAS_MPI)
#include <mpi.h>
#endif

#if defined(DAGR_HAS_LIBXLSXWRITER)
#include <xlsxwriter.h>
#endif

using std::vector;
using std::string;
using std::ostringstream;
using std::ofstream;
using std::cerr;
using std::endl;

namespace internal
{
// ********************************************************************************
int write_csv(const_p_dagr_table table, const std::string &file_name)
{
    ofstream os(file_name.c_str());
    if (!os.good())
    {
        DAGR_ERROR("Failed to open \"" << file_name << "\" for writing")
        return -1;
    }

    table->to_stream(os);

    return 0;
}

// ********************************************************************************
int write_bin(const_p_dagr_table table, const std::string &file_name)
{
    // serialize the table to a binary representation
    dagr_binary_stream bs;
    table->to_stream(bs);

    if (dagr_file_util::write_stream(file_name.c_str(), "dagr_table", bs))
    {
        DAGR_ERROR("Failed to write \"" << file_name << "\"")
        return -1;
    }

    return 0;
}

#if defined(DAGR_HAS_LIBXLSXWRITER)
// ********************************************************************************
int write_xlsx(const_p_dagr_table table, lxw_worksheet *worksheet)
{
    // write column headers
    unsigned int n_cols = table->get_number_of_columns();
    for (unsigned int i = 0; i < n_cols; ++i)
        worksheet_write_string(worksheet, 0, i, table->get_column_name(i).c_str(), NULL);

    // write the columns
    unsigned long long n_rows = table->get_number_of_rows();
    for (unsigned long long j = 0; j < n_rows; ++j)
    {
        for (unsigned int i = 0; i < n_cols; ++i)
        {
            TEMPLATE_DISPATCH(const dagr_variant_array_impl,
                table->get_column(i).get(),
                const TT *a = dynamic_cast<const TT*>(table->get_column(i).get());
                NT v = NT();
                a->get(j, v);
                worksheet_write_number(worksheet, j+1, i, static_cast<double>(v), NULL);
                )
            else TEMPLATE_DISPATCH_CASE(const dagr_variant_array_impl,
                std::string,
                table->get_column(i).get(),
                const TT *a = dynamic_cast<const TT*>(table->get_column(i).get());
                NT v = NT();
                a->get(j, v);
                worksheet_write_string(worksheet, j+1, i, v.c_str(), NULL);
                )
        }
    }

    return 0;
}
#endif
};


// --------------------------------------------------------------------------
dagr_table_writer::dagr_table_writer()
    : file_name("table_%t%.bin"), output_format(format_auto)
{
    this->set_number_of_input_connections(1);
    this->set_number_of_output_ports(1);
}

// --------------------------------------------------------------------------
dagr_table_writer::~dagr_table_writer()
{}

#if defined(DAGR_HAS_BOOST)
// --------------------------------------------------------------------------
void dagr_table_writer::get_properties_description(
    const string &prefix, options_description &global_opts)
{
    options_description opts("Options for "
        + (prefix.empty()?"dagr_table_writer":prefix));

    opts.add_options()
        DAGR_POPTS_GET(string, prefix, file_name,
            "path/name of file to write")
        DAGR_POPTS_GET(int, prefix, output_format,
            "output file format enum, 0:csv, 1:bin, 2:xlsx, 3:auto."
            "if auto is used, format is deduced from file_name")
        ;

    global_opts.add(opts);
}

// --------------------------------------------------------------------------
void dagr_table_writer::set_properties(const string &prefix, variables_map &opts)
{
    DAGR_POPTS_SET(opts, string, prefix, file_name)
    DAGR_POPTS_SET(opts, bool, prefix, output_format)
}
#endif

// --------------------------------------------------------------------------
const_p_dagr_dataset dagr_table_writer::execute(
    unsigned int port,
    const std::vector<const_p_dagr_dataset> &input_data,
    const dagr_metadata &request)
{
    (void) port;

    // in parallel only rank 0 is required to have data
    int rank = 0;
#if defined(DAGR_HAS_MPI)
    int init = 0;
    MPI_Initialized(&init);
    if (init)
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif
    if (!input_data[0])
    {
        if (rank == 0)
        {
            DAGR_ERROR("empty input")
        }
        return nullptr;
    }

    string out_file = this->file_name;

    // replace time step
    unsigned long time_step = 0l;
    request.get("time_step", time_step);
    dagr_file_util::replace_timestep(out_file, time_step);

    // replace extension
    int fmt = this->output_format;
    if (fmt == format_auto)
    {
        if (out_file.rfind(".xlsx") != std::string::npos)
        {
            fmt = format_xlsx;
        }
        else if (out_file.rfind(".csv") != std::string::npos)
        {
            fmt = format_csv;
        }
        else if (out_file.rfind(".bin") != std::string::npos)
        {
            fmt = format_bin;
        }
        else
        {
            if (rank == 0)
            {
                DAGR_WARNING("Failed to determine extension from file name \""
                    << out_file << "\". Using bin format.")
            }
            fmt = format_bin;
        }
    }
    else
    {
        const char *ext;
        switch (fmt)
        {
            case format_bin:
                ext = "bin";
                break;
            case format_csv:
                ext = "csv";
                break;
            case format_xlsx:
                ext = "xlsx";
                break;
            default:
                DAGR_ERROR("Invalid output format")
                return nullptr;
        }
        dagr_file_util::replace_extension(out_file, ext);
    }

    // convert table to database
    const_p_dagr_table table
        = std::dynamic_pointer_cast<const dagr_table>(input_data[0]);

    const_p_dagr_database database;

    if (table)
    {
        p_dagr_database tmp = dagr_database::New();
        tmp->append_table("table 1",
            std::const_pointer_cast<dagr_table>(table));
        database = tmp;
    }
    else
    {
        database = std::dynamic_pointer_cast
            <const dagr_database>(input_data[0]);
        if (!database)
        {
            DAGR_ERROR("input must be a table or a database")
            return nullptr;
        }
    }

    // write based on format
    switch (fmt)
    {
        case format_csv:
        case format_bin:
            {
            unsigned int n = database->get_number_of_tables();
            for (unsigned int i = 0; i < n; ++i)
            {
                std::string name = database->get_table_name(i);
                std::string out_file_i = out_file;
                dagr_file_util::replace_identifier(out_file_i, name);
                const_p_dagr_table table = database->get_table(i);
                if (((fmt == format_csv) && internal::write_csv(table, out_file_i))
                  || ((fmt == format_bin) && internal::write_bin(table, out_file_i)))
                {
                    DAGR_ERROR("Failed to write table " << i << " \"" << name << "\"")
                    return nullptr;
                }
            }
            }
            break;
        case format_xlsx:
            {
#if defined(DAGR_HAS_LIBXLSXWRITER)
            // open the workbook
            lxw_workbook_options options;
            options.constant_memory = 1;

            lxw_workbook *workbook  =
                workbook_new_opt(out_file.c_str(), &options);

            if (!workbook)
            {
                DAGR_ERROR("xlsx failed to create workbook ")
            }

            unsigned int n = database->get_number_of_tables();
            for (unsigned int i = 0; i < n; ++i)
            {
                // add a sheet for the table
                std::string name = database->get_table_name(i);
                lxw_worksheet *worksheet =
                    workbook_add_worksheet(workbook, name.c_str());

                if (internal::write_xlsx(database->get_table(i), worksheet))
                {
                    DAGR_ERROR("Failed to write table " << i << " \"" << name << "\"")
                    return nullptr;
                }
            }

            // close the workbook
            workbook_close(workbook);
#else
            DAGR_ERROR("DAGR was not compiled with libxlsx support")
#endif
            }
            break;
        default:
            DAGR_ERROR("invalid output format")
    }

    // pass the output through
    p_dagr_dataset output = input_data[0]->new_instance();
    output->shallow_copy(std::const_pointer_cast<dagr_dataset>(input_data[0]));
    return output;
}
