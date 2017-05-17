%{
#include "dagr_config.h"
#include "dagr_algorithm.h"
#include "dagr_apply_binary_mask.h"
#include "dagr_binary_segmentation.h"
#include "dagr_cartesian_mesh_subset.h"
#include "dagr_cartesian_mesh_regrid.h"
#include "dagr_connected_components.h"
#include "dagr_dataset_diff.h"
#include "dagr_dataset_capture.h"
#include "dagr_dataset_source.h"
#include "dagr_derived_quantity.h"
#include "dagr_derived_quantity_numerics.h"
#include "dagr_descriptive_statistics.h"
#include "dagr_evaluate_expression.h"
#include "dagr_table_region_mask.h"
#include "dagr_l2_norm.h"
#include "dagr_mask.h"
#include "dagr_programmable_algorithm.h"
#include "dagr_programmable_reduce.h"
#include "dagr_table_sort.h"
#include "dagr_table_reduce.h"
#include "dagr_table_remove_rows.h"
#include "dagr_table_to_stream.h"
#include "dagr_temporal_average.h"
#include "dagr_vorticity.h"
#include "dagr_py_object.h"
#include "dagr_py_algorithm.h"
#include "dagr_py_gil_state.h"
%}

%include "dagr_config.h"
%include "dagr_py_common.i"
%include "dagr_py_shared_ptr.i"
%include "dagr_py_core.i"
%include "dagr_py_data.i"
%include <std_string.i>

/***************************************************************************
 cartesian_mesh_subset
 ***************************************************************************/
%ignore dagr_cartesian_mesh_subset::shared_from_this;
%shared_ptr(dagr_cartesian_mesh_subset)
%ignore dagr_cartesian_mesh_subset::operator=;
%include "dagr_cartesian_mesh_subset.h"

/***************************************************************************
 cartesian_mesh_regrid
 ***************************************************************************/
%ignore dagr_cartesian_mesh_regrid::shared_from_this;
%shared_ptr(dagr_cartesian_mesh_regrid)
%ignore dagr_cartesian_mesh_regrid::operator=;
%include "dagr_cartesian_mesh_regrid.h"

/***************************************************************************
 connected_components
 ***************************************************************************/
%ignore dagr_connected_components::shared_from_this;
%shared_ptr(dagr_connected_components)
%ignore dagr_connected_components::operator=;
%include "dagr_connected_components.h"

/***************************************************************************
 l2_norm
 ***************************************************************************/
%ignore dagr_l2_norm::shared_from_this;
%shared_ptr(dagr_l2_norm)
%ignore dagr_l2_norm::operator=;
%include "dagr_l2_norm.h"

/***************************************************************************
 mask
 ***************************************************************************/
%ignore dagr_mask::shared_from_this;
%shared_ptr(dagr_mask)
%ignore dagr_mask::operator=;
%include "dagr_mask.h"

/***************************************************************************
 table_reduce
 ***************************************************************************/
%ignore dagr_table_reduce::shared_from_this;
%shared_ptr(dagr_table_reduce)
%ignore dagr_table_reduce::operator=;
%include "dagr_table_reduce.h"

/***************************************************************************
 table_to_stream
 ***************************************************************************/
%ignore dagr_table_to_stream::shared_from_this;
%shared_ptr(dagr_table_to_stream)
%ignore dagr_table_to_stream::operator=;
%ignore dagr_table_to_stream::set_stream(std::ostream *);
%include "dagr_table_to_stream.h"

/***************************************************************************
 temporal_average
 ***************************************************************************/
%ignore dagr_temporal_average::shared_from_this;
%shared_ptr(dagr_temporal_average)
%ignore dagr_temporal_average::operator=;
%include "dagr_temporal_average.h"

/***************************************************************************
 vorticity
 ***************************************************************************/
%ignore dagr_vorticity::shared_from_this;
%shared_ptr(dagr_vorticity)
%ignore dagr_vorticity::operator=;
%include "dagr_vorticity.h"

/***************************************************************************
 programmable_algorithm
 ***************************************************************************/
%ignore dagr_programmable_algorithm::shared_from_this;
%shared_ptr(dagr_programmable_algorithm)
%extend dagr_programmable_algorithm
{
    // note: its not worth acquiring the GIL while setting the callbacks
    // as these are intended to be used only from the main thread during
    // initialization

    void set_report_callback(PyObject *f)
    {
        self->set_report_callback(dagr_py_algorithm::report_callback(f));
    }

    void set_request_callback(PyObject *f)
    {
        self->set_request_callback(dagr_py_algorithm::request_callback(f));
    }

    void set_execute_callback(PyObject *f)
    {
        self->set_execute_callback(dagr_py_algorithm::execute_callback(f));
    }
}
%ignore dagr_programmable_algorithm::operator=;
%ignore dagr_programmable_algorithm::set_report_callback;
%ignore dagr_programmable_algorithm::get_report_callback;
%ignore dagr_programmable_algorithm::set_request_callback;
%ignore dagr_programmable_algorithm::get_request_callback;
%ignore dagr_programmable_algorithm::set_execute_callback;
%ignore dagr_programmable_algorithm::get_execute_callback;
%include "dagr_programmable_algorithm_fwd.h"
%include "dagr_programmable_algorithm.h"

