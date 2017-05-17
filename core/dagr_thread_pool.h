#ifndef dagr_thread_pool_h
#define dagr_thread_pool_h

#include "dagr_common.h"
#include "dagr_algorithm_fwd.h"
#include "dagr_threadsafe_queue.h"

#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <future>
#if defined(_GNU_SOURCE)
#include <pthread.h>
#include <sched.h>
#include <cstring>
#include <deque>
#include <utility>
#include <algorithm>
#include <cstdint>
#include <sstream>
#include <iomanip>
#if defined(DAGR_HAS_MPI)
#include <mpi.h>
#endif
#endif

namespace internal
{
#if defined(_GNU_SOURCE)
struct closest_core
{
    closest_core(int base_id, int n_cores)
        : m_base_id(base_id%n_cores) {}

    int operator()(int qq) const
    { return std::abs(m_base_id-qq); }

    int m_base_id;
};

struct closest_hyperthread
{
    closest_hyperthread(int base_id, int n_cores)
        : m_base_id(base_id%n_cores), m_n_cores(n_cores) {}

    int operator()(int qq) const
    { return std::abs(m_base_id-(qq%m_n_cores)); }

    int m_base_id;
    int m_n_cores;
};

struct least_used_hyperthread
{
    least_used_hyperthread(int *hyperthreads) :
        m_hyperthreads(hyperthreads) {}

    int operator()(int qq) const
    { return m_hyperthreads[qq]; }

    int *m_hyperthreads;
};

// **************************************************************************
int select(int n_slots, int *slots, bool any_slot,
    const std::function<int(int)> &dist_to)
{
    // scan for empy core, compute the distance, select the closest
    int q = std::numeric_limits<int>::max();
    int d = std::numeric_limits<int>::max();
    for (int qq = 0; qq < n_slots; ++qq)
    {
        if (any_slot || !slots[qq])
        {
            // this core is empty, prefer the closest
            int dd = dist_to(qq);
            if (dd <= d)
            {
                d = dd;
                q = qq;
            }
        }

    }
    return q;
}

// **************************************************************************
int cpuid(uint64_t leaf, uint64_t level, uint64_t& ra, uint64_t& rb,
    uint64_t& rc, uint64_t& rd)
{
#if !defined(_WIN32)
    asm volatile("cpuid\n"
                 : "=a"(ra), "=b"(rb), "=c"(rc), "=d"(rd)
                 : "a"(leaf), "c"(level)
                 : "cc" );
    return 0;
#else
    return -1;
#endif
}

// **************************************************************************
int detect_cpu_topology(int &n_threads, int &n_threads_per_core)
{
    // TODO: to do this correctly we need to detect number of chips per
    // board, and if hyperthreading has been enabled. for more info:
    // https://software.intel.com/en-us/articles/intel-64-architecture-processor-topology-enumeration/
    // see specifically table A3.

    // defaults should cpuid fail on this platform. hyperthreads are
    // treated as cores. this will lead to poor performance but without
    // cpuid we can't distinguish physical cores from hyperthreads.
    n_threads = std::thread::hardware_concurrency();
    n_threads_per_core = 1;

    // check if topology leaf is supported on this processor.
    uint64_t ra = 0, rb = 0, rc = 0, rd = 0;
    if (internal::cpuid(0, 0, ra, rb, rc, rd) || (ra < 0xb))
        return -1;

    // this is all Intel specific, AMD uses a different leaf in cpuid
    // rax=0xb, rcx=i  get's the topology leaf level i
    uint64_t level = 0;
    do
    {
        internal::cpuid(0xb, level, ra, rb, rc, rd);
        n_threads = ((rc&0x0ff00) == 0x200) ? (0xffff&rb) : n_threads;
        n_threads_per_core = ((rc&0x0ff00) == 0x100) ? (0xffff&rb) : n_threads_per_core;
        level += 1;
    }
    while ((0xff00&rc) && (level < 16));

    // this should ever occur on intel cpu.
    if (level == 16)
        return -1;

    return 0;
}

// **************************************************************************
int generate_report(bool local, int local_proc, int base_id,
    const std::deque<int> &afin)
{
#if !defined(DAGR_HAS_MPI)
    (void)local;
#endif
    int rank = 0;
    int n_ranks = 1;
#if defined(DAGR_HAS_MPI)
    if (!local)
    {
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &n_ranks);
    }
#endif

