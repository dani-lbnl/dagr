#ifndef dagr_table_reader_h
#define dagr_table_reader_h

#include "dagr_algorithm.h"
#include "dagr_metadata.h"
#include "dagr_shared_object.h"
#include "dagr_table.h"

#include <vector>
#include <string>
#include <mutex>


DAGR_SHARED_OBJECT_FORWARD_DECL(dagr_table_reader)

/// a reader for data stored in binary table format
/**
A reader for data stored in binary table format. By default
the reader reads and returns the entire table on rank 0.
The reader can partition the data accross an "index column".
The index column assigns a unique id to rows that should be
returned together. The reader reports the number of unique
ids to the pipeline which can then be requested by the pipeline
during parallel or sequential execution.

output:
    generates a table containing the data read from the file.
*/
class dagr_table_reader : public dagr_algorithm
{
public:
    DAGR_ALGORITHM_STATIC_NEW(dagr_table_reader)
    ~dagr_table_reader();

    DAGR_ALGORITHM_DELETE_COPY_ASSIGN(dagr_table_reader)

    // report/initialize to/from Boost program options
    // objects.
    DAGR_GET_ALGORITHM_PROPERTIES_DESCRIPTION()
    DAGR_SET_ALGORITHM_PROPERTIES()

    // the file from which data will be read.
    DAGR_ALGORITHM_PROPERTY(std::string, file_name)

    // name of the column containing index values.
    // if this is not empty the reader will operate
    // in parallel mode serving up requested indices
    // on demand. otherwise rank 0 reads the entire
    // table regardless of what is requested.
    DAGR_ALGORITHM_PROPERTY(std::string, index_column)

    // when set a column named "original_ids" is placed
    // into the output. values map back to the row number
    // of the source dataset. By default this is off.
    DAGR_ALGORITHM_PROPERTY(int, generate_original_ids)

    // name of columns to copy directly into metadata
    DAGR_ALGORITHM_VECTOR_PROPERTY(std::string, metadata_column_name)

    // keys that identify metadata columns
    DAGR_ALGORITHM_VECTOR_PROPERTY(std::string, metadata_column_key)

    // add a metadata column with the given key
    void add_metadata_column(const std::string &column, const std::string &key)
    {
        this->append_metadata_column_name(column);
        this->append_metadata_column_key(key);
    }

    // removes all metadata columns
    void clear_metadata_columns()
    {
        this->clear_metadata_column_names();
        this->clear_metadata_column_keys();
    }

protected:
    dagr_table_reader();

private:
    dagr_metadata get_output_metadata(unsigned int port,
        const std::vector<dagr_metadata> &input_md) override;

    const_p_dagr_dataset execute(unsigned int port,
        const std::vector<const_p_dagr_dataset> &input_data,
        const dagr_metadata &request) override;

    void set_modified() override;
    void clear_cached_metadata();

private:
    std::string file_name;
    std::string index_column;
    int generate_original_ids;
    std::vector<std::string> metadata_column_names;
    std::vector<std::string> metadata_column_keys;

    struct dagr_table_reader_internals;
    dagr_table_reader_internals *internals;
};

#endif
