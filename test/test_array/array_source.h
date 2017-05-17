#ifndef array_source_h
#define array_source_h

#include "dagr_shared_object.h"
#include "dagr_algorithm.h"
#include <vector>
#include <string>

DAGR_SHARED_OBJECT_FORWARD_DECL(array_source)

/** an example implementation of a dagr_algorithm
that generates arrays over a number of timesteps

metadata keys:
     time
     number_of_time_steps
     array_names
     array_size
     extent

request keys:
     time_step (required)
     array_name (required)
     extent (required)
*/
class array_source : public dagr_algorithm
{
public:
    DAGR_ALGORITHM_STATIC_NEW(array_source)
    ~array_source();

    // set the number of arrays that coulod be generated.
    // these can be requested names names "array_0" ... "array_n-1"
    void set_number_of_arrays(unsigned int n);
    int get_number_of_arrays() const { return this->array_names.size(); }

    // set the size of the arrays to generate
    DAGR_ALGORITHM_PROPERTY(unsigned int, array_size)

    // set the number of timesteps to generate
    DAGR_ALGORITHM_PROPERTY(unsigned int, number_of_timesteps)

    // set the time step size
    DAGR_ALGORITHM_PROPERTY(double, time_delta)

protected:
    array_source();

private:
    dagr_metadata get_output_metadata(
        unsigned int port,
        const std::vector<dagr_metadata> &input_md) override;

    const_p_dagr_dataset execute(
        unsigned int port,
        const std::vector<const_p_dagr_dataset> &input_data,
        const dagr_metadata &request) override;

private:
    std::vector<std::string> array_names;
    unsigned int array_size;
    unsigned int number_of_timesteps;
    double time_delta;
};

#endif
