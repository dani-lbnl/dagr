#ifndef dagr_uniform_cartesian_mesh_h
#define dagr_uniform_cartesian_mesh_h

#include "dagr_uniform_cartesian_mesh_fwd.h"
#include "dagr_mesh.h"

/// data on a uniform cartesian mesh
class dagr_uniform_cartesian_mesh : public dagr_mesh
{
public:
    DAGR_DATASET_STATIC_NEW(dagr_uniform_cartesian_mesh)
    DAGR_DATASET_NEW_INSTANCE()
    DAGR_DATASET_NEW_COPY()

    virtual ~dagr_uniform_cartesian_mesh() = default;

    // set/get metadata
    DAGR_DATASET_METADATA(time, double, 1)
    DAGR_DATASET_METADATA(time_step, unsigned long, 1)
    DAGR_DATASET_METADATA(spacing, double, 3)
    DAGR_DATASET_METADATA(origin, double, 3)
    DAGR_DATASET_METADATA(extent, unsigned long, 6)
    DAGR_DATASET_METADATA(local_extent, unsigned long, 6)

    // copy data and metadata. shallow copy uses reference
    // counting, while copy duplicates the data.
    void copy(const const_p_dagr_dataset &) override;
    void shallow_copy(const p_dagr_dataset &) override;

    // swap internals of the two objects
    void swap(p_dagr_dataset &) override;

protected:
    dagr_uniform_cartesian_mesh();
};

#endif
