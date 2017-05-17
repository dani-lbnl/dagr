#include "dagr_dataset_diff.h"

#include "dagr_table.h"
#include "dagr_cartesian_mesh.h"
#include "dagr_array_collection.h"
#include "dagr_metadata.h"
#include "dagr_file_util.h"

#include <iostream>
#include <sstream>
#include <stdarg.h>
#include <cmath>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#if defined(DAGR_HAS_BOOST)
#include <boost/program_options.hpp>
#endif

using std::vector;
using std::string;
using std::ostringstream;
using std::ofstream;
using std::cerr;
using std::endl;

// --------------------------------------------------------------------------
dagr_dataset_diff::dagr_dataset_diff()
    : tolerance(1e-6)
{
    this->set_number_of_input_connections(2);
    this->set_number_of_output_ports(1);
}

// --------------------------------------------------------------------------
dagr_dataset_diff::~dagr_dataset_diff()
{}

#if defined(DAGR_HAS_BOOST)
// --------------------------------------------------------------------------
void dagr_dataset_diff::get_properties_description(
    const string &prefix, options_description &global_opts)
{
    options_description opts("Options for "
        + (prefix.empty()?"dagr_dataset_diff":prefix));

    opts.add_options()
        DAGR_POPTS_GET(double, prefix, tolerance, "relative test tolerance")
        ;

    global_opts.add(opts);
}

// --------------------------------------------------------------------------
void dagr_dataset_diff::set_properties(const string &prefix, variables_map &opts)
{
    DAGR_POPTS_SET(opts, double, prefix, tolerance)
}
#endif

// --------------------------------------------------------------------------
const_p_dagr_dataset dagr_dataset_diff::execute(
    unsigned int port,
    const std::vector<const_p_dagr_dataset> &input_data,
    const dagr_metadata &request)
{
    (void) port;
    (void) request;

    // after map-reduce phase of a parallel run, only rank 0
    // will have data. we can assume that if the first input,
    // which by convention is the reference dataset, is empty
    // then the second one should be as well.
    if (!input_data[0] && !input_data[1])
        return nullptr;

    // We need exactly two non-NULL inputs to compute a difference.
    if (!input_data[0])
    {
        DAGR_ERROR("Input dataset 1 is NULL.")
        return nullptr;
    }

    if (!input_data[1])
    {
        DAGR_ERROR("Input dataset 2 is NULL.")
        return nullptr;
    }

    // If one dataset is empty but not the other, the datasets differ.
    if (input_data[0]->empty() && !input_data[1]->empty())
    {
        DAGR_ERROR("dataset 1 is empty, 2 is not.")
        return nullptr;
    }

    if (!input_data[0]->empty() && input_data[1]->empty())
    {
        DAGR_ERROR("dataset 2 is empty, 1 is not.")
        return nullptr;
    }

    // If the datasets are both empty, they are "equal." :-/
    if (input_data[0]->empty() && input_data[1]->empty())
        return nullptr;

    // get the inputs. They can be tables or cartesian meshes.
    const_p_dagr_table table1 =
        std::dynamic_pointer_cast<const dagr_table>(input_data[0]);

    const_p_dagr_table table2 =
         std::dynamic_pointer_cast<const dagr_table>(input_data[1]);

    const_p_dagr_cartesian_mesh mesh1 =
        std::dynamic_pointer_cast<const dagr_cartesian_mesh>(input_data[0]);

    const_p_dagr_cartesian_mesh mesh2 =
        std::dynamic_pointer_cast<const dagr_cartesian_mesh>(input_data[1]);

    // No mixed types!
    if (((table1 && !table2) || (!table1 && table2)) ||
        ((mesh1 && !mesh2) || (!mesh1 && mesh2)))
    {
        DAGR_ERROR("input datasets must have matching types.");
        return nullptr;
    }

    if (!table1 && !mesh1)
    {
        DAGR_ERROR("input datasets must be dagr_tables or dagr_cartesian_meshes.")
        return nullptr;
    }

    if (table1)
    {
        if (this->compare_tables(table1, table2))
        {
            DAGR_ERROR("Failed to compare tables.");
            return nullptr;
        }
    }
    else
    {
        if (this->compare_cartesian_meshes(mesh1, mesh1))
        {
            DAGR_ERROR("Failed to compare cartesian meshes.");
            return nullptr;
        }
    }

    return nullptr;
}

