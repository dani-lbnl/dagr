#include "dagr_mesh.h"

dagr_mesh::impl_t::impl_t()
{
    this->point_arrays = dagr_array_collection::New();
    this->cell_arrays = dagr_array_collection::New();
    this->edge_arrays = dagr_array_collection::New();
    this->face_arrays = dagr_array_collection::New();
    this->info_arrays = dagr_array_collection::New();
}

// --------------------------------------------------------------------------
dagr_mesh::dagr_mesh()
    : m_impl(std::make_shared<dagr_mesh::impl_t>())
{}

// --------------------------------------------------------------------------
void dagr_mesh::copy(const const_p_dagr_dataset &dataset)
{
    const_p_dagr_mesh other
        = std::dynamic_pointer_cast<const dagr_mesh>(dataset);

    if (!other)
        throw std::bad_cast();

    if (this == other.get())
        return;

    this->dagr_dataset::copy(dataset);

    m_impl = std::make_shared<dagr_mesh::impl_t>();
    m_impl->point_arrays->copy(other->m_impl->point_arrays);
    m_impl->cell_arrays->copy(other->m_impl->cell_arrays);
    m_impl->edge_arrays->copy(other->m_impl->edge_arrays);
    m_impl->face_arrays->copy(other->m_impl->face_arrays);
    m_impl->info_arrays->copy(other->m_impl->info_arrays);
}

// --------------------------------------------------------------------------
void dagr_mesh::shallow_copy(const p_dagr_dataset &dataset)
{
    p_dagr_mesh other
        = std::dynamic_pointer_cast<dagr_mesh>(dataset);

    if (!other)
        throw std::bad_cast();

    if (this == other.get())
        return;

    this->dagr_dataset::shallow_copy(dataset);

    m_impl = std::make_shared<dagr_mesh::impl_t>();
    m_impl->point_arrays->shallow_copy(other->m_impl->point_arrays);
    m_impl->cell_arrays->shallow_copy(other->m_impl->cell_arrays);
    m_impl->edge_arrays->shallow_copy(other->m_impl->edge_arrays);
    m_impl->face_arrays->shallow_copy(other->m_impl->face_arrays);
    m_impl->info_arrays->shallow_copy(other->m_impl->info_arrays);
}

// --------------------------------------------------------------------------
void dagr_mesh::swap(p_dagr_dataset &dataset)
{
    p_dagr_mesh other
        = std::dynamic_pointer_cast<dagr_mesh>(dataset);

    if (!other)
        throw std::bad_cast();

    if (this == other.get())
        return;

    this->dagr_dataset::swap(dataset);

    std::swap(m_impl, other->m_impl);
}

// --------------------------------------------------------------------------
void dagr_mesh::to_stream(dagr_binary_stream &s) const
{
    this->dagr_dataset::to_stream(s);
    m_impl->point_arrays->to_stream(s);
    m_impl->cell_arrays->to_stream(s);
    m_impl->edge_arrays->to_stream(s);
    m_impl->face_arrays->to_stream(s);
    m_impl->info_arrays->to_stream(s);
}

// --------------------------------------------------------------------------
void dagr_mesh::from_stream(dagr_binary_stream &s)
{
    this->dagr_dataset::from_stream(s);
    m_impl->point_arrays->from_stream(s);
    m_impl->cell_arrays->from_stream(s);
    m_impl->edge_arrays->from_stream(s);
    m_impl->face_arrays->from_stream(s);
    m_impl->info_arrays->from_stream(s);
}

// --------------------------------------------------------------------------
void dagr_mesh::to_stream(std::ostream &s) const
{
    this->dagr_dataset::to_stream(s);

    s << "point arrays = ";
    m_impl->point_arrays->to_stream(s);
    s << std::endl;
}

// --------------------------------------------------------------------------
bool dagr_mesh::empty() const noexcept
{
    return
        !( m_impl->point_arrays->size()
        || m_impl->cell_arrays->size()
        || m_impl->edge_arrays->size()
        || m_impl->face_arrays->size()
        || m_impl->info_arrays->size());
}
