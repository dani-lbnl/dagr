#include "dagr_config.h"
#include "dagr_table_reader.h"
#include "dagr_table_to_stream.h"
#include "dagr_evaluate_expression.h"
#include "dagr_table_writer.h"
#include "dagr_dataset_diff.h"
#include "dagr_file_util.h"
#include "dagr_system_interface.h"

#include <vector>
#include <string>
#include <iostream>

using std::cerr;
using std::endl;

int main(int argc, char **argv)
{
    dagr_system_interface::set_stack_trace_on_error();

    // parse command line
    if (argc < 4)
    {
        cerr << endl << "Usage error:" << endl
            << "test_evaluate_expression_table [input table] [test baseline] [expression]"
            << endl << endl;
        return -1;
    }
    std::string input_table = argv[1];
    std::string baseline_table = argv[2];
    std::string expression = argv[3];

    // create the pipeline
    p_dagr_table_reader input_reader = dagr_table_reader::New();
    input_reader->set_file_name(input_table);

    p_dagr_evaluate_expression eval_expr = dagr_evaluate_expression::New();
    eval_expr->set_input_connection(input_reader->get_output_port());
    eval_expr->set_expression(expression);
    eval_expr->set_result_variable("test_tracks");

    if (dagr_file_util::file_exists(baseline_table.c_str()))
    {
        // run the test
        p_dagr_table_reader baseline_table_reader = dagr_table_reader::New();
        baseline_table_reader->set_file_name(baseline_table);

        p_dagr_dataset_diff diff = dagr_dataset_diff::New();
        diff->set_input_connection(0, baseline_table_reader->get_output_port());
        diff->set_input_connection(1, eval_expr->get_output_port());
        diff->update();
    }
    else
    {
        // make a baseline
        cerr << "generating baseline image " << baseline_table << endl;

        /*p_dagr_table_to_stream dump_table = dagr_table_to_stream::New();
        dump_table->set_input_connection(eval_expr->get_output_port());*/

        p_dagr_table_writer table_writer = dagr_table_writer::New();
        //table_writer->set_input_connection(dump_table->get_output_port());
        table_writer->set_input_connection(eval_expr->get_output_port());
        table_writer->set_file_name(baseline_table);
        table_writer->update();
        return -1;
    }

    return 0;
}
