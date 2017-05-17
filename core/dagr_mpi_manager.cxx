#include "dagr_mpi_manager.h"
#include "dagr_config.h"
#include "dagr_common.h"

#include <cstdlib>

#if defined(DAGR_HAS_MPI)
#include <mpi.h>
#endif

// --------------------------------------------------------------------------
dagr_mpi_manager::dagr_mpi_manager(int &argc, char **&argv)
    : m_rank(0),  m_size(1)
{
#if defined(DAGR_HAS_MPI)
    int mpi_thread_required = MPI_THREAD_SERIALIZED;
    int mpi_thread_provided = 0;
    MPI_Init_thread(&argc, &argv, mpi_thread_required, &mpi_thread_provided);
    if (mpi_thread_provided < mpi_thread_required)
    {
        DAGR_ERROR("This MPI does not support thread serialized");
        abort();
    }
    MPI_Comm_rank(MPI_COMM_WORLD, &m_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &m_size);
#else
    (void)argc;
    (void)argv;
#endif
}

// --------------------------------------------------------------------------
dagr_mpi_manager::~dagr_mpi_manager()
{
#if defined(DAGR_HAS_MPI)
    int ok = 0;
    MPI_Initialized(&ok);
    if (ok)
        MPI_Finalize();
#endif
}