    // gather proc ids
    std::vector<int> local_procs;
    if (rank == 0)
    {
        local_procs.resize(n_ranks);
        local_procs[0] = local_proc;
#if defined(DAGR_HAS_MPI)
        if (!local)
        {
            MPI_Gather(MPI_IN_PLACE, 0, MPI_DATATYPE_NULL, local_procs.data(),
                1, MPI_INT, 0, MPI_COMM_WORLD);
        }
    }
    else if (!local)
    {
        MPI_Gather(&local_proc, 1, MPI_INT, nullptr,
            0, MPI_DATATYPE_NULL, 0, MPI_COMM_WORLD);
#endif
    }

    // gather base core ids
    std::vector<int> base_ids;
    if (rank == 0)
    {
        base_ids.resize(n_ranks);
        base_ids[0] = base_id;
#if defined(DAGR_HAS_MPI)
        if (!local)
        {
            MPI_Gather(MPI_IN_PLACE, 1, MPI_INT, base_ids.data(),
                1, MPI_INT, 0, MPI_COMM_WORLD);
        }
    }
    else if (!local)
    {
        MPI_Gather(&base_id, 1, MPI_INT, nullptr,
            0, MPI_DATATYPE_NULL, 0, MPI_COMM_WORLD);
#endif
    }


    // gather host names
    std::vector<char> hosts;
    if (rank == 0)
    {
        hosts.resize(64*n_ranks);
        gethostname(hosts.data(), 64);
        hosts[63] = '\0';
#if defined(DAGR_HAS_MPI)
        if (!local)
        {
            MPI_Gather(MPI_IN_PLACE, 64, MPI_BYTE, hosts.data(),
                64, MPI_BYTE, 0, MPI_COMM_WORLD);
        }
    }
    else if (!local)
    {
        char host[64];
        gethostname(host, 64);
        host[63] = '\0';
        MPI_Gather(host, 64, MPI_BYTE, nullptr,
            0, MPI_DATATYPE_NULL, 0, MPI_COMM_WORLD);
#endif
    }

    // gather thread affinity map
    std::vector<int> recv_cnt;
    if (rank == 0)
    {
        recv_cnt.resize(n_ranks);
        recv_cnt[0] = afin.size();
#if defined(DAGR_HAS_MPI)
        if (!local)
        {
            MPI_Gather(MPI_IN_PLACE, 1, MPI_INT, recv_cnt.data(),
                1, MPI_INT, 0, MPI_COMM_WORLD);
        }
    }
    else if (!local)
    {
        int cnt = afin.size();
        MPI_Gather(&cnt, 1, MPI_INT, nullptr,
            0, MPI_DATATYPE_NULL, 0, MPI_COMM_WORLD);
#endif
    }

    std::vector<int> afins;
    std::vector<int> displ;
    if (rank == 0)
    {
        int accum = 0;
        displ.resize(n_ranks);
        for (int i = 0; i < n_ranks; ++i)
        {
            displ[i] = accum;
            accum += recv_cnt[i];
        }
        afins.resize(accum);
        for (int i = 0; i < recv_cnt[0]; ++i)
            afins[i] = afin[i];
#if defined(DAGR_HAS_MPI)
        if (!local)
        {
            MPI_Gatherv(MPI_IN_PLACE, 0, MPI_DATATYPE_NULL, afins.data(),
                recv_cnt.data(), displ.data(), MPI_INT, 0, MPI_COMM_WORLD);
        }
    }
    else if (!local)
    {
        afins.assign(afin.begin(), afin.end());
        MPI_Gatherv(afins.data(), afins.size(), MPI_INT, nullptr,
            nullptr, nullptr, MPI_DATATYPE_NULL, 0, MPI_COMM_WORLD);
#endif
    }

    if (rank == 0)
    {
        std::ostringstream oss;
        for (int i = 0; i < n_ranks; ++i)
        {
            oss << std::setw(4) << std::right << i << " : " << &hosts[i*64] << " : "
                << std::setw(3) << std::right << local_procs[i] << "."
                << std::setw(3) << std::left << base_ids[i] << " : ";

            for (int j = 0; j < recv_cnt[i]; ++j)
            {
                oss << afins[displ[i]+j] << " ";
            }
            oss << (i<n_ranks-1 ? "\n" : "");
        }
        DAGR_STATUS("threadpool afinity:" << std::endl << oss.str())
    }

    return 0;
}

