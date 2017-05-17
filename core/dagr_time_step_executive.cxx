#include "dagr_config.h"
#include "dagr_time_step_executive.h"

#include "dagr_common.h"

#include <string>
#include <iostream>
#include <utility>

using std::vector;
using std::string;
using std::cerr;
using std::endl;

#if defined(DAGR_HAS_MPI)
#include <mpi.h>
#endif

#define DAGR_TIME_STEP_EXECUTIVE_DEBUG

// --------------------------------------------------------------------------
dagr_time_step_executive::dagr_time_step_executive()
    : first_step(0), last_step(-1), stride(1)
{
}

// --------------------------------------------------------------------------
void dagr_time_step_executive::set_step(long s)
{
    this->first_step = std::max(0l, s);
    this->last_step = s;
}

// --------------------------------------------------------------------------
void dagr_time_step_executive::set_first_step(long s)
{
    this->first_step = std::max(0l, s);
}

// --------------------------------------------------------------------------
void dagr_time_step_executive::set_last_step(long s)
{
    this->last_step = s;
}

// --------------------------------------------------------------------------
void dagr_time_step_executive::set_stride(long s)
{
    this->stride = std::max(0l, s);
}

// --------------------------------------------------------------------------
void dagr_time_step_executive::set_extent(unsigned long *ext)
{
    this->set_extent({ext[0], ext[1], ext[2], ext[3], ext[4], ext[4]});
}

// --------------------------------------------------------------------------
void dagr_time_step_executive::set_extent(const std::vector<unsigned long> &ext)
{
    this->extent = ext;
}

// --------------------------------------------------------------------------
void dagr_time_step_executive::set_arrays(const std::vector<std::string> &v)
{
    this->arrays = v;
}

// --------------------------------------------------------------------------
int dagr_time_step_executive::initialize(const dagr_metadata &md)
{
    this->requests.clear();

    // locate available times
    long n_times = 1;
    if (md.get("number_of_time_steps", n_times))
    {
        DAGR_ERROR("metadata is missing \"number_of_time_steps\"")
        return -1;
    }

    // apply restriction
    long last
        = this->last_step >= 0 ? this->last_step : n_times - 1;

    long first
        = ((this->first_step >= 0) && (this->first_step <= last))
            ? this->first_step : 0;

    n_times = last - first + 1;

    // partition time across MPI ranks. each rank
    // will end up with a unique block of times
    // to process.
    size_t rank = 0;
    size_t n_ranks = 1;
#if defined(DAGR_HAS_MPI)
    int is_init = 0;
    MPI_Initialized(&is_init);
    if (is_init)
    {
        int tmp = 0;
        MPI_Comm_size(MPI_COMM_WORLD, &tmp);
        n_ranks = tmp;
        MPI_Comm_rank(MPI_COMM_WORLD, &tmp);
        rank = tmp;
    }
#endif
    size_t n_big_blocks = n_times%n_ranks;
    size_t block_size = 1;
    size_t block_start = 0;
    if (rank < n_big_blocks)
    {
        block_size = n_times/n_ranks + 1;
        block_start = block_size*rank;
    }
    else
    {
        block_size = n_times/n_ranks;
        block_start = block_size*rank + n_big_blocks;
    }

    // consrtuct base request
    dagr_metadata base_req;
    if (this->extent.empty())
    {
        vector<unsigned long> whole_extent(6, 0l);
        md.get("whole_extent", whole_extent);
        base_req.insert("extent", whole_extent);
    }
    else
        base_req.insert("extent", this->extent);
    base_req.insert("arrays", this->arrays);

    // apply the base request to local times.
    for (size_t i = 0; i < block_size; ++i)
    {
        size_t step = i + block_start + first;
        if ((step % this->stride) == 0)
        {
            this->requests.push_back(base_req);
            this->requests.back().insert("time_step", step);
        }
    }

#if defined(DAGR_TIME_STEP_EXECUTIVE_DEBUG)
    cerr << dagr_parallel_id()
        << " dagr_time_step_executive::initialize first="
        << this->first_step << " last=" << last_step << " stride="
        << this->stride << endl;
#endif

    return 0;
}

// --------------------------------------------------------------------------
dagr_metadata dagr_time_step_executive::get_next_request()
{
    dagr_metadata req;
    if (!this->requests.empty())
    {
        req = this->requests.back();
        this->requests.pop_back();

#if defined(DAGR_TIME_STEP_EXECUTIVE_DEBUG)
        vector<unsigned long> ext;
        req.get("extent", ext);

        unsigned long time_step;
        req.get("time_step", time_step);

        cerr << dagr_parallel_id()
            << " dagr_time_step_executive::get_next_request time_step="
            << time_step << " extent=" << ext[0] << ", " << ext[1] << ", "
            << ext[2] << ", " << ext[3] << ", " << ext[4] << ", " << ext[5]
            << endl;
#endif
    }

    return req;
}
