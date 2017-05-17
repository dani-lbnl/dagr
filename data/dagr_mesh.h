#ifndef dagr_mesh_h
#define dagr_mesh_h

#include "dagr_mesh_fwd.h"
#include "dagr_dataset.h"
#include "dagr_metadata.h"
#include "dagr_array_collection.h"

/// class for geometric data
class dagr_mesh : public dagr_dataset
{
public:
    ~dagr_mesh() = default;

    // get point centered data
    p_dagr_array_collection get_point_arrays()
    { return m_impl->point_arrays; }

    const_p_dagr_array_collection get_point_arrays() const
    { return m_impl->point_arrays; }

    // get cell centered data
    p_dagr_array_collection get_cell_arrays()
    { return m_impl->cell_arrays; }

    const_p_dagr_array_collection get_cell_arrays() const
    { return m_impl->cell_arrays; }

    // get edge centered data
    p_dagr_array_collection get_edge_arrays()
    { return m_impl->edge_arrays; }

    const_p_dagr_array_collection get_edge_arrays() const
    { return m_impl->edge_arrays; }

    // get face centered data
    p_dagr_array_collection get_face_arrays()
    { return m_impl->face_arrays; }

    const_p_dagr_array_collection get_face_arrays() const
    { return m_impl->face_arrays; }

    // get non-geometric data
    p_dagr_array_collection get_information_arrays()
    { return m_impl->info_arrays; }

    const_p_dagr_array_collection get_information_arrays() const
    { return m_impl->info_arrays; }

    // return true if the dataset is empty.
    bool empty() const noexcept override;

    // copy data and metadata. shallow copy uses reference
    // counting, while copy duplicates the data.
    void copy(const const_p_dagr_dataset &) override;
    void shallow_copy(const p_dagr_dataset &) override;

    // swap internals of the two objects
    void swap(p_dagr_dataset &) override;

    // serialize the dataset to/from the given stream
    // for I/O or communication
    void to_stream(dagr_binary_stream &) const override;
    void from_stream(dagr_binary_stream &) override;

    // stream to/from human readable representation
    void to_stream(std::ostream &) const override;
    void from_stream(std::istream &) override {}

protected:
    dagr_mesh();

public:
    struct impl_t
    {
        impl_t();
        //
        p_dagr_array_collection point_arrays;
        p_dagr_array_collection cell_arrays;
        p_dagr_array_collection edge_arrays;
        p_dagr_array_collection face_arrays;
        p_dagr_array_collection info_arrays;
    };
    std::shared_ptr<impl_t> m_impl;
};

#endif
