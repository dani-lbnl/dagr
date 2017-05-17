#include "dagr_config.h"
#include "dagr_programmable_algorithm.h"
#include "dagr_table_reader.h"
#include "dagr_table_writer.h"
#include "dagr_dataset_diff.h"
#include "dagr_time_step_executive.h"
#include "dagr_table.h"
#include "dagr_test_util.h"
#include "dagr_system_interface.h"

#include <vector>
#include <string>
#include <iostream>
#include <cmath>
using namespace std;

struct report
{
    long num_tables;

    report() : num_tables(0) {}
    explicit report(long n) : num_tables(n) {}

    dagr_metadata operator()
        (unsigned int, const std::vector<dagr_metadata> &)
    {
        dagr_metadata md;
        md.insert("number_of_time_steps", num_tables);
        return md;
    }
};

// This test should be executed after test_table_writer.cpp, which writes the 
// files needed to test the dataset differ.
struct execute_create_test_table
{
    const_p_dagr_dataset operator()
        (unsigned int, const std::vector<const_p_dagr_dataset> &,
        const dagr_metadata &req)
    {
        long step;
        if (req.get("time_step", step))
        {
            cerr << "request is missing \"time_step\"" << endl;
            return nullptr;
        }

        return dagr_test_util::create_test_table(step);
    }
};

void write_test_tables(const string& file_name, long num_tables)
{
    p_dagr_programmable_algorithm s = dagr_programmable_algorithm::New();
    s->set_number_of_input_connections(0);
    s->set_number_of_output_ports(1);
    s->set_report_callback(report(num_tables));
    s->set_execute_callback(execute_create_test_table());

    p_dagr_table_writer w = dagr_table_writer::New();
    w->set_input_connection(s->get_output_port());
    w->set_executive(dagr_time_step_executive::New());
    w->set_file_name(file_name);
    w->set_output_format_bin();

    w->update();
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    dagr_system_interface::set_stack_trace_on_error();

    // Write 2 test files.
    write_test_tables("dataset_diff_test_%t%.%e%", 2);

    {
      // Set up a pair of readers that read a single file. The resulting
      // datasets should be identical.
      p_dagr_table_reader r1 = dagr_table_reader::New();
      r1->set_file_name("dataset_diff_test_0.bin");

      p_dagr_table_reader r2 = dagr_table_reader::New();
      r2->set_file_name("dataset_diff_test_0.bin");

      // Set up the dataset diff algorithm to accept the two readers.
      p_dagr_dataset_diff diff = dagr_dataset_diff::New();
      diff->set_input_connection(0, r1->get_output_port());
      diff->set_input_connection(1, r2->get_output_port());

      diff->update();
    }

    // The following is a test of the dataset_diff algorithms ability to
    // correctly identify two different datasets. Our error reporting
    // needs to get a little more sophisticated before we take this on,
    // however, so this is getting commented out for the moment. -JNJ
    //{
    //    // Set up a pair of readers that read different files. The resulting
    //    // datasets should be different.
    //    p_dagr_table_reader r1 = dagr_table_reader::New();
    //    r1->set_file_name("dataset_diff_test_0.bin");
    //    p_dagr_table_reader r2 = dagr_table_reader::New();
    //    r2->set_file_name("dataset_diff_test_1.bin");
    //
    //    // Set up the dataset diff algorithm to accept the two readers.
    //    p_dagr_dataset_diff diff = dagr_dataset_diff::New();
    //    diff->set_input_connection(0, r1->get_output_port());
    //    diff->set_input_connection(1, r2->get_output_port());
    //
    //    diff->update();
    //}

    return 0;
}
