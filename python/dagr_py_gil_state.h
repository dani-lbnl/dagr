#ifndef dagr_py_gil_state_h
#define dagr_py_gil_state_h

#include <Python.h>

// RAII helper for managing the Python GIL
class dagr_py_gil_state
{
public:
    dagr_py_gil_state()
    { m_state = PyGILState_Ensure(); }

    ~dagr_py_gil_state()
    { PyGILState_Release(m_state); }

    dagr_py_gil_state(const dagr_py_gil_state&) = delete;
    void operator=(const dagr_py_gil_state&) = delete;

private:
    PyGILState_STATE m_state;
};

#endif
