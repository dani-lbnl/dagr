#ifndef array_writer_h
#define array_writer_h

#include "dagr_shared_object.h"
#include "dagr_algorithm.h"
#include "dagr_metadata.h"

#include <vector>
#include <iostream>

DAGR_SHARED_OBJECT_FORWARD_DECL(array_writer)

/**
an example implementation of a dagr_algorithm
that writes arrays to a user provided stream
*/
class array_writer : public dagr_algorithm
{
public:
    DAGR_ALGORITHM_STATIC_NEW(array_writer)
    ~array_writer();

protected:
    array_writer();

private:
    const_p_dagr_dataset execute(
        unsigned int port,
        const std::vector<const_p_dagr_dataset> &input_data,
        const dagr_metadata &request) override;
};

#endif