// --------------------------------------------------------------------------
int dagr_dataset_diff::compare_tables(
    const_p_dagr_table table1,
    const_p_dagr_table table2)
{
    unsigned int ncols1 = table1->get_number_of_columns();
    unsigned int ncols2 = table2->get_number_of_columns();

    // If the tables are different sizes, the datasets differ.
    if (ncols1 != ncols2)
    {
        const_p_dagr_table bigger = ncols1 > ncols2 ? table1 : table2;
        const_p_dagr_table smaller = ncols1 <= ncols2 ? table1 : table2;
        unsigned int ncols = ncols1 > ncols2 ? ncols1 : ncols2;

        ostringstream oss;
        for (unsigned int i = 0; i < ncols; ++i)
        {
            std::string colname = bigger->get_column_name(i);
            if (!smaller->has_column(colname))
                oss << (oss.tellp()?", \"":"\"") << colname << "\"";
        }

        DAGR_ERROR("The baseline table has " << ncols1
            << " columns while test table has " << ncols2
            << " columns. Columns " << oss.str() << " are missing")
        return 1;
    }

    if (table1->get_number_of_rows() != table2->get_number_of_rows())
    {
        DAGR_ERROR("The baseline table has " << table1->get_number_of_rows()
            << " rows while test table has " << table2->get_number_of_rows()
            << " rows.")
        return 1;
    }

    // At this point, we know that the tables are both non-empty and the same size,
    // so we simply compare them one element at a time.
    for (unsigned int col = 0; col < ncols1; ++col)
    {
        const_p_dagr_variant_array col1 = table1->get_column(col);
        const_p_dagr_variant_array col2 = table2->get_column(col);
        std::stringstream col_str;
        col_str << "In column " << col << " \""
            << table1->get_column_name(col) << "\"" << std::ends;
        this->push_frame(col_str.str());
        int status = compare_arrays(col1, col2);
        this->pop_frame();
        if (status != 0)
            return status;
    }

    return 0;
}

// --------------------------------------------------------------------------
int dagr_dataset_diff::compare_arrays(
    const_p_dagr_variant_array array1,
    const_p_dagr_variant_array array2)
{
    // Arrays of different sizes are different.
    size_t n_elem = array1->size();
    if (n_elem != array2->size())
    {
        DAGR_ERROR("arrays have different sizes "
            << n_elem << " and " << array2->size())
        return 1;
    }

    // handle POD arrays
    TEMPLATE_DISPATCH(const dagr_variant_array_impl,
        array1.get(),

        // we know the type of array 1 now,
        // check the type of array 2
        const TT *a2 = dynamic_cast<const TT*>(array2.get());
        if (!a2)
        {
            DAGR_ERROR("arrays have different element types.")
            return 1;
        }

        // compare elements
        const NT *pa1 = static_cast<const TT*>(array1.get())->get();
        const NT *pa2 = a2->get();

        for (size_t i = 0; i < n_elem; ++i)
        {
            // we don't care too much about performance here so
            // use double precision for the comparison.
            double ref_val = static_cast<double>(pa1[i]);  // reference
            double comp_val = static_cast<double>(pa2[i]); // computed

            // Compute the relative difference.
            double rel_diff = 0.0;
            if (ref_val != 0.0)
                rel_diff = std::abs(comp_val - ref_val) / std::abs(ref_val);
            else if (comp_val != 0.0)
                rel_diff = std::abs(comp_val - ref_val) / std::abs(comp_val);

            if (rel_diff > this->tolerance)
            {
                DAGR_ERROR("relative difference " << rel_diff << " exceeds tolerance "
                    << this->tolerance << " in element " << i << ". ref value \""
                    << ref_val << "\" is not equal to test value \"" << comp_val << "\"")
                return 1;
            }
        }

        // we are here, arrays are the same
        return 0;
        )
    // handle arrays of strings
    TEMPLATE_DISPATCH_CLASS(
        const dagr_variant_array_impl, std::string,
        array1.get(), array2.get(),

        const TT *a1 = static_cast<const TT*>(array1.get());
        const TT *a2 = static_cast<const TT*>(array2.get());

        for (size_t i = 0; i < n_elem; ++i)
        {
            // compare elements
            const std::string &v1 = a1->get(i);
            const std::string &v2 = a2->get(i);
            if (v1 != v2)
            {
                DAGR_ERROR("string element " << i << " not equal. ref value \"" << v1
                    << "\" is not equal to test value \"" << v2 << "\"")
                return 1;
            }
        }

        // we are here, arrays are the same
        return 0;
        )

    // we are here, array 1 type is not handled
    DAGR_ERROR("diff for the element type of "
        "array1 is not implemented.")
    return 1;
}

