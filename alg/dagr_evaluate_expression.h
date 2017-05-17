#ifndef dagr_evaluate_expression_h
#define dagr_evaluate_expression_h

#include "dagr_shared_object.h"
#include "dagr_algorithm.h"
#include "dagr_metadata.h"

#include <string>
#include <vector>

DAGR_SHARED_OBJECT_FORWARD_DECL(dagr_evaluate_expression)

/**
An algorithm that evaluates an expression stores the
result in a new variable.

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
class dagr_evaluate_expression : public dagr_algorithm
{
public:
    DAGR_ALGORITHM_STATIC_NEW(dagr_evaluate_expression)
    ~dagr_evaluate_expression();

    // report/initialize to/from Boost program options
    // objects.
    DAGR_GET_ALGORITHM_PROPERTIES_DESCRIPTION()
    DAGR_SET_ALGORITHM_PROPERTIES()

    // set/get the expression to evaluate
    void set_expression(const std::string &expr);

    std::string get_expression()
    { return this->expression; }

    // set the name of the variable to store the result in
    DAGR_ALGORITHM_PROPERTY(std::string, result_variable);

    // when set columns used in the calculation are removed
    // from the output. deault off.
    DAGR_ALGORITHM_PROPERTY(int, remove_dependent_variables)

protected:
    dagr_evaluate_expression();

private:
    dagr_metadata get_output_metadata(unsigned int port,
        const std::vector<dagr_metadata> &input_md) override;

    std::vector<dagr_metadata> get_upstream_request(
        unsigned int port, const std::vector<dagr_metadata> &input_md,
        const dagr_metadata &request) override;

    const_p_dagr_dataset execute(unsigned int port,
        const std::vector<const_p_dagr_dataset> &input_data,
        const dagr_metadata &request) override;

private:
    std::string expression;
    std::string result_variable;
    std::string postfix_expression;
    std::set<std::string> dependent_variables;
    int remove_dependent_variables;
};

#endif
