#include "dagr_config.h"
#include "dagr_cf_reader.h"
#include "dagr_dataset_diff.h"
#include "dagr_descriptive_statistics.h"
#include "dagr_file_util.h"
#include "dagr_table_reader.h"
#include "dagr_programmable_reduce.h"
#include "dagr_table_sort.h"
#include "dagr_table_writer.h"
#include "dagr_test_util.h"
#include "dagr_mpi_manager.h"
#include "dagr_system_interface.h"
#include "dagr_table.h"

#include <vector>
#include <string>
#include <iostream>
using namespace std;

#if defined(DAGR_HAS_MPI)
#include <mpi.h>
#endif

struct reduce_callback
{
    p_dagr_dataset operator()(const const_p_dagr_dataset &in_data_0,
        const const_p_dagr_dataset &in_data_1)
    {
        const_p_dagr_table table_0 =
            std::dynamic_pointer_cast<const dagr_table>(in_data_0);

        const_p_dagr_table table_1 =
            std::dynamic_pointer_cast<const dagr_table>(in_data_1);

        p_dagr_table table_2 = nullptr;

        if (table_0 && table_1)
        {
            table_2 = std::dynamic_pointer_cast
                <dagr_table>(table_0->new_copy());

            table_2->concatenate_rows(table_1);
        }
        else if (table_0)
        {
            table_2 = std::dynamic_pointer_cast
                <dagr_table>(table_0->new_copy());
        }
        else if (table_1)
        {
            table_2 = std::dynamic_pointer_cast
                <dagr_table>(table_1->new_copy());
        }

        return table_2;
    }
};


int main(int argc, char **argv)
{
    dagr_mpi_manager mpi_man(argc, argv);
    int rank = mpi_man.get_comm_rank();
    int nranks = mpi_man.get_comm_size();

    dagr_system_interface::set_stack_trace_on_error();

    // parse command line
    string regex;
    string baseline;
    int have_baseline = 0;
    long first_step = 0;
    long last_step = -1;
    unsigned int n_threads = 1;
    vector<string> arrays;
    if (rank == 0)
    {
        if (argc < 3)
        {
            cerr << endl << "Usage error:" << endl
                << "test_map_descriptive_statistics [input regex] [test baseline] [first step = 0] "
                << "[last step = -1] [num threads = 1] [array 0 =] ... [array n =]"
                << endl << endl;
            return -1;
        }
        regex = argv[1];
        baseline = argv[2];
        if (dagr_file_util::file_exists(baseline.c_str()))
            have_baseline = 1;
        if (argc > 3)
            first_step = atoi(argv[3]);
        if (argc > 4)
            last_step = atoi(argv[4]);
        if (argc > 5)
            n_threads = atoi(argv[5]);
        for (int i = 6; i < argc; ++i)
            arrays.push_back(argv[i]);

        cerr << "Testing with " << nranks << " MPI ranks each with "
            << n_threads << " threads" << endl;
    }
    dagr_test_util::bcast(regex);
    dagr_test_util::bcast(baseline);
    dagr_test_util::bcast(have_baseline);
    dagr_test_util::bcast(first_step);
    dagr_test_util::bcast(last_step);
    dagr_test_util::bcast(n_threads);
    dagr_test_util::bcast(arrays);

    // create the pipeline
    p_dagr_cf_reader cf_reader = dagr_cf_reader::New();
    cf_reader->set_files_regex(regex);

    p_dagr_descriptive_statistics stats = dagr_descriptive_statistics::New();
    stats->set_input_connection(cf_reader->get_output_port());
    stats->set_dependent_variables(arrays);

    p_dagr_programmable_reduce map_reduce = dagr_programmable_reduce::New();
    map_reduce->set_input_connection(stats->get_output_port());
    map_reduce->set_first_step(first_step);
    map_reduce->set_last_step(last_step);
    map_reduce->set_thread_pool_size(n_threads);
    map_reduce->set_reduce_callback(reduce_callback());

    p_dagr_table_sort sort = dagr_table_sort::New();
    sort->set_input_connection(map_reduce->get_output_port());
    sort->set_index_column("step");

    if (have_baseline)
    {
        // run the test
        p_dagr_table_reader table_reader = dagr_table_reader::New();
        table_reader->set_file_name(baseline);

        p_dagr_dataset_diff diff = dagr_dataset_diff::New();
        diff->set_input_connection(0, table_reader->get_output_port());
        diff->set_input_connection(1, cal->get_output_port());
        diff->update();
    }
    else
    {
        // make a baseline
        if (rank == 0)
            cerr << "generating baseline image " << baseline << endl;
        p_dagr_table_writer table_writer = dagr_table_writer::New();
        table_writer->set_input_connection(cal->get_output_port());
        table_writer->set_file_name(baseline.c_str());
        table_writer->set_output_format_bin();
        table_writer->update();
    }

    return 0;
}
