#include "dagr_variant_array.h"
#include "dagr_metadata.h"
#include "dagr_common.h"

// --------------------------------------------------------------------------
void dagr_variant_array::copy(const dagr_variant_array &other)
{
    TEMPLATE_DISPATCH_CLASS(
        dagr_variant_array_impl, std::string, this, &other,
        *p1_tt = *p2_tt;
        return;
        )
    TEMPLATE_DISPATCH_CLASS(
        dagr_variant_array_impl, dagr_metadata, this, &other,
        *p1_tt = *p2_tt;
        return;
        )
    TEMPLATE_DISPATCH(dagr_variant_array_impl, this,
        TT *this_t = static_cast<TT*>(this);
        this_t->copy(other);
        return;
        )
    throw std::bad_cast();
}

// --------------------------------------------------------------------------
void dagr_variant_array::append(const dagr_variant_array &other)
{
    TEMPLATE_DISPATCH_CLASS(
        dagr_variant_array_impl, std::string, this, &other,
        p1_tt->append(p2_tt->m_data);
        return;
        )
    TEMPLATE_DISPATCH_CLASS(
        dagr_variant_array_impl, dagr_metadata, this, &other,
        p1_tt->append(p2_tt->m_data);
        return;
        )
    TEMPLATE_DISPATCH(dagr_variant_array_impl, this,
        TT *this_t = static_cast<TT*>(this);
        this_t->append(other);
        return;
        )
    throw std::bad_cast();
}