/***************************************************************************
 programmable_reduce
 ***************************************************************************/
%ignore dagr_programmable_reduce::shared_from_this;
%shared_ptr(dagr_programmable_reduce)
%extend dagr_programmable_reduce
{
    // note: its not worth acquiring the GIL while setting the callbacks
    // as these are intended to be used only from the main thread during
    // initialization

    void set_report_callback(PyObject *f)
    {
        self->set_report_callback(dagr_py_algorithm::report_callback(f));
    }

    void set_request_callback(PyObject *f)
    {
        self->set_request_callback(dagr_py_algorithm::request_callback(f));
    }

    void set_reduce_callback(PyObject *f)
    {
        self->set_reduce_callback(dagr_py_algorithm::reduce_callback(f));
    }
}
%ignore dagr_programmable_reduce::operator=;
%ignore dagr_programmable_reduce::set_report_callback;
%ignore dagr_programmable_reduce::get_report_callback;
%ignore dagr_programmable_reduce::set_request_callback;
%ignore dagr_programmable_reduce::get_request_callback;
%ignore dagr_programmable_reduce::set_reduce_callback;
%ignore dagr_programmable_reduce::get_reduce_callback;
%include "dagr_programmable_reduce_fwd.h"
%include "dagr_programmable_reduce.h"

/***************************************************************************
 derived_quantity
 ***************************************************************************/
%ignore dagr_derived_quantity::shared_from_this;
%shared_ptr(dagr_derived_quantity)
%extend dagr_derived_quantity
{
    // see notes in dagr_programmable_algorithm

    void set_execute_callback(PyObject *f)
    {
        self->set_execute_callback(dagr_py_algorithm::execute_callback(f));
    }
}
%ignore dagr_derived_quantity::operator=;
%ignore dagr_derived_quantity::set_execute_callback;
%ignore dagr_derived_quantity::get_execute_callback;
%include "dagr_derived_quantity.h"
%include "dagr_derived_quantity_numerics.h"

/***************************************************************************
 descriptive_statistics
 ***************************************************************************/
%ignore dagr_descriptive_statistics::shared_from_this;
%shared_ptr(dagr_descriptive_statistics)
%ignore dagr_descriptive_statistics::operator=;
%include "dagr_descriptive_statistics.h"

/***************************************************************************
 table_sort
 ***************************************************************************/
%ignore dagr_table_sort::shared_from_this;
%shared_ptr(dagr_table_sort)
%ignore dagr_table_sort::operator=;
%include "dagr_table_sort.h"

/***************************************************************************
 dataset_diff
 ***************************************************************************/
%ignore dagr_dataset_diff::shared_from_this;
%shared_ptr(dagr_dataset_diff)
%ignore dagr_dataset_diff::operator=;
%include "dagr_dataset_diff.h"

/***************************************************************************
 binary_segmentation
 ***************************************************************************/
%ignore dagr_binary_segmentation::shared_from_this;
%shared_ptr(dagr_binary_segmentation)
%ignore dagr_binary_segmentation::operator=;
%include "dagr_binary_segmentation.h"

/***************************************************************************
 apply_binary_mask
 ***************************************************************************/
%ignore dagr_apply_binary_mask::shared_from_this;
%shared_ptr(dagr_apply_binary_mask)
%ignore dagr_apply_binary_mask::operator=;
%include "dagr_apply_binary_mask.h"

/***************************************************************************
 evaluate_expression
 ***************************************************************************/
%ignore dagr_evaluate_expression::shared_from_this;
%shared_ptr(dagr_evaluate_expression)
%ignore dagr_evaluate_expression::operator=;
%include "dagr_evaluate_expression.h"

/***************************************************************************
 table_remove_rows
 ***************************************************************************/
%ignore dagr_table_remove_rows::shared_from_this;
%shared_ptr(dagr_table_remove_rows)
%ignore dagr_table_remove_rows::operator=;
%include "dagr_table_remove_rows.h"

/***************************************************************************
 dataset_source
 ***************************************************************************/
%ignore dagr_dataset_source::shared_from_this;
%shared_ptr(dagr_dataset_source)
%ignore dagr_dataset_source::operator=;
%include "dagr_dataset_source.h"

/***************************************************************************
 dataset_capture
 ***************************************************************************/
%ignore dagr_dataset_capture::shared_from_this;
%shared_ptr(dagr_dataset_capture)
%ignore dagr_dataset_capture::operator=;
%include "dagr_dataset_capture.h"
