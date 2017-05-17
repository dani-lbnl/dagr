#ifndef dagr_table_remove_rows_h
#define dagr_table_remove_rows_h

#include "dagr_shared_object.h"
#include "dagr_algorithm.h"
#include "dagr_metadata.h"

#include <string>
#include <vector>

DAGR_SHARED_OBJECT_FORWARD_DECL(dagr_table_remove_rows)

/**
an algorithm that removes rows from a table where
a given expression evaluates to true.

the expression parser supports the following operations:
    +,-,*,/,%,<.<=,>,>=,==,!=,&&,||.!,?

grouping in the expression is denoted in the usual
way: ()

constants in the expression are expanded to full length
arrays and can be typed. The supported types are:
    d,f,L,l,i,s,c
coresponding to double,float,long long, long, int,
short and char repsectively.  integer types can be
unsigned by including u after the code.
*/
class dagr_table_remove_rows : public dagr_algorithm
{
public:
    DAGR_ALGORITHM_STATIC_NEW(dagr_table_remove_rows)
    ~dagr_table_remove_rows();

    // report/initialize to/from Boost program options
    // objects.
    DAGR_GET_ALGORITHM_PROPERTIES_DESCRIPTION()
    DAGR_SET_ALGORITHM_PROPERTIES()

    // set the expression to use to determine which rows
    // are removed. rows are removed where the expression
    // evaluates true.
    void set_mask_expression(const std::string &expr);

    std::string get_mask_expression()
    { return this->mask_expression; }

    // when set columns used in the calculation are removed
    // from the output. deault off.
    DAGR_ALGORITHM_PROPERTY(int, remove_dependent_variables)

protected:
    dagr_table_remove_rows();

private:
    const_p_dagr_dataset execute(unsigned int port,
        const std::vector<const_p_dagr_dataset> &input_data,
        const dagr_metadata &request) override;

private:
    std::string mask_expression;
    std::string postfix_expression;
    std::set<std::string> dependent_variables;
    int remove_dependent_variables;
};

#endif
