try:
    from mpi4py import *
    rank = MPI.COMM_WORLD.Get_rank()
    n_ranks = MPI.COMM_WORLD.Get_size()
except:
    rank = 0
    n_ranks = 1
from dagr import *
import numpy as np
import sys
import os

set_stack_trace_on_error()

if len(sys.argv) < 7:
    sys.stderr.write('test_map_reduce.py [dataset regex] ' \
        '[out file name] [first step] [last step] [n threads]' \
        '[array 1] .. [ array n]\n')
    sys.exit(-1)

data_regex = sys.argv[1]
out_file = sys.argv[2]
first_step = int(sys.argv[3])
last_step = int(sys.argv[4])
n_threads = int(sys.argv[5])
var_names = sys.argv[6:]

def get_request_callback(var_names):
    def request(port, md_in, req_in):
        req = dagr_metadata(req_in)
        req['arrays'] = var_names
        return [req]
    return request

def get_execute_callback(rank, var_names):
    def execute(port, data_in, req):
        sys.stderr.write('[%d] execute\n'%(rank))

        mesh = as_dagr_cartesian_mesh(data_in[0])

        table = dagr_table.New()
        table.copy_metadata(mesh)

        table.declare_columns(['step','time'], ['ul','d'])
        table << mesh.get_time_step() << mesh.get_time()

        for var_name in var_names:

            table.declare_columns(['min '+var_name, 'avg '+var_name, \
                'max '+var_name, 'std '+var_name, 'low_q '+var_name, \
                'med '+var_name, 'up_q '+var_name], ['d']*7)

            var = mesh.get_point_arrays().get(var_name).as_array()

            table << float(np.min(var)) << float(np.average(var)) \
                << float(np.max(var)) << float(np.std(var)) \
                << map(float, np.percentile(var, [25.,50.,75.]))

        return table
    return execute

def get_reduce_callback(rank):
    def reduce(data_in_0, data_in_1):
        sys.stderr.write('[%d] reduce\n'%(rank))

        table_0 = as_dagr_table(data_in_0)
        table_1 = as_dagr_table(data_in_1)

        data_out = None
        if table_0 is not None and table_1 is not None:
            data_out = as_dagr_table(table_0.new_copy())
            data_out.concatenate_rows(table_1)

        elif table_0 is not None:
            data_out = table_0.new_copy()

        elif table_1 is not None:
            data_out = table_1.new_copy()

        return data_out
    return reduce

if (rank == 0):
    sys.stderr.write('Testing on %d MPI processes %d threads\n'%(n_ranks, n_threads))

cfr = dagr_cf_reader.New()
cfr.set_files_regex(data_regex)

stats = dagr_programmable_algorithm.New()
stats.set_request_callback(get_request_callback(var_names))
stats.set_execute_callback(get_execute_callback(rank, var_names))
stats.set_input_connection(cfr.get_output_port())

mr = dagr_programmable_reduce.New()
mr.set_input_connection(stats.get_output_port())
mr.set_first_step(first_step)
mr.set_last_step(last_step)
mr.set_thread_pool_size(n_threads)
mr.set_reduce_callback(get_reduce_callback(rank))

sort = dagr_table_sort.New()
sort.set_input_connection(mr.get_output_port())
sort.set_index_column('time')

cal = dagr_table_calendar.New()
cal.set_input_connection(sort.get_output_port())

if os.path.exists(out_file):
  tr = dagr_table_reader.New()
  tr.set_file_name(out_file)

  diff = dagr_dataset_diff.New()
  diff.set_input_connection(0, tr.get_output_port())
  diff.set_input_connection(1, cal.get_output_port())
  diff.update()
else:
  #write data
  tw = dagr_table_writer.New()
  tw.set_input_connection(cal.get_output_port())
  #tw.set_output_format_csv()
  tw.set_output_format_bin()
  tw.set_file_name(out_file)

  tw.update()
