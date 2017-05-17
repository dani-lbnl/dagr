#include "dagr_algorithm_executive.h"

// --------------------------------------------------------------------------
int dagr_algorithm_executive::initialize(const dagr_metadata &)
{
    // make a non-empty request. any key that's not used will
    // work here, prepending __ to add some extra safety in this
    // regard.
    m_md.insert("__request_empty", 0);
    return 0;
}

// --------------------------------------------------------------------------
dagr_metadata dagr_algorithm_executive::get_next_request()
{
    // send the cached request and replace it with
    // an empty one.
    dagr_metadata req = m_md;
    m_md = dagr_metadata();
    return req;
}
