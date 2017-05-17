#include "dagr_mask.h"

#include "dagr_mesh.h"
#include "dagr_array_collection.h"
#include "dagr_variant_array.h"
#include "dagr_metadata.h"
#include "dagr_cartesian_mesh.h"

#include <algorithm>
#include <iostream>
#include <deque>
#include <set>
#include <cmath>

using std::deque;
using std::vector;
using std::set;
using std::cerr;
using std::endl;

//#define DAGR_DEBUG
namespace {

// set locations in the output where the input array
// has values within the low high range.
template <typename num_t>
void apply_mask(
    num_t *output, const num_t *input, size_t n_vals,
    num_t low, num_t high, num_t mask)
{
    for (size_t i = 0; i < n_vals; ++i)
        output[i] = ((input[i] >= low) && (input[i] <= high)) ? mask : input[i];
}
};

// a note about the defaults:
// range is set to lowest - max, which is all inclusive. the user
// so the could set just one value. not setting the other is not an
// error.
// mask value is set to NaN as way to detect an error. A mask value
// must be specified.
// --------------------------------------------------------------------------
dagr_mask::dagr_mask() : mask_variables(),
    low_threshold_value(std::numeric_limits<double>::lowest()),
    high_threshold_value(std::numeric_limits<double>::max()),
    mask_value(std::numeric_limits<double>::quiet_NaN())
{
    this->set_number_of_input_connections(1);
    this->set_number_of_output_ports(1);
}

// --------------------------------------------------------------------------
dagr_mask::~dagr_mask()
{}

// --------------------------------------------------------------------------
std::vector<std::string> dagr_mask::get_mask_variables(
    const dagr_metadata &request)
{
    std::vector<std::string> mask_vars = this->mask_variables;

    if (mask_vars.empty() &&
        request.has("dagr_mask::mask_variables"))
            request.get("dagr_mask::mask_variables",
                mask_vars);

    return mask_vars;
}

// --------------------------------------------------------------------------
std::vector<dagr_metadata> dagr_mask::get_upstream_request(
    unsigned int port,
    const std::vector<dagr_metadata> &input_md,
    const dagr_metadata &request)
{
#ifdef DAGR_DEBUG
    cerr << dagr_parallel_id()
        << "dagr_mask::get_upstream_request" << endl;
#endif
    (void) port;
    (void) input_md;

    vector<dagr_metadata> up_reqs;

    // get the name of the arrays to request
    std::vector<std::string> mask_vars = this->get_mask_variables(request);
    if (mask_vars.empty())
    {
        DAGR_ERROR("A threshold variable was not specified")
        return up_reqs;
    }

    // pass the incoming request upstream, and
    // add in what we need
    dagr_metadata req(request);
    std::set<std::string> arrays;
    if (req.has("arrays"))
        req.get("arrays", arrays);
    arrays.insert(mask_vars.begin(), mask_vars.end());

    req.insert("arrays", arrays);

    // send up
    up_reqs.push_back(req);
    return up_reqs;
}

// --------------------------------------------------------------------------
const_p_dagr_dataset dagr_mask::execute(
    unsigned int port,
    const std::vector<const_p_dagr_dataset> &input_data,
    const dagr_metadata &request)
{
#ifdef DAGR_DEBUG
    cerr << dagr_parallel_id()
        << "dagr_mask::execute" << endl;
#endif
    (void)port;

    // get the input
    const_p_dagr_mesh in_mesh =
        std::dynamic_pointer_cast<const dagr_mesh>(
            input_data[0]);
    if (!in_mesh)
    {
        DAGR_ERROR("empty input, or not a mesh")
        return nullptr;
    }

    // create output and copy metadata, coordinates, etc
    p_dagr_mesh out_mesh =
        std::dynamic_pointer_cast<dagr_mesh>(in_mesh->new_instance());

    out_mesh->shallow_copy(
        std::const_pointer_cast<dagr_mesh>(in_mesh));

    // get the input array names
    std::vector<std::string> mask_vars = this->get_mask_variables(request);
    if (mask_vars.empty())
    {
        DAGR_ERROR("A mask variable was not specified")
        return nullptr;
    }

    // get threshold values
    double low_val = this->low_threshold_value;

    if (low_val == std::numeric_limits<double>::lowest()
        && request.has("dagr_mask::low_threshold_value"))
        request.get("dagr_mask::low_threshold_value", low_val);

    double high_val = this->high_threshold_value;

    if (high_val == std::numeric_limits<double>::max()
        && request.has("dagr_mask::high_threshold_value"))
        request.get("dagr_mask::high_threshold_value", high_val);

    // get the mask value
    double mask_val = this->mask_value;

    if (std::isnan(mask_val)
        && request.has("dagr_mask::mask_value"))
        request.get("dagr_mask::mask_value", mask_val);

    if (std::isnan(mask_val))
    {
        DAGR_ERROR("A mask value was not specified")
        return nullptr;
    }

    size_t n_arrays = mask_vars.size();
    for (size_t i = 0; i < n_arrays; ++i)
    {
        // get the input array
        const_p_dagr_variant_array input_array
            = out_mesh->get_point_arrays()->get(mask_vars[i]);
        if (!input_array)
        {
            DAGR_ERROR("mask variable \"" << mask_vars[i]
                << "\" is not in the input")
            return nullptr;
        }

        // apply the mask
        size_t n_elem = input_array->size();
        p_dagr_variant_array mask = input_array->new_instance(n_elem);

        TEMPLATE_DISPATCH(dagr_variant_array_impl,
            mask.get(),

            const NT *p_in = static_cast<const TT*>(input_array.get())->get();
            NT *p_mask = static_cast<TT*>(mask.get())->get();

            ::apply_mask(p_mask, p_in,  n_elem,
                static_cast<NT>(low_val), static_cast<NT>(high_val),
                static_cast<NT>(mask_val));
            )

        // set the masked array in the output
        out_mesh->get_point_arrays()->set(mask_vars[i], mask);
    }

    return out_mesh;
}
