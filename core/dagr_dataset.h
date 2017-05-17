#ifndef dagr_dataset_h
#define dagr_dataset_h

#include "dagr_dataset_fwd.h"
#include <iosfwd>
class dagr_binary_stream;
class dagr_metadata;

/**
interface for dagr datasets.
*/
class dagr_dataset : public std::enable_shared_from_this<dagr_dataset>
{
public:
    virtual ~dagr_dataset();

    // covert to bool. true if the dataset is not empty.
    // otherwise false.
    explicit operator bool() const noexcept
    { return !this->empty(); }

    // return true if the dataset is empty.
    virtual bool empty() const noexcept
    { return true; }

    // virtual constructor. return a new dataset of the same type.
    virtual p_dagr_dataset new_instance() const = 0;

    // virtual copy constructor. return a deep copy of this
    // dataset in a new instance.
    virtual p_dagr_dataset new_copy() const = 0;

    // copy data and metadata. shallow copy uses reference
    // counting, while copy duplicates the data.
    virtual void copy(const const_p_dagr_dataset &other);
    virtual void shallow_copy(const p_dagr_dataset &other);

    // copy metadata. always a deep copy.
    virtual void copy_metadata(const const_p_dagr_dataset &other);

    // swap internals of the two objects
    virtual void swap(p_dagr_dataset &other);

    // access metadata
    virtual dagr_metadata &get_metadata() noexcept;
    virtual const dagr_metadata &get_metadata() const noexcept;
    virtual void set_metadata(const dagr_metadata &md);

    // serialize the dataset to/from the given stream
    // for I/O or communication
    virtual void to_stream(dagr_binary_stream &) const;
    virtual void from_stream(dagr_binary_stream &);

    // stream to/from human readable representation
    virtual void to_stream(std::ostream &) const;
    virtual void from_stream(std::istream &);

protected:
    dagr_dataset();

    dagr_dataset(const dagr_dataset &) = delete;
    dagr_dataset(const dagr_dataset &&) = delete;

    void operator=(const p_dagr_dataset &other) = delete;
    void operator=(p_dagr_dataset &&other) = delete;

    dagr_metadata *metadata;
};

#endif
