#include "dagr_table.h"

#include "dagr_binary_stream.h"
#include "dagr_mesh.h"

using std::vector;
using std::map;

dagr_table::impl_t::impl_t() :
    columns(dagr_array_collection::New()), active_column(0)
{}


// --------------------------------------------------------------------------
dagr_table::dagr_table() : m_impl(new dagr_table::impl_t())
{}

// --------------------------------------------------------------------------
void dagr_table::clear()
{
    this->get_metadata().clear();
    m_impl->columns->clear();
    m_impl->active_column = 0;
}

// --------------------------------------------------------------------------
bool dagr_table::empty() const noexcept
{
    return m_impl->columns->size() == 0;
}

// --------------------------------------------------------------------------
unsigned int dagr_table::get_number_of_columns() const noexcept
{
    return m_impl->columns->size();
}

// --------------------------------------------------------------------------
unsigned long dagr_table::get_number_of_rows() const noexcept
{
    if (m_impl->columns->size())
        return m_impl->columns->get(0)->size();

    return 0;
}

// --------------------------------------------------------------------------
p_dagr_variant_array dagr_table::get_column(const std::string &col_name)
{
    return m_impl->columns->get(col_name);
}

// --------------------------------------------------------------------------
const_p_dagr_variant_array dagr_table::get_column(const std::string &col_name) const
{
    return m_impl->columns->get(col_name);
}

// --------------------------------------------------------------------------
void dagr_table::resize(unsigned long n)
{
    unsigned int n_cols = m_impl->columns->size();
    for (unsigned int i = 0; i < n_cols; ++i)
        m_impl->columns->get(i)->resize(n);
}

// --------------------------------------------------------------------------
void dagr_table::reserve(unsigned long n)
{
    unsigned int n_cols = m_impl->columns->size();
    for (unsigned int i = 0; i < n_cols; ++i)
        m_impl->columns->get(i)->reserve(n);
}

// --------------------------------------------------------------------------
void dagr_table::to_stream(dagr_binary_stream &s) const
{
    this->dagr_dataset::to_stream(s);
    m_impl->columns->to_stream(s);
}

// --------------------------------------------------------------------------
void dagr_table::from_stream(dagr_binary_stream &s)
{
    this->clear();
    this->dagr_dataset::from_stream(s);
    m_impl->columns->from_stream(s);
}

// --------------------------------------------------------------------------
void dagr_table::to_stream(std::ostream &s) const
{
    // because this is used for general purpose I/O
    // we don't let the base class insert anything.

    unsigned int n_cols = m_impl->columns->size();
    if (n_cols)
    {
        s << "\"" << m_impl->columns->get_name(0) << "\"";
        for (unsigned int i = 1; i < n_cols; ++i)
            s << ", \"" << m_impl->columns->get_name(i) << "\"";
        s << std::endl;
    }
    unsigned long long n_rows = this->get_number_of_rows();
    for (unsigned long long j = 0; j < n_rows; ++j)
    {
        if (n_cols)
        {
            TEMPLATE_DISPATCH(dagr_variant_array_impl,
                m_impl->columns->get(0).get(),
                TT *a = dynamic_cast<TT*>(m_impl->columns->get(0).get());
                NT v = NT();
                a->get(j, v);
                s << v;
                )
            else TEMPLATE_DISPATCH_CASE(dagr_variant_array_impl,
                std::string,
                m_impl->columns->get(0).get(),
                TT *a = dynamic_cast<TT*>(m_impl->columns->get(0).get());
                NT v = NT();
                a->get(j, v);
                s << "\"" << v << "\"";
                )
            for (unsigned int i = 1; i < n_cols; ++i)
            {
                TEMPLATE_DISPATCH(dagr_variant_array_impl,
                    m_impl->columns->get(i).get(),
                    TT *a = dynamic_cast<TT*>(m_impl->columns->get(i).get());
                    NT v = NT();
                    a->get(j, v);
                    s << ", " << v;
                    )
                else TEMPLATE_DISPATCH_CASE(dagr_variant_array_impl,
                    std::string,
                    m_impl->columns->get(i).get(),
                    TT *a = dynamic_cast<TT*>(m_impl->columns->get(i).get());
                    NT v = NT();
                    a->get(j, v);
                    s << ", \"" << v << "\"";
                    )
            }
        }
        s << std::endl;
    }
}

// --------------------------------------------------------------------------
void dagr_table::copy(const const_p_dagr_dataset &dataset)
{
    const_p_dagr_table other
        = std::dynamic_pointer_cast<const dagr_table>(dataset);

    if (!other)
        throw std::bad_cast();

    if (this == other.get())
        return;

    this->clear();

    this->dagr_dataset::copy(dataset);
    m_impl->columns->copy(other->m_impl->columns);
}

// --------------------------------------------------------------------------
void dagr_table::copy(const const_p_dagr_table &other,
    unsigned long first_row, unsigned long last_row)
{
    if (this == other.get())
        return;

    this->clear();

    if (!other)
        return;

    this->dagr_dataset::copy(other);

    unsigned int n_cols = other->get_number_of_columns();
    for (unsigned int i = 0; i < n_cols; ++i)
    {
        m_impl->columns->append(other->m_impl->columns->get_name(i),
            other->m_impl->columns->get(i)->new_copy(first_row, last_row));
    }
}

// --------------------------------------------------------------------------
void dagr_table::shallow_copy(const p_dagr_dataset &dataset)
{
    const_p_dagr_table other
        = std::dynamic_pointer_cast<const dagr_table>(dataset);

    if (!other)
        throw std::bad_cast();

    this->clear();

    this->dagr_dataset::shallow_copy(dataset);
    m_impl->columns->shallow_copy(other->m_impl->columns);
}

// --------------------------------------------------------------------------
void dagr_table::copy_structure(const const_p_dagr_table &other)
{
    unsigned int n_cols = other->get_number_of_columns();
    for (unsigned int i = 0; i < n_cols; ++i)
    {
        m_impl->columns->append(other->m_impl->columns->get_name(i),
            other->m_impl->columns->get(i)->new_instance());
    }
}

// --------------------------------------------------------------------------
void dagr_table::swap(p_dagr_dataset &dataset)
{
    p_dagr_table other
        = std::dynamic_pointer_cast<dagr_table>(dataset);

    if (!other)
        throw std::bad_cast();

    this->dagr_dataset::swap(dataset);

    std::shared_ptr<dagr_table::impl_t> tmp = m_impl;
    m_impl = other->m_impl;
    other->m_impl = tmp;

    m_impl->active_column = 0;
}

// --------------------------------------------------------------------------
void dagr_table::concatenate_rows(const const_p_dagr_table &other)
{
    if (!other)
        return;

    size_t n_cols = 0;
    if ((n_cols = other->get_number_of_columns()) != this->get_number_of_columns())
    {
        DAGR_ERROR("append failed. Number of columns don't match")
        return;
    }

    for (size_t i = 0; i < n_cols; ++i)
        this->get_column(i)->append(*(other->get_column(i).get()));
}

// --------------------------------------------------------------------------
void dagr_table::concatenate_cols(const const_p_dagr_table &other, bool deep)
{
    if (!other)
        return;

    unsigned int n_cols = other->get_number_of_columns();
    for (unsigned int i=0; i<n_cols; ++i)
    {
        m_impl->columns->append(
            other->m_impl->columns->get_name(i),
            deep ? other->m_impl->columns->get(i)->new_copy()
                : std::const_pointer_cast<dagr_variant_array>(
                    other->m_impl->columns->get(i)));
    }
}
