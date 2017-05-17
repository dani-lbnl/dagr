#include "dagr_threaded_algorithm.h"
#include "dagr_metadata.h"
#include "dagr_thread_pool.h"


#include <memory>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <future>
#include <cstdlib>

#if defined(DAGR_HAS_BOOST)
#include <boost/program_options.hpp>
#endif

// function that executes the data request and returns the
// requested dataset
class dagr_data_request
{
public:
    dagr_data_request(const p_dagr_algorithm &alg,
        const dagr_algorithm_output_port up_port,
        const dagr_metadata &up_req) : m_alg(alg),
        m_up_port(up_port), m_up_req(up_req)
    {}

    const_p_dagr_dataset operator()()
    { return m_alg->request_data(m_up_port, m_up_req); }

public:
    p_dagr_algorithm m_alg;
    dagr_algorithm_output_port m_up_port;
    dagr_metadata m_up_req;
};

// task
using dagr_data_request_task = std::packaged_task<const_p_dagr_dataset()>;

using dagr_data_request_queue =
    dagr_thread_pool<dagr_data_request_task, const_p_dagr_dataset>;

using p_dagr_data_request_queue = std::shared_ptr<dagr_data_request_queue>;

// internals for dagr threaded algorithm
class dagr_threaded_algorithm_internals
{
public:
    dagr_threaded_algorithm_internals() :
        thread_pool(new dagr_data_request_queue(-1, false, true, false))
     {}

    void thread_pool_resize(int n, bool local, bool bind, bool verbose);

    unsigned int get_thread_pool_size() const noexcept
    { return this->thread_pool->size(); }

public:
    p_dagr_data_request_queue thread_pool;
};

// --------------------------------------------------------------------------
void dagr_threaded_algorithm_internals::thread_pool_resize(int n, bool local,
    bool bind, bool verbose)
{
    this->thread_pool = std::make_shared<dagr_data_request_queue>(n,
        local, bind, verbose);
}




// --------------------------------------------------------------------------
dagr_threaded_algorithm::dagr_threaded_algorithm() : verbose(0),
    bind_threads(1), internals(new dagr_threaded_algorithm_internals)
{
}

// --------------------------------------------------------------------------
dagr_threaded_algorithm::~dagr_threaded_algorithm() noexcept
{
    delete this->internals;
}

#if defined(DAGR_HAS_BOOST)
// --------------------------------------------------------------------------
void dagr_threaded_algorithm::get_properties_description(
    const std::string &prefix, options_description &global_opts)
{
    options_description opts("Options for "
        + (prefix.empty()?"dagr_threaded_algorithm":prefix));

    opts.add_options()
        DAGR_POPTS_GET(int, prefix, bind_threads,
            "bind software threads to hardware cores (1)")
        DAGR_POPTS_GET(int, prefix, verbose,
            "print a run time report of settings (0)")
        DAGR_POPTS_GET(int, prefix, thread_pool_size,
            "number of threads in pool. When n == -1, 1 thread per core is created (-1)")
        ;

    global_opts.add(opts);
}

// --------------------------------------------------------------------------
void dagr_threaded_algorithm::set_properties(const std::string &prefix,
    variables_map &opts)
{
    DAGR_POPTS_SET(opts, int, prefix, bind_threads)
    DAGR_POPTS_SET(opts, int, prefix, verbose)

    std::string opt_name = (prefix.empty()?"":prefix+"::") + "thread_pool_size";
    if (opts.count(opt_name))
        this->set_thread_pool_size(opts[opt_name].as<int>());
}
#endif

// --------------------------------------------------------------------------
void dagr_threaded_algorithm::set_thread_pool_size(int n)
{
    this->internals->thread_pool_resize(n, false, this->bind_threads, this->verbose);
}

// --------------------------------------------------------------------------
unsigned int dagr_threaded_algorithm::get_thread_pool_size() const noexcept
{
    return this->internals->get_thread_pool_size();
}

// --------------------------------------------------------------------------
const_p_dagr_dataset dagr_threaded_algorithm::request_data(
    dagr_algorithm_output_port &current,
    const dagr_metadata &request)
{
    // execute current algorithm to fulfill the request.
    // return the data
    p_dagr_algorithm alg = get_algorithm(current);
    unsigned int port = get_port(current);

    // check for cached data
    dagr_metadata key = alg->get_cache_key(port, request);
    const_p_dagr_dataset out_data = alg->get_output_data(port, key);
    if (!out_data)
    {
        // determine what data is available on our inputs
        unsigned int n_inputs = alg->get_number_of_input_connections();
        std::vector<dagr_metadata> input_md(n_inputs);
        for (unsigned int i = 0; i < n_inputs; ++i)
        {
            input_md[i]
              = alg->get_output_metadata(alg->get_input_connection(i));
        }

        // get requests for upstream data
        std::vector<dagr_metadata> up_reqs
            = alg->get_upstream_request(port, input_md, request);

        // push data requests on to the thread pool's work
        // queue. mapping the requests round-robbin on to
        // the inputs
        size_t n_up_reqs = up_reqs.size();

        for (unsigned int i = 0; i < n_up_reqs; ++i)
        {
            if (!up_reqs[i].empty())
            {
                dagr_algorithm_output_port &up_port
                    = alg->get_input_connection(i%n_inputs);

                dagr_data_request dreq(get_algorithm(up_port), up_port, up_reqs[i]);
                dagr_data_request_task task(dreq);

                this->internals->thread_pool->push_task(task);
            }
        }

        // get the requested data. will block until it's ready.
        std::vector<const_p_dagr_dataset> input_data;
        this->internals->thread_pool->wait_data(input_data);

        // execute override
        out_data = alg->execute(port, input_data, request);

        // cache the output
        alg->cache_output_data(port, key, out_data);
    }

    return out_data;
}
