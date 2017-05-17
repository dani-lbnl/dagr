#include "dagr_dataset.h"
#include "dagr_metadata.h"

// --------------------------------------------------------------------------
dagr_dataset::dagr_dataset()
{
    this->metadata = new dagr_metadata;
}

// --------------------------------------------------------------------------
dagr_dataset::~dagr_dataset()
{
    delete this->metadata;
}

// --------------------------------------------------------------------------
void dagr_dataset::copy(const const_p_dagr_dataset &other)
{
    *this->metadata = *(other->metadata);
}

// --------------------------------------------------------------------------
void dagr_dataset::shallow_copy(const p_dagr_dataset &other)
{
    *this->metadata = *(other->metadata);
}

// --------------------------------------------------------------------------
void dagr_dataset::swap(p_dagr_dataset &other)
{
    dagr_metadata *tmp = this->metadata;
    this->metadata = other->metadata;
    other->metadata = tmp;
}

// --------------------------------------------------------------------------
void dagr_dataset::copy_metadata(const const_p_dagr_dataset &other)
{
    *this->metadata = *(other->metadata);
}

// --------------------------------------------------------------------------
dagr_metadata &dagr_dataset::get_metadata() noexcept
{
    return *this->metadata;
}

// --------------------------------------------------------------------------
const dagr_metadata &dagr_dataset::get_metadata() const noexcept
{
    return *this->metadata;
}

// --------------------------------------------------------------------------
void dagr_dataset::set_metadata(const dagr_metadata &md)
{
    *this->metadata = md;
}

// --------------------------------------------------------------------------
void dagr_dataset::to_stream(dagr_binary_stream &bs) const
{
    this->metadata->to_stream(bs);
}

// --------------------------------------------------------------------------
void dagr_dataset::from_stream(dagr_binary_stream &bs)
{
    this->metadata->from_stream(bs);
}

// --------------------------------------------------------------------------
void dagr_dataset::to_stream(std::ostream &os) const
{
    this->metadata->to_stream(os);
}

// --------------------------------------------------------------------------
void dagr_dataset::from_stream(std::istream &)
{}
