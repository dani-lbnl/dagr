#include "dagr_cartesian_mesh.h"

#include <iostream>
using std::endl;

// --------------------------------------------------------------------------
dagr_cartesian_mesh::dagr_cartesian_mesh()
    : m_coordinate_arrays(dagr_array_collection::New())
{}

// --------------------------------------------------------------------------
void dagr_cartesian_mesh::copy(const const_p_dagr_dataset &dataset)
{
    const_p_dagr_cartesian_mesh other
        = std::dynamic_pointer_cast<const dagr_cartesian_mesh>(dataset);

    if (!other)
        throw std::bad_cast();

    if (this == other.get())
        return;

    this->dagr_mesh::copy(dataset);
    m_coordinate_arrays->copy(other->m_coordinate_arrays);
}

// --------------------------------------------------------------------------
void dagr_cartesian_mesh::shallow_copy(const p_dagr_dataset &dataset)
{
    p_dagr_cartesian_mesh other
        = std::dynamic_pointer_cast<dagr_cartesian_mesh>(dataset);

    if (!other)
        throw std::bad_cast();

    if (this == other.get())
        return;

    this->dagr_mesh::shallow_copy(dataset);
    m_coordinate_arrays->shallow_copy(other->m_coordinate_arrays);
}

// --------------------------------------------------------------------------
void dagr_cartesian_mesh::copy_metadata(const const_p_dagr_dataset &dataset)
{
    const_p_dagr_cartesian_mesh other
        = std::dynamic_pointer_cast<const dagr_cartesian_mesh>(dataset);

    if (!other)
        throw std::bad_cast();

    if (this == other.get())
        return;

    this->dagr_mesh::copy_metadata(dataset);

    m_coordinate_arrays->copy(other->m_coordinate_arrays);
}

// --------------------------------------------------------------------------
void dagr_cartesian_mesh::swap(p_dagr_dataset &dataset)
{
    p_dagr_cartesian_mesh other
        = std::dynamic_pointer_cast<dagr_cartesian_mesh>(dataset);

    if (!other)
        throw std::bad_cast();

    if (this == other.get())
        return;

    this->dagr_mesh::swap(dataset);
    m_coordinate_arrays->swap(other->m_coordinate_arrays);
}

// --------------------------------------------------------------------------
void dagr_cartesian_mesh::to_stream(dagr_binary_stream &s) const
{
    this->dagr_mesh::to_stream(s);
    m_coordinate_arrays->to_stream(s);
}

// --------------------------------------------------------------------------
void dagr_cartesian_mesh::from_stream(dagr_binary_stream &s)
{
    this->dagr_mesh::from_stream(s);
    m_coordinate_arrays->from_stream(s);
}

// --------------------------------------------------------------------------
void dagr_cartesian_mesh::to_stream(std::ostream &s) const
{
    this->dagr_mesh::to_stream(s);
    s << "coordinate arrays = {";
    m_coordinate_arrays->to_stream(s);
    s << "}" << endl;
}
