#ifndef array_executive_h
#define array_executive_h

#include "dagr_shared_object.h"
#include "dagr_algorithm_executive.h"
#include "dagr_metadata.h"

#include <vector>

DAGR_SHARED_OBJECT_FORWARD_DECL(array_executive)

class array_executive : public dagr_algorithm_executive
{
public:
    DAGR_ALGORITHM_EXECUTIVE_STATIC_NEW(array_executive)

    virtual int initialize(const dagr_metadata &md);
    virtual dagr_metadata get_next_request();

protected:
    array_executive(){}

private:
    std::vector<dagr_metadata> requests;
};

#endif
