#ifndef dagr_table_region_mask_h
#define dagr_table_region_mask_h

#include "dagr_shared_object.h"
#include "dagr_algorithm.h"
#include "dagr_metadata.h"

#include <string>
#include <vector>

DAGR_SHARED_OBJECT_FORWARD_DECL(dagr_table_region_mask)

/**
 an algorithm that identifies rows in the table that are
inside the list of regions provided. a new column, called
the mask column is created. It has 1's if the row is in
the set of regions, otherwise 0's. The invert property
can be used to invert the result.
*/
class dagr_table_region_mask : public dagr_algorithm
{
public:
    DAGR_ALGORITHM_STATIC_NEW(dagr_table_region_mask)
    ~dagr_table_region_mask();

    // report/initialize to/from Boost program options
    // objects.
    DAGR_GET_ALGORITHM_PROPERTIES_DESCRIPTION()
    DAGR_SET_ALGORITHM_PROPERTIES()

    // set the name of the columns to use as coordinates
    // defaults are empty which disables space based filtering
    DAGR_ALGORITHM_PROPERTY(std::string, x_coordinate_column)
    DAGR_ALGORITHM_PROPERTY(std::string, y_coordinate_column)

    // set the name of the column to store the mask in
    // the mask is a column of 1 and 0 indicating if the
    // row satsifies the criteria or not. the default is
    // "region_mask"
    DAGR_ALGORITHM_PROPERTY(std::string, result_column);

    // the following group of properties define a set of
    // polygons describing arbitrary regions. events are removed
    // when outside of the regions. note: must explicitly include
    // end point.
    DAGR_ALGORITHM_VECTOR_PROPERTY(unsigned long, region_size)
    DAGR_ALGORITHM_VECTOR_PROPERTY(unsigned long, region_start)
    DAGR_ALGORITHM_VECTOR_PROPERTY(double, region_x_coordinate)
    DAGR_ALGORITHM_VECTOR_PROPERTY(double, region_y_coordinate)

    // clear the list of region definitions.
    void clear_regions();

    // load a predefined basin region by name. one can use
    // dagr_geography::get_cyclone_basin_names to obtain
    // the list of basin names. the basin region definition
    // is appended to the current list of regions.
    int load_cyclone_basin(const std::string &name);

    // invert the test. when true the result will be true
    // if the point is outside the regions
    DAGR_ALGORITHM_PROPERTY(int, invert)

protected:
    dagr_table_region_mask();

private:
    const_p_dagr_dataset execute(unsigned int port,
        const std::vector<const_p_dagr_dataset> &input_data,
        const dagr_metadata &request) override;

private:
    std::string x_coordinate_column;
    std::string y_coordinate_column;
    std::string result_column;
    std::vector<unsigned long> region_sizes;
    std::vector<unsigned long> region_starts;
    std::vector<double> region_x_coordinates;
    std::vector<double> region_y_coordinates;
    int invert;
};

#endif
