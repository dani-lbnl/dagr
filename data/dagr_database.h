#ifndef dagr_database_h
#define dagr_database_h

#include "dagr_dataset.h"
#include "dagr_table_fwd.h"
#include "dagr_database_fwd.h"
#include "dagr_table_collection.h"
#include <iosfwd>
class dagr_binary_stream;

/// dagr_database - A collection of named tables
/**
A dataset consisting of a collection of named tables. This
is a thin wrapper around the dagr_table_collection implementing
the dagr_dataset API.
*/
class dagr_database : public dagr_dataset
{
public:
    DAGR_DATASET_STATIC_NEW(dagr_database)
    DAGR_DATASET_NEW_INSTANCE()
    DAGR_DATASET_NEW_COPY()

    ~dagr_database();

    // append table
    int append_table(p_dagr_table table)
    { return this->tables->append(table); }

    int append_table(const std::string &name, p_dagr_table table)
    { return this->tables->append(name, table); }

    // declare a table
    void declare_table(const std::string &name)
    { this->tables->declare(name); }

    // declare a set of unnamed tables
    void declare_tables(unsigned int n);

    // get the number of tables
    unsigned int get_number_of_tables() const
    { return this->tables->size(); }

    // get the ith table
    p_dagr_table get_table(unsigned int i)
    { return this->tables->get(i); }

    const_p_dagr_table get_table(unsigned int i) const
    { return this->tables->get(i); }

    // get table by its name
    p_dagr_table get_table(const std::string &name)
    { return this->tables->get(name); }

    const_p_dagr_table get_table(const std::string &name) const
    { return this->tables->get(name); }

    // get the name of the ith table
    std::string get_table_name(unsigned int i)
    { return this->tables->get_name(i); }

    const std::string &get_table_name(unsigned int i) const
    { return this->tables->get_name(i); }

    // set the table by name or index
    int set_table(const std::string &name, p_dagr_table table)
    { return this->tables->set(name, table); }

    int set_table(unsigned int i, p_dagr_table table)
    { return this->tables->set(i, table); }

    // remove the table
    int remove_table(unsigned int i)
    { return this->tables->remove(i); }

    int remove_table(const std::string &name)
    { return this->tables->remove(name); }

    // return true if the dataset is empty.
    bool empty() const noexcept override;

    // copy data and metadata. shallow copy uses reference
    // counting, while copy duplicates the data.
    void copy(const const_p_dagr_dataset &other) override;
    void shallow_copy(const p_dagr_dataset &other) override;

    // copy metadata. always a deep copy.
    void copy_metadata(const const_p_dagr_dataset &other) override;

    // swap internals of the two objects
    void swap(p_dagr_dataset &other) override;

    // serialize the dataset to/from the given stream
    // for I/O or communication
    void to_stream(dagr_binary_stream &) const override;
    void from_stream(dagr_binary_stream &) override;

    // stream to/from human readable representation
    void to_stream(std::ostream &) const override;
    void from_stream(std::istream &) override {}

protected:
    dagr_database();

private:
    p_dagr_table_collection tables;
};

#endif
