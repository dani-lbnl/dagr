#ifndef dagr_programmable_algorithm_h
#define dagr_programmable_algorithm_h

#include "dagr_algorithm.h"
#include "dagr_metadata.h"
#include "dagr_dataset_fwd.h"
#include "dagr_programmable_algorithm_fwd.h"

/// an algorithm implemented with  user provided callbacks
/**
The user can provide a callback for each of the three phases
of pipeline execution. The number of input and output ports
can also be set for finters (1 or more inputs, 1 or more outputs)
sources, (no  inputs, 1 or more outputs), or sinks (1 or more
inputs, no outputs).

1) report phase. the report callback returns metadata
    describing data that can be produced. The report callback
    is optional. It's only needed if the algorithm will produce
    new data or transform metadata.

    the report callback must be callable with signature:
    dagr_metadata(unsigned int)

2) request phase. the request callback generates a vector
    of requests(metadata objects) that inform the upstream of
    what data to generate. The request callback is optional.
    It's only needed if the algorithm needs data from the
    upstream or transform metadata.

    the request callback must be callable with the signature:
    std::vector<dagr_metadata>(
        unsigned int,
        const std::vector<dagr_metadata> &,
        const dagr_metadata &)

3) execute phase. the execute callback is used to do useful
    work on incoming or outgoing data. Examples include
    generating new datasets, processing datasets, reading
    and writing data to/from disk, and so on. The execute
    callback is  optional.

    the execute callback must be callable with the signature:
    const_p_dagr_dataset(
        unsigned int, const std::vector<const_p_dagr_dataset> &,
        const dagr_metadata &)

see also:

set_number_of_input_connections
set_number_of_output_ports
set_report_callback
set_request_callback
set_execute_callback
*/
class dagr_programmable_algorithm : public dagr_algorithm
{
public:
    DAGR_ALGORITHM_STATIC_NEW(dagr_programmable_algorithm)
    ~dagr_programmable_algorithm();

    // set the number of input and outputs
    using dagr_algorithm::set_number_of_input_connections;
    using dagr_algorithm::set_number_of_output_ports;

    // install the default implementation
    void use_default_report_action();
    void use_default_request_action();
    void use_default_execute_action();

    // set callback that responds to reporting stage
    // of pipeline execution. the report callback must
    // be callable with signature:
    //
    //   dagr_metadata (unsigned int,
    //      const std::vector<dagr_metadata> &)
    //
    // the default implementation forwards downstream
    DAGR_ALGORITHM_CALLBACK_PROPERTY(
        report_callback_t, report_callback)

    // set the callback that responds to the requesting
    // stage of pipeline execution. the request callback
    // must be callable with the signature:
    //
    //  std::vector<dagr_metadata> (
    //    unsigned int,
    //    const std::vector<dagr_metadata> &,
    //    const dagr_metadata &)
    //
    // the default implementation forwards upstream
    DAGR_ALGORITHM_CALLBACK_PROPERTY(
        request_callback_t, request_callback)

    // set the callback that responds to the execution stage
    // of pipeline execution. the execute callback must be
    // callable with the signature:
    //
    //  const_p_dagr_dataset (
    //    unsigned int, const std::vector<const_p_dagr_dataset> &,
    //    const dagr_metadata &)
    //
    // the default implementation returns a nullptr
    DAGR_ALGORITHM_CALLBACK_PROPERTY(
        execute_callback_t, execute_callback)

protected:
    dagr_programmable_algorithm();

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
    report_callback_t report_callback;
    request_callback_t request_callback;
    execute_callback_t execute_callback;
};

#endif
