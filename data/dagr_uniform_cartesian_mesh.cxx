#include "dagr_uniform_cartesian_mesh.h"

// --------------------------------------------------------------------------
dagr_uniform_cartesian_mesh::dagr_uniform_cartesian_mesh()
{}

// --------------------------------------------------------------------------
void dagr_uniform_cartesian_mesh::copy(const const_p_dagr_dataset &dataset)
{
    const_p_dagr_uniform_cartesian_mesh other
        = std::dynamic_pointer_cast<const dagr_uniform_cartesian_mesh>(dataset);

    if (!other)
        throw std::bad_cast();

    if (this == other.get())
        return;

    this->dagr_mesh::copy(dataset);
}

// --------------------------------------------------------------------------
void dagr_uniform_cartesian_mesh::shallow_copy(const p_dagr_dataset &dataset)
{
    p_dagr_uniform_cartesian_mesh other
        = std::dynamic_pointer_cast<dagr_uniform_cartesian_mesh>(dataset);

    if (!other)
        throw std::bad_cast();

    if (this == other.get())
        return;

    this->dagr_mesh::shallow_copy(dataset);
}

// --------------------------------------------------------------------------
void dagr_uniform_cartesian_mesh::swap(p_dagr_dataset &dataset)
{
    p_dagr_uniform_cartesian_mesh other
        = std::dynamic_pointer_cast<dagr_uniform_cartesian_mesh>(dataset);

    if (!other)
        throw std::bad_cast();

    if (this == other.get())
        return;

    this->dagr_mesh::swap(dataset);
}
