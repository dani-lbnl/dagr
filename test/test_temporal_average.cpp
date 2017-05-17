#include "dagr_cf_reader.h"
#include "dagr_temporal_average.h"
#include "dagr_vtk_cartesian_mesh_writer.h"
#include "dagr_time_step_executive.h"
#include "dagr_system_interface.h"

#include <vector>
#include <string>
#include <iostream>
using namespace std;

// example use
// ./test/test_cf_reader ~/work/dagr/data/'cam5_1_amip_run2.cam2.h2.1991-10-.*' tmp 0 -1 PS

int main(int argc, char **argv)
{
    dagr_system_interface::set_stack_trace_on_error();

    if (argc < 7)
    {
        cerr << endl << "Usage error:" << endl
            << "test_cf_reader [input regex] [output] [first step] [last step] [filter width] [array] [array] ..." << endl
            << endl;
        return -1;
    }

    // parse command line
    string regex = argv[1];
    string output = argv[2];
    long first_step = atoi(argv[3]);
    long last_step = atoi(argv[4]);
    int filter_width = atoi(argv[5]);
    vector<string> arrays;
    arrays.push_back(argv[6]);
    for (int i = 7; i < argc; ++i)
        arrays.push_back(argv[i]);

    // create the cf reader
    p_dagr_cf_reader r = dagr_cf_reader::New();
    r->set_files_regex(regex);

    p_dagr_temporal_average a = dagr_temporal_average::New();
    a->set_filter_width(filter_width);
    a->set_filter_type(dagr_temporal_average::backward);
    a->set_input_connection(r->get_output_port());

    // create the vtk writer connected to the cf reader
    p_dagr_vtk_cartesian_mesh_writer w = dagr_vtk_cartesian_mesh_writer::New();
    w->set_file_name(output);
    w->set_input_connection(a->get_output_port());

    // set the executive on the writer to stream time steps
    p_dagr_time_step_executive exec = dagr_time_step_executive::New();
    exec->set_first_step(first_step);
    exec->set_last_step(last_step);
    exec->set_arrays(arrays);

    w->set_executive(exec);

    // run the pipeline
    w->update();

    return 0;
}
