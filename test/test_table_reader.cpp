#include "dagr_config.h"
#include "dagr_programmable_algorithm.h"
#include "dagr_table_writer.h"
#include "dagr_table_reader.h"
#include "dagr_table.h"
#include "dagr_dataset_diff.h"
#include "dagr_test_util.h"
#include "dagr_system_interface.h"

#include <vector>
#include <string>
#include <iostream>
#include <cmath>
using namespace std;

struct execute_create_test_table
{
    int table_id;

    execute_create_test_table() : table_id(dagr_test_util::base_table) {}
    execute_create_test_table(int tid) : table_id(tid) {}

    const_p_dagr_dataset operator()
        (unsigned int, const std::vector<const_p_dagr_dataset> &,
        const dagr_metadata &)
    { return dagr_test_util::create_test_table(0, table_id); }
};

int main(int, char **)
{
    dagr_system_interface::set_stack_trace_on_error();

    // Write a test table.
    p_dagr_programmable_algorithm s = dagr_programmable_algorithm::New();
    s->set_number_of_input_connections(0);
    s->set_number_of_output_ports(1);
    s->set_execute_callback(execute_create_test_table());

    p_dagr_table_writer w = dagr_table_writer::New();
    w->set_input_connection(s->get_output_port());
    w->set_file_name("table_reader_test.bin");

    w->update();

    // Set up reader to read it back in
    p_dagr_table_reader r = dagr_table_reader::New();
    r->set_file_name("table_reader_test.bin");

    // create the same table in memory
    s = dagr_programmable_algorithm::New();
    s->set_number_of_input_connections(0);
    s->set_number_of_output_ports(1);
    s->set_execute_callback(execute_create_test_table());

    // Set up the dataset diff algorithm
    p_dagr_dataset_diff diff = dagr_dataset_diff::New();
    diff->set_input_connection(0, s->get_output_port());
    diff->set_input_connection(1, r->get_output_port());

    // run the test
    diff->update();

    return 0;
}
