#include "dagr_database.h"
#include "dagr_table_collection.h"
#include <sstream>

// --------------------------------------------------------------------------
dagr_database::dagr_database()
{
    this->tables = dagr_table_collection::New();
}

// --------------------------------------------------------------------------
dagr_database::~dagr_database()
{}

// --------------------------------------------------------------------------
void dagr_database::declare_tables(unsigned int n)
{
    for (unsigned int i = 0; i < n; ++i)
    {
       std::ostringstream oss;
       oss << "table_" << i;
       this->declare_table(oss.str());
    }
}

// --------------------------------------------------------------------------
bool dagr_database::empty() const noexcept
{
    return !this->tables || !this->tables->size();
}

// --------------------------------------------------------------------------
void dagr_database::copy(const const_p_dagr_dataset &o)
{
    const_p_dagr_database other
        = std::dynamic_pointer_cast<const dagr_database>(o);

    if (!other)
    {
        DAGR_ERROR("Copy failed. Source must be a database")
        return;
    }

    this->dagr_dataset::copy(o);
    this->tables->copy(other->tables);
}

// --------------------------------------------------------------------------
void dagr_database::shallow_copy(const p_dagr_dataset &o)
{
    p_dagr_database other
        = std::dynamic_pointer_cast<dagr_database>(o);

    if (!other)
    {
        DAGR_ERROR("Copy failed. Source must be a database")
        return;
    }

    this->dagr_dataset::shallow_copy(o);
    this->tables->shallow_copy(other->tables);
}

// --------------------------------------------------------------------------
void dagr_database::copy_metadata(const const_p_dagr_dataset &o)
{
    const_p_dagr_database other
        = std::dynamic_pointer_cast<const dagr_database>(o);

    if (!other)
    {
        DAGR_ERROR("Copy failed. Source must be a database")
        return;
    }

    this->dagr_dataset::copy_metadata(o);

    unsigned int n = other->tables->size();
    for (unsigned int i = 0; i < n; ++i)
    {
        p_dagr_table table = dagr_table::New();
        table->copy_metadata(other->tables->get(i));
        this->tables->append(other->tables->get_name(i), table);
    }
}

// --------------------------------------------------------------------------
void dagr_database::swap(p_dagr_dataset &o)
{
    p_dagr_database other
        = std::dynamic_pointer_cast<dagr_database>(o);

    if (!other)
    {
        DAGR_ERROR("Copy failed. Source must be a database")
        return;
    }

    this->dagr_dataset::swap(o);

    p_dagr_table_collection tmp = this->tables;
    this->tables = other->tables;
    other->tables = tmp;
}

// --------------------------------------------------------------------------
void dagr_database::to_stream(dagr_binary_stream &s) const
{
    this->dagr_dataset::to_stream(s);
    this->tables->to_stream(s);
}

// --------------------------------------------------------------------------
void dagr_database::from_stream(dagr_binary_stream &s)
{
    this->dagr_dataset::from_stream(s);
    this->tables->from_stream(s);
}

// --------------------------------------------------------------------------
void dagr_database::to_stream(std::ostream &s) const
{
    this->dagr_dataset::to_stream(s);
    this->tables->to_stream(s);
}
