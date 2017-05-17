#include "dagr_evaluate_expression.h"

#include "dagr_mesh.h"
#include "dagr_table.h"
#include "dagr_array_collection.h"
#include "dagr_variant_array.h"
#include "dagr_metadata.h"
#include "dagr_parser.h"
#include "dagr_variant_array_operator.h"
#include "dagr_variant_array_operand.h"

#include <iostream>
#include <string>
#include <set>
#include <sstream>

#if defined(DAGR_HAS_BOOST)
#include <boost/program_options.hpp>
#endif
#if defined(DAGR_HAS_UDUNITS)
#include "calcalcs.h"
#endif
#if defined(DAGR_HAS_MPI)
#include <mpi.h>
#endif

using std::cerr;
using std::endl;

using operator_resolver_t = dagr_variant_array_operator::resolver;
using operand_resolver_t = dagr_variant_array_operand::resolver;

// --------------------------------------------------------------------------
dagr_evaluate_expression::dagr_evaluate_expression() :
    remove_dependent_variables(0)
{
    this->set_number_of_input_connections(1);
    this->set_number_of_output_ports(1);
}

// --------------------------------------------------------------------------
dagr_evaluate_expression::~dagr_evaluate_expression()
{}

#if defined(DAGR_HAS_BOOST)
// --------------------------------------------------------------------------
void dagr_evaluate_expression::get_properties_description(
    const std::string &prefix, options_description &global_opts)
{
    options_description opts("Options for "
        + (prefix.empty()?"dagr_evaluate_expression":prefix));

    opts.add_options()
        DAGR_POPTS_GET(std::string, prefix, expression,
            "the expression to evaluate")
        DAGR_POPTS_GET(std::string, prefix, result_variable,
            "name of the variable to store the result in")
        DAGR_POPTS_GET(int, prefix, remove_dependent_variables,
            "when set columns used in the calculation are removed from output")
        ;

    global_opts.add(opts);
}

// --------------------------------------------------------------------------
void dagr_evaluate_expression::set_properties(
    const std::string &prefix, variables_map &opts)
{
    DAGR_POPTS_SET(opts, std::string, prefix, expression)
    DAGR_POPTS_SET(opts, std::string, prefix, result_variable)
    DAGR_POPTS_SET(opts, int, prefix, remove_dependent_variables)
}
#endif

// --------------------------------------------------------------------------
void dagr_evaluate_expression::set_expression(const std::string &expr)
{
    if (expr == this->expression)
        return;

    // convert the expression to postfix, doing so here let's
    // us know which variables will be needed
    std::set<std::string> dep_vars;
    char *pfix_expr = dagr_parser::infix_to_postfix(expr.c_str(), &dep_vars);
    if (!pfix_expr)
    {
        DAGR_ERROR("failed to convert \"" << expr << "\" to postfix")
        return;
    }

    this->expression = expr;
    this->postfix_expression = pfix_expr;
    this->dependent_variables = std::move(dep_vars);
    this->set_modified();

    free(pfix_expr);
}

// --------------------------------------------------------------------------
dagr_metadata dagr_evaluate_expression::get_output_metadata(unsigned int port,
    const std::vector<dagr_metadata> &input_md)
{
#ifdef DAGR_DEBUG
    cerr << dagr_parallel_id()
        << "dagr_evaluate_expression::get_output_metadata" << endl;
#endif
    (void)port;

    // add in the array we will generate
    dagr_metadata out_md(input_md[0]);
    out_md.append("variables", this->result_variable);

    return out_md;
}

// --------------------------------------------------------------------------
std::vector<dagr_metadata> dagr_evaluate_expression::get_upstream_request(
    unsigned int port, const std::vector<dagr_metadata> &input_md,
    const dagr_metadata &request)
{
    (void)port;
    (void)input_md;

    std::vector<dagr_metadata> up_reqs;

    // copy the incoming request to preserve the downstream
    // requirements and add the arrays we need
    dagr_metadata req(request);

    std::set<std::string> arrays;
    if (req.has("arrays"))
        req.get("arrays", arrays);

    arrays.insert(this->dependent_variables.begin(),
         this->dependent_variables.end());

    // capture the array we produce
    arrays.erase(this->result_variable);

    // update the request
    req.insert("arrays", arrays);

    // send it up
    up_reqs.push_back(req);
    return up_reqs;
}

// --------------------------------------------------------------------------
const_p_dagr_dataset dagr_evaluate_expression::execute(
    unsigned int port, const std::vector<const_p_dagr_dataset> &input_data,
    const dagr_metadata &request)
{
#ifdef DAGR_DEBUG
    cerr << dagr_parallel_id() << "dagr_evaluate_expression::execute" << endl;
#endif
    (void)port;
    (void)request;

    // in table based algorithms only rank 0 is required to have data
    // other ranks that don't have data exit here
    int rank = 0;
#if defined(DAGR_HAS_MPI)
    int init = 0;
    MPI_Initialized(&init);
    if (init)
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif
    if (!input_data[0])
    {
        if (rank == 0)
        {
            DAGR_ERROR("Input is empty or not a table")
        }
        return nullptr;
    }

    // shallow copy the input to output
    p_dagr_dataset output_data = input_data[0]->new_instance();

    output_data->shallow_copy(
        std::const_pointer_cast<dagr_dataset>(input_data[0]));

    // grab the aviable vairables from the output
    p_dagr_array_collection variables;
    if (std::dynamic_pointer_cast<dagr_table>(output_data))
        variables = static_cast<dagr_table*>(output_data.get())->get_columns();
    else if (std::dynamic_pointer_cast<dagr_mesh>(output_data))
        variables = static_cast<dagr_mesh*>(output_data.get())->get_point_arrays();

    if (!variables)
    {
        DAGR_ERROR("input was not a table nor a mesh")
        return nullptr;
    }

    // construct the data adaptor that serves up variables
    // and constants
    operand_resolver_t operand_resolver;
    operand_resolver.set_variables(variables);

    // verify that we have a valid expression
    if (this->postfix_expression.empty())
    {
        DAGR_ERROR("An expression was not provided.")
        return nullptr;
    }

    // evaluate the postfix expression
    const_p_dagr_variant_array result;
    if (dagr_parser::eval_postfix<p_dagr_variant_array,
        const_p_dagr_variant_array, operand_resolver_t,
        operator_resolver_t>(result, this->postfix_expression.c_str(),
            operand_resolver))
    {
        DAGR_ERROR("failed to evaluate the expression \""
            << this->expression << "\"")
        return nullptr;
    }

    // remove dependent variables
    if (this->remove_dependent_variables)
    {
        std::set<std::string>::iterator it = this->dependent_variables.begin();
        std::set<std::string>::iterator end = this->dependent_variables.end();
        for (; it != end; ++it)
            variables->remove(*it);
    }

    // verify that we have the name to store the result
    if (this->result_variable.empty())
    {
        DAGR_ERROR("A name for the result was not provided.")
        return nullptr;
    }

    // store the result
    variables->append(this->result_variable,
        std::const_pointer_cast<dagr_variant_array>(result));

    // return output
    return output_data;
}
