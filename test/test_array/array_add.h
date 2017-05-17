#ifndef array_add_h
#define array_add_h

#include "dagr_shared_object.h"
#include "dagr_algorithm.h"
#include "dagr_metadata.h"

#include <string>
#include <vector>

DAGR_SHARED_OBJECT_FORWARD_DECL(array_add)

/**
an example implementation of a dagr_algorithm
that adds two arrays on its inputs
*/
class array_add : public dagr_algorithm
{
public:
    DAGR_ALGORITHM_STATIC_NEW(array_add)
    ~array_add();

    // sets the names of the two arrays to add
    DAGR_ALGORITHM_PROPERTY(std::string, array_1)
    DAGR_ALGORITHM_PROPERTY(std::string, array_2)

protected:
    array_add();

    // helper to get the two names to add or
    // default values if the user has not set
    // these
    int get_active_array(
        const std::string &user_array,
        const dagr_metadata &input_md,
        std::string &active_array) const;

private:
    dagr_metadata get_output_metadata(
        unsigned int port,
        const std::vector<dagr_metadata> &input_md) override;

    std::vector<dagr_metadata> get_upstream_request(
        unsigned int port,
        const std::vector<dagr_metadata> &input_md,
        const dagr_metadata &request) override;

    const_p_dagr_dataset execute(
        unsigned int port,
        const std::vector<const_p_dagr_dataset> &input_data,
        const dagr_metadata &request) override;

private:
    std::string array_1;
    std::string array_2;
};

#endif

