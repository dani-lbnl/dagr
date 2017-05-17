#ifndef array_h
#define array_h

#include "array_fwd.h"
#include "dagr_dataset.h"

#include <string>
#include <vector>

class dagr_binary_stream;

// trivial implementation of an array based datatset
// for testing the pipeline
class array : public dagr_dataset
{
public:
    DAGR_DATASET_STATIC_NEW(array);
    virtual ~array() = default;

    DAGR_DATASET_PROPERTY(std::vector<double>, data)
    DAGR_DATASET_PROPERTY(std::vector<size_t>, extent)
    DAGR_DATASET_PROPERTY(std::string, name)

    // return true if the dataset is empty.
    bool empty() const noexcept override
    { return this->data.empty(); }

    // return a new dataset of the same type
    p_dagr_dataset new_instance() const override
    { return p_dagr_dataset(new array()); }

    // return a new copy constructed array
    p_dagr_dataset new_copy() const override;

    size_t size() const
    { return this->data.size(); }

    void resize(size_t n);
    void clear();

    double &get(size_t i)
    { return this->data[i]; }

    const double &get(size_t i) const
    { return this->data[i]; }

    double &operator[](size_t i)
    { return this->data[i]; }

    const double &operator[](size_t i) const
    { return this->data[i]; }

    void append(double d)
    { this->data.push_back(d); }

    void copy(const const_p_dagr_dataset &) override;
    void shallow_copy(const p_dagr_dataset &) override;
    void copy_metadata(const const_p_dagr_dataset &) override;
    void swap(p_dagr_dataset &) override;

    // serialize the dataset to/from the given stream
    // for I/O or communication
    void to_stream(dagr_binary_stream &s) const override;
    void from_stream(dagr_binary_stream &s) override;

    void to_stream(std::ostream &s) const override;

protected:
    array() : extent({0,0}) {}

    array(const array &);
    void operator=(const array &);

private:
    std::string name;
    std::vector<size_t> extent;
    std::vector<double> data;
};

#endif