// **************************************************************************
int thread_parameters(int base_core_id, int n_req, bool local,
    bool bind, bool verbose, std::deque<int> &affinity)
{
    std::vector<int> base_core_ids;

    int n_threads = n_req;

    // get the number of MPI ranks on this node, and their core id's
    int n_procs = 1;
    int proc_id = 0;

    if (local)
    {
        base_core_ids.push_back(base_core_id);
    }
    else
    {
#if defined(DAGR_HAS_MPI)
        MPI_Comm comm;
        MPI_Comm_split_type(MPI_COMM_WORLD, MPI_COMM_TYPE_SHARED,
            0, MPI_INFO_NULL, &comm);

        MPI_Comm_size(comm, &n_procs);
        MPI_Comm_rank(comm, &proc_id);

        base_core_ids.resize(n_procs);
        base_core_ids[proc_id] = base_core_id;

        MPI_Allgather(MPI_IN_PLACE,0,MPI_DATATYPE_NULL,
            base_core_ids.data(), 1, MPI_UNSIGNED, comm);

        MPI_Comm_free(&comm);
#else
        base_core_ids.push_back(base_core_id);
#endif
    }

    // get the number of cores on this cpu
    int threads_per_chip = 1;
    int hw_threads_per_core = 1;
    if (internal::detect_cpu_topology(threads_per_chip, hw_threads_per_core))
    {
        DAGR_WARNING("failed to detect cpu topology. Assuming "
            << threads_per_chip/hw_threads_per_core << " physical cores.")
    }
    int threads_per_node = std::thread::hardware_concurrency();
    int cores_per_node = threads_per_node/hw_threads_per_core;

    // thread pool size is based on core and process count
    int nlg = 0;
    if (n_req > 0)
    {
        // user specified override
        n_threads = n_req;
    }
    else
    {
        // map threads to physical cores
        nlg = cores_per_node % n_procs;
        n_threads = cores_per_node/n_procs + (proc_id < nlg ? 1 : 0);
    }

    // stop now if we are not binding threads to cores
    if (!bind)
    {
        if (verbose)
            DAGR_STATUS("thread to core binding disabled")

        return n_threads;
    }

    // track which threads/cores are in use
    // using two maps, the first track use of cores, the second hyperthreads
    // the hyper thread map is 2d with core id on first dim and hyperthread id
    // on the second.
    long map_bytes = threads_per_node*sizeof(int);
    int *thread_use = static_cast<int*>(malloc(map_bytes));
    memset(thread_use, 0, map_bytes);

    map_bytes = cores_per_node*sizeof(int);
    int *core_use = static_cast<int*>(malloc(map_bytes));
    memset(core_use, 0, map_bytes);

    // there are enough cores that each thread can have it's own core
    // mark the cores which have the root thread as used so that we skip them.
    // if we always did this in the fully apcked case we'd always be assigning
    // hyperthreads off core. it is better to keep them local.
    if (((n_threads+1)*n_procs) < cores_per_node)
    {
        for (int i = 0; i < n_procs; ++i)
        {
            int bcid = base_core_ids[i];
            core_use[bcid%cores_per_node] = 1;
            thread_use[bcid] = 1;
        }
    }

    // mark resources used by other processes, up to and including this process.
    // also record the core ids we will bind to.
    for (int i = 0; i <= proc_id; ++i)
    {
        int proc_base = base_core_ids[i];
        int proc_n_threads = n_req > 0 ? n_req : cores_per_node/n_procs + (i < nlg ? 1 : 0);
        for (int j = 0; j < proc_n_threads; ++j)
        {
            // scan for empty core
            int q = select(cores_per_node, core_use, false,
                closest_core(proc_base, cores_per_node));

            if (q < cores_per_node)
            {
                // found one, mark core as used
                core_use[q] = 1;

                // now find an empty hyperthread on this core and
                // mark it as taken. if this is for us record the id
                // for later use
                int p = 0;
                while (thread_use[q+p*cores_per_node] && (p < hw_threads_per_core)) ++p;

                int pp = q+p*cores_per_node;

                // mark hyperthread as used
                thread_use[pp] = 1;

                // store the core id we will bind one of our threads to it
                if (i == proc_id)
                    affinity.push_back(pp);
            }
            else
            {
                // if we are here it means all the cores have at least one
                // hyperthread assigned. find the first empty hyperthread on
                // any core. if that fails then find the least used hyperthread.
                // if this for us record the id for later use
                int q = select(threads_per_node, thread_use, false,
                    closest_hyperthread(proc_base, cores_per_node));

                if (q >= threads_per_node)
                    q = select(threads_per_node, thread_use, true,
                        least_used_hyperthread(thread_use));

                if (q < threads_per_node)
                {
                    // found one. increment usage
                    thread_use[q] += 1;

                    // store the core id we will bind one of our threads to it
                    if (i == proc_id)
                        affinity.push_back(q);
                }
            }
        }
    }

    free(core_use);
    free(thread_use);

    if (verbose)
        generate_report(local, proc_id, base_core_id, affinity);

    return n_threads;
}
#endif
}



template <typename task_t, typename data_t>
class dagr_thread_pool;

