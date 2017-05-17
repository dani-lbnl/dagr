#include "dagr_table.h"
#include "dagr_metadata.h"
#include "dagr_programmable_algorithm.h"
#include "dagr_table_writer.h"
#include "dagr_time_step_executive.h"
#include "dagr_test_util.h"
#include "dagr_system_interface.h"

#include <iostream>
using namespace std;

struct report
{
    dagr_metadata operator()
        (unsigned int, const std::vector<dagr_metadata> &)
    {
        dagr_metadata md;
        md.insert("number_of_time_steps", long(4));
        return md;
    }
};

struct execute
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

int main(int, char **)
{
    dagr_system_interface::set_stack_trace_on_error();

    p_dagr_programmable_algorithm s = dagr_programmable_algorithm::New();
    s->set_number_of_input_connections(0);
    s->set_number_of_output_ports(1);
    s->set_report_callback(report());
    s->set_execute_callback(execute());

    // Write some .csv files.
    {
      p_dagr_table_writer w = dagr_table_writer::New();
      w->set_input_connection(s->get_output_port());
      w->set_executive(dagr_time_step_executive::New());
      w->set_file_name("table_writer_test_%t%.csv");

      w->update();
    }

    // Write some binary files.
    {
      p_dagr_table_writer w = dagr_table_writer::New();
      w->set_input_connection(s->get_output_port());
      w->set_executive(dagr_time_step_executive::New());
      w->set_file_name("table_writer_test_%t%.bin");

      w->update();
    }

    return 0;
}
