#ifndef dagr_table_to_stream_h
#define dagr_table_to_stream_h

#include "dagr_shared_object.h"
#include "dagr_algorithm.h"
#include "dagr_metadata.h"

#include <string>
#include <vector>
#include <iostream>

DAGR_SHARED_OBJECT_FORWARD_DECL(dagr_table_to_stream)

/// an algorithm that serializes a table to a c++ stream object.
/// This is primarilly useful for debugging.
class dagr_table_to_stream : public dagr_algorithm
{
public:
    DAGR_ALGORITHM_STATIC_NEW(dagr_table_to_stream)
    ~dagr_table_to_stream();

    // report/initialize to/from Boost program options
    // objects.
    DAGR_GET_ALGORITHM_PROPERTIES_DESCRIPTION()
    DAGR_SET_ALGORITHM_PROPERTIES()

    DAGR_ALGORITHM_PROPERTY(std::string, header)
    DAGR_ALGORITHM_PROPERTY(std::string, footer)

    // set the stream object to store the table in.
    // note that this stream must out live it's use here
    // as streams are not copy-able and thus we store
    // a reference to it.
    void set_stream(std::ostream &s);

    // set the stream by name. stderr, stdout.
    void set_stream(const std::string &s);
    void set_stream_to_stderr();
    void set_stream_to_stdout();

protected:
    dagr_table_to_stream();

private:
    const_p_dagr_dataset execute(
        unsigned int port,
        const std::vector<const_p_dagr_dataset> &input_data,
        const dagr_metadata &request) override;

private:
    std::string header;
    std::string footer;
    std::ostream *stream;
};

#endif