template <typename task_t, typename data_t>
using p_dagr_thread_pool = std::shared_ptr<dagr_thread_pool<task_t, data_t>>;

// a class to manage a fixed size pool of threads that dispatch
// I/O work
template <typename task_t, typename data_t>
class dagr_thread_pool
{
public:
    dagr_thread_pool() = delete;

    // construct/destruct the thread pool.
    // arguments:
    //   n        number of threads to create for the pool. -1 will
    //            create 1 thread per physical CPU core. If local is false
    //            all MPI ranks running on the same node are taken into
    //            account, resulting in 1 thread per core node wide.
    //
    //   local    consider other MPI ranks on the node. This introduces
    //            MPI collective operations, so all ranks in comm world
    //            must call it.
    //
    //   bind     bind each thread to a specific core.
    //
    //   verbose  print a report of the thread to core bindings
    dagr_thread_pool(int n, bool local, bool bind, bool verbose);
    ~dagr_thread_pool() noexcept;

    // get rid of copy and asignment
    DAGR_ALGORITHM_DELETE_COPY_ASSIGN(dagr_thread_pool)

    // add a data request task to the queue, returns a future
    // from which the generated dataset can be accessed.
    void push_task(task_t &task);

    // wait for all of the requests to execute and transfer
    // datasets in the order that corresponding requests
    // were added to the queue.
    template <template <typename ... > class container_t, typename ... args>
    void wait_data(container_t<data_t, args ...> &data);

    // get the number of threads
    unsigned int size() const noexcept
    { return m_threads.size(); }

private:
    // create n threads for the pool
    void create_threads(int n_threads, bool local, bool bind, bool verbose);

private:
    std::atomic<bool> m_live;
    dagr_threadsafe_queue<task_t> m_queue;

    std::vector<std::future<data_t>>
        m_futures;

    std::vector<std::thread> m_threads;
};

// --------------------------------------------------------------------------
template <typename task_t, typename data_t>
dagr_thread_pool<task_t, data_t>::dagr_thread_pool(int n, bool local,
    bool bind, bool verbose) : m_live(true)
{
    this->create_threads(n, local, bind, verbose);
}

// --------------------------------------------------------------------------
template <typename task_t, typename data_t>
void dagr_thread_pool<task_t, data_t>::create_threads(int n, bool local,
    bool bind, bool verbose)
{
#if !defined(_GNU_SOURCE)
    (void)bind;
    (void)verbose;
    if (n < 1)
    {
        DAGR_WARNING("Cannot autmatically detect threading parameters "
            "on this platform. The default is 1 thread per process.")
        n = 1;
    }
    int n_threads = n;
#else
    int base_core_id = sched_getcpu();
    std::deque<int> core_ids;
    int n_threads = internal::thread_parameters
        (base_core_id, n, local, bind, verbose, core_ids);
#endif

    // allocate the threads
    for (int i = 0; i < n_threads; ++i)
    {
        m_threads.push_back(std::thread([this]()
        {
            // "main" for each thread in the pool
            while (m_live.load())
            {
                task_t task;
                if (m_queue.try_pop(task))
                    task();
                else
                    std::this_thread::yield();
            }
        }));
#if defined(_GNU_SOURCE)
        // bind each to a hyperthread
        if (bind)
        {
            int core_id = core_ids.front();
            core_ids.pop_front();

            cpu_set_t core_mask;
            CPU_ZERO(&core_mask);
            CPU_SET(core_id, &core_mask);

            if (pthread_setaffinity_np(m_threads[i].native_handle(),
                sizeof(cpu_set_t), &core_mask))
            {
                DAGR_WARNING("Failed to set thread affinity.")
            }
        }
#endif
    }
}

// --------------------------------------------------------------------------
template <typename task_t, typename data_t>
dagr_thread_pool<task_t, data_t>::~dagr_thread_pool() noexcept
{
    m_live = false;
    std::for_each(m_threads.begin(), m_threads.end(),
        [](std::thread &t) { t.join(); });
}

// --------------------------------------------------------------------------
template <typename task_t, typename data_t>
void dagr_thread_pool<task_t, data_t>::push_task(task_t &task)
{
    m_futures.push_back(task.get_future());
    m_queue.push(std::move(task));
}

// --------------------------------------------------------------------------
template <typename task_t, typename data_t>
template <template <typename ... > class container_t, typename ... args>
void dagr_thread_pool<task_t, data_t>::wait_data(container_t<data_t, args ...> &data)
{
    // wait on all pending requests and gather the generated
    // datasets
    std::for_each(m_futures.begin(), m_futures.end(),
        [&data] (std::future<data_t> &f)
        {
            data.push_back(f.get());
        });
    m_futures.clear();
}

#endif
