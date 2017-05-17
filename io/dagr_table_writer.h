#ifndef dagr_table_writer_h
#define dagr_table_writer_h

#include "dagr_shared_object.h"
#include "dagr_algorithm.h"
#include "dagr_metadata.h"
#include "dagr_table_fwd.h"

#include <vector>
#include <string>

DAGR_SHARED_OBJECT_FORWARD_DECL(dagr_table_writer)

/// dagr_table_writer - writes tabular datasets in CSV format.
/**
an algorithm that writes tabular data in a CSV (comma separated value)
format that is easily ingested by most spreadsheet apps. Each page of
a database is written to a file.
*/
class dagr_table_writer : public dagr_algorithm
{
public:
    DAGR_ALGORITHM_STATIC_NEW(dagr_table_writer)
    ~dagr_table_writer();

    // set the output filename. for time series the substring
    // %t% is replaced with the current time step. the substring
    // %e% is replaced with .bin in binary mode and .csv otherwise
    // %s% is replaced with the table name (workbooks only).
    DAGR_ALGORITHM_PROPERTY(std::string, file_name)

    // report/initialize to/from Boost program options
    // objects.
    DAGR_GET_ALGORITHM_PROPERTIES_DESCRIPTION()
    DAGR_SET_ALGORITHM_PROPERTIES()

    // Select the output file format. 0 : csv, 1 : bin, 2 : xlsx.
    // the default is csv.
    enum {format_csv, format_bin, format_xlsx, format_auto};
    DAGR_ALGORITHM_PROPERTY(int, output_format)
    void set_output_format_csv(){ this->set_output_format(format_csv); }
    void set_output_format_bin(){ this->set_output_format(format_bin); }
    void set_output_format_xlsx(){ this->set_output_format(format_xlsx); }
    void set_output_format_auto(){ this->set_output_format(format_auto); }

protected:
    dagr_table_writer();

private:
    const_p_dagr_dataset execute(
        unsigned int port,
        const std::vector<const_p_dagr_dataset> &input_data,
        const dagr_metadata &request) override;

private:
    std::string file_name;
    int output_format;
};

#endif
