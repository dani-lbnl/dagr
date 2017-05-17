#ifndef dagr_mpi_manager_h
#define dagr_mpi_manager_h

/// A RAII class to ease MPI initalization and finalization
// MPI_Init is handled in the constructor, MPI_Finalize is
// handled in the destructor.
class dagr_mpi_manager
{
public:
    dagr_mpi_manager() = delete;
    dagr_mpi_manager(const dagr_mpi_manager &) = delete;
    void operator=(const dagr_mpi_manager &) = delete;

    dagr_mpi_manager(int &argc, char **&argv);
    ~dagr_mpi_manager();

    int get_comm_rank(){ return m_rank; }
    int get_comm_size(){ return m_size; }

private:
    int m_rank;
    int m_size;
};

#endif