// --------------------------------------------------------------------------
int dagr_dataset_diff::compare_array_collections(
    const_p_dagr_array_collection reference_arrays,
    const_p_dagr_array_collection data_arrays)
{
    // The data arrays should contain all the data in the reference arrays.
    for (unsigned int i = 0; i < reference_arrays->size(); ++i)
    {
        if (!data_arrays->has(reference_arrays->get_name(i)))
        {
            DAGR_ERROR("data array collection does not have array \""
                 << reference_arrays->get_name(i)
                 << "\" from the reference array collection.")
            return 1;
         }
    }

    // Now diff the contents.
    for (unsigned int i = 0; i < reference_arrays->size(); ++i)
    {
        const_p_dagr_variant_array a1 = reference_arrays->get(i);
        string name = reference_arrays->get_name(i);
        const_p_dagr_variant_array a2 = data_arrays->get(name);
        this->push_frame(name);
        int status = this->compare_arrays(a1, a2);
        this->pop_frame();
        if (status != 0)
            return status;
    }
    return 0;
}

// --------------------------------------------------------------------------
int dagr_dataset_diff::compare_cartesian_meshes(
    const_p_dagr_cartesian_mesh reference_mesh,
    const_p_dagr_cartesian_mesh data_mesh)
{
    // If the meshes are different sizes, the datasets differ.
    if (reference_mesh->get_x_coordinates()->size()
        != data_mesh->get_x_coordinates()->size())
    {
        DAGR_ERROR("data mesh has " << data_mesh->get_x_coordinates()->size()
            << " points in x, whereas reference mesh has "
            << reference_mesh->get_x_coordinates()->size() << ".")
        return 1;
    }
    if (reference_mesh->get_y_coordinates()->size()
        != data_mesh->get_y_coordinates()->size())
    {
        DAGR_ERROR("data mesh has " << data_mesh->get_y_coordinates()->size()
            << " points in y, whereas reference mesh has "
            << reference_mesh->get_y_coordinates()->size() << ".")
        return 1;
    }
    if (reference_mesh->get_z_coordinates()->size()
        != data_mesh->get_z_coordinates()->size())
    {
        DAGR_ERROR("data mesh has " << data_mesh->get_z_coordinates()->size()
            << " points in z, whereas reference mesh has "
            << reference_mesh->get_z_coordinates()->size() << ".")
        return 1;
    }

    // If the arrays are different in shape or in content, the datasets differ.
    int status;
    const_p_dagr_array_collection arrays1, arrays2;

    // Point arrays.
    arrays1 = reference_mesh->get_point_arrays();
    arrays2 = data_mesh->get_point_arrays();
    this->push_frame("Point arrays");
    status = this->compare_array_collections(arrays1, arrays2);
    this->pop_frame();
    if (status != 0)
        return status;

    // cell-centered arrays.
    arrays1 = reference_mesh->get_cell_arrays();
    arrays2 = data_mesh->get_cell_arrays();
    this->push_frame("Cell arrays");
    status = this->compare_array_collections(arrays1, arrays2);
    this->pop_frame();
    if (status != 0)
        return status;

    // Edge-centered arrays.
    arrays1 = reference_mesh->get_edge_arrays();
    arrays2 = data_mesh->get_edge_arrays();
    this->push_frame("Edge arrays");
    status = this->compare_array_collections(arrays1, arrays2);
    this->pop_frame();
    if (status != 0)
      return status;

    // Face-centered arrays.
    arrays1 = reference_mesh->get_face_arrays();
    arrays2 = data_mesh->get_face_arrays();
    this->push_frame("Face arrays");
    status = this->compare_array_collections(arrays1, arrays2);
    this->pop_frame();
    if (status != 0)
        return status;

    // Non-geometric arrays.
    arrays1 = reference_mesh->get_information_arrays();
    arrays2 = data_mesh->get_information_arrays();
    this->push_frame("Informational arrays");
    status = this->compare_array_collections(arrays1, arrays2);
    this->pop_frame();
    if (status != 0)
        return status;

    // Coordinate arrays.
    this->push_frame("X coordinates");
    status = this->compare_arrays(reference_mesh->get_x_coordinates(),
                                  data_mesh->get_x_coordinates());
    this->pop_frame();
    if (status != 0)
      return status;

    this->push_frame("Y coordinates");
    status = this->compare_arrays(reference_mesh->get_y_coordinates(),
                                  data_mesh->get_y_coordinates());
    this->pop_frame();
    if (status != 0)
        return status;

    this->push_frame("Z coordinates");
    status = this->compare_arrays(reference_mesh->get_z_coordinates(),
                                  data_mesh->get_z_coordinates());
    this->pop_frame();
    if (status != 0)
        return status;

    return 0;
}

void dagr_dataset_diff::push_frame(const std::string& frame)
{
  this->stack.push_back(frame);
}

void dagr_dataset_diff::pop_frame()
{
  this->stack.pop_back();
}

