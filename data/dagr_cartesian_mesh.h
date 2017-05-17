#ifndef dagr_cartesian_mesh_h
#define dagr_cartesian_mesh_h

#include "dagr_mesh.h"
#include "dagr_cartesian_mesh_fwd.h"

/// data on a physically uniform cartesian mesh
class dagr_cartesian_mesh : public dagr_mesh
{
public:
    DAGR_DATASET_STATIC_NEW(dagr_cartesian_mesh)
    DAGR_DATASET_NEW_INSTANCE()
    DAGR_DATASET_NEW_COPY()

    virtual ~dagr_cartesian_mesh() = default;

    // set/get metadata
    DAGR_DATASET_METADATA(time, double, 1)
    DAGR_DATASET_METADATA(calendar, std::string, 1)
    DAGR_DATASET_METADATA(time_units, std::string, 1)
    DAGR_DATASET_METADATA(time_step, unsigned long, 1)
    DAGR_DATASET_METADATA(whole_extent, unsigned long, 6)
    DAGR_DATASET_METADATA(extent, unsigned long, 6)

    // get x coordinate array
    p_dagr_variant_array get_x_coordinates()
    { return m_coordinate_arrays->get("x"); }

    const_p_dagr_variant_array get_x_coordinates() const
    { return m_coordinate_arrays->get("x"); }

    // get y coordinate array
    p_dagr_variant_array get_y_coordinates()
    { return m_coordinate_arrays->get("y"); }

    const_p_dagr_variant_array get_y_coordinates() const
    { return m_coordinate_arrays->get("y"); }

    // get z coordinate array
    p_dagr_variant_array get_z_coordinates()
    { return m_coordinate_arrays->get("z"); }

    const_p_dagr_variant_array get_z_coordinates() const
    { return m_coordinate_arrays->get("z"); }

    // set coordinate arrays
    void set_x_coordinates(const p_dagr_variant_array &a)
    { m_coordinate_arrays->set("x", a); }

    void set_y_coordinates(const p_dagr_variant_array &a)
    { m_coordinate_arrays->set("y", a); }

    void set_z_coordinates(const p_dagr_variant_array &a)
    { m_coordinate_arrays->set("z", a); }

    // copy data and metadata. shallow copy uses reference
    // counting, while copy duplicates the data.
    void copy(const const_p_dagr_dataset &) override;
    void shallow_copy(const p_dagr_dataset &) override;

    // copy metadata. always a deep copy.
    void copy_metadata(const const_p_dagr_dataset &other) override;

    // swap internals of the two objects
    void swap(p_dagr_dataset &) override;

    // serialize the dataset to/from the given stream
    // for I/O or communication
    void to_stream(dagr_binary_stream &) const override;
    void from_stream(dagr_binary_stream &) override;

    // stream to/from human readable representation
    void to_stream(std::ostream &) const override;

protected:
    dagr_cartesian_mesh();

private:
    p_dagr_array_collection m_coordinate_arrays;
};

#endif
