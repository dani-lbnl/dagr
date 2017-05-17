#ifndef dagr_py_array_h
#define dagr_py_array_h

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/arrayobject.h>
#include <Python.h>
#include <cstdlib>

#include "dagr_common.h"
#include "dagr_variant_array.h"

namespace dagr_py_array
{
/// cpp_tt -- traits class for working with PyArrayObject's
/**
cpp_tt::type -- get the C++ type given a numpy enum.

CODE -- numpy type enumeration
CPP_T -- corresponding C++ type
*/
template <int numpy_code> struct cpp_tt
{};

#define dagr_py_array_cpp_tt_declare(CODE, CPP_T)   \
template <> struct cpp_tt<CODE>                     \
{                                                   \
    typedef CPP_T type;                             \
};
dagr_py_array_cpp_tt_declare(NPY_BYTE, char)
dagr_py_array_cpp_tt_declare(NPY_INT32, int)
dagr_py_array_cpp_tt_declare(NPY_INT64, long long)
dagr_py_array_cpp_tt_declare(NPY_UBYTE, unsigned char)
dagr_py_array_cpp_tt_declare(NPY_UINT32, unsigned int)
dagr_py_array_cpp_tt_declare(NPY_UINT64, unsigned long long)
dagr_py_array_cpp_tt_declare(NPY_FLOAT, float)
dagr_py_array_cpp_tt_declare(NPY_DOUBLE, double)


/// numpy_tt - traits class for working with PyArrayObject's
/**
::code - get the numpy type enum given a C++ type.
::is_type - return true if the PyArrayObject has the given type

CODE -- numpy type enumeration
CPP_T -- corresponding C++ type
*/
template <typename cpp_t> struct numpy_tt
{};

#define dagr_py_array_numpy_tt_declare(CODE, CPP_T) \
template <> struct numpy_tt<CPP_T>                  \
{                                                   \
    enum { code = CODE };                           \
    static bool is_type(PyArrayObject *arr)         \
    { return PyArray_TYPE(arr) == CODE; }           \
};
dagr_py_array_numpy_tt_declare(NPY_BYTE, char)
dagr_py_array_numpy_tt_declare(NPY_INT16, short)
dagr_py_array_numpy_tt_declare(NPY_INT32, int)
dagr_py_array_numpy_tt_declare(NPY_LONG, long)
dagr_py_array_numpy_tt_declare(NPY_INT64, long long)
dagr_py_array_numpy_tt_declare(NPY_UBYTE, unsigned char)
dagr_py_array_numpy_tt_declare(NPY_UINT16, unsigned short)
dagr_py_array_numpy_tt_declare(NPY_UINT32, unsigned int)
dagr_py_array_numpy_tt_declare(NPY_ULONG, unsigned long)
dagr_py_array_numpy_tt_declare(NPY_UINT64, unsigned long long)
dagr_py_array_numpy_tt_declare(NPY_FLOAT, float)
dagr_py_array_numpy_tt_declare(NPY_DOUBLE, double)


// CPP_T - array type to match
// OBJ - PyArrayObject* instance
// CODE - code to execute on match
#define DAGR_PY_ARRAY_DISPATCH_CASE(CPP_T, OBJ, CODE)   \
    if (dagr_py_array::numpy_tt<CPP_T>::is_type(OBJ))   \
    {                                                   \
        using AT = CPP_T;                               \
        CODE                                            \
    }

#define DAGR_PY_ARRAY_DISPATCH(OBJ, CODE)                       \
    DAGR_PY_ARRAY_DISPATCH_CASE(float, OBJ, CODE)               \
    DAGR_PY_ARRAY_DISPATCH_CASE(double, OBJ, CODE)              \
    DAGR_PY_ARRAY_DISPATCH_CASE(int, OBJ, CODE)                 \
    DAGR_PY_ARRAY_DISPATCH_CASE(unsigned int, OBJ, CODE)        \
    DAGR_PY_ARRAY_DISPATCH_CASE(long, OBJ, CODE)                \
    DAGR_PY_ARRAY_DISPATCH_CASE(unsigned long, OBJ, CODE)       \
    DAGR_PY_ARRAY_DISPATCH_CASE(long long, OBJ, CODE)           \
    DAGR_PY_ARRAY_DISPATCH_CASE(unsigned long long, OBJ, CODE)  \
    DAGR_PY_ARRAY_DISPATCH_CASE(char, OBJ, CODE)                \
    DAGR_PY_ARRAY_DISPATCH_CASE(unsigned char, OBJ, CODE)

// ****************************************************************************
bool append(dagr_variant_array *varr, PyObject *obj)
{
    // not an array
    if (!PyArray_Check(obj))
        return false;

    PyArrayObject *arr = reinterpret_cast<PyArrayObject*>(obj);

    // nothing to do.
    size_t n_elem = PyArray_SIZE(arr);
    if (!n_elem)
        return true;

    // append
    TEMPLATE_DISPATCH(dagr_variant_array_impl, varr,
        TT *varrt = static_cast<TT*>(varr);
        varrt->reserve(n_elem);

        DAGR_PY_ARRAY_DISPATCH(arr,
            NpyIter *it = NpyIter_New(arr, NPY_ITER_READONLY,
                    NPY_KEEPORDER, NPY_NO_CASTING, nullptr);
            NpyIter_IterNextFunc *next = NpyIter_GetIterNext(it, nullptr);
            AT **ptrptr = reinterpret_cast<AT**>(NpyIter_GetDataPtrArray(it));
            do
            {
                varrt->append(**ptrptr);
            }
            while (next(it));
            NpyIter_Deallocate(it);
            return true;
            )
        )

    // unknown type
    return false;
}

// ****************************************************************************
bool copy(dagr_variant_array *varr, PyObject *obj)
{
    // not an array
    if (!PyArray_Check(obj))
        return false;

    PyArrayObject *arr = reinterpret_cast<PyArrayObject*>(obj);

    // nothing to do.
    size_t n_elem = PyArray_SIZE(arr);
    if (!n_elem)
        return true;

    // copy
    TEMPLATE_DISPATCH(dagr_variant_array_impl, varr,

        TT *varrt = static_cast<TT*>(varr);
        varrt->resize(0);
        varrt->reserve(n_elem);

        DAGR_PY_ARRAY_DISPATCH(arr,
            NpyIter *it = NpyIter_New(arr, NPY_ITER_READONLY,
                    NPY_KEEPORDER, NPY_NO_CASTING, nullptr);
            NpyIter_IterNextFunc *next = NpyIter_GetIterNext(it, nullptr);
            AT **ptrptr = reinterpret_cast<AT**>(NpyIter_GetDataPtrArray(it));
            do
            {
                varrt->append(**ptrptr);
            }
            while (next(it));
            NpyIter_Deallocate(it);
            return true;
            )
        )

    // unknown type
    return false;
}

// ****************************************************************************
p_dagr_variant_array new_variant_array(PyObject *obj)
{
    // not an array
    if (!PyArray_Check(obj))
        return nullptr;

    PyArrayObject *arr = reinterpret_cast<PyArrayObject*>(obj);

    // allocate and copy
    DAGR_PY_ARRAY_DISPATCH(arr,
        size_t n_elem = PyArray_SIZE(arr);

        p_dagr_variant_array_impl<AT> varrt
             = dagr_variant_array_impl<AT>::New();
        varrt->reserve(n_elem);

        NpyIter *it = NpyIter_New(arr, NPY_ITER_READONLY,
                NPY_KEEPORDER, NPY_NO_CASTING, nullptr);
        NpyIter_IterNextFunc *next = NpyIter_GetIterNext(it, nullptr);
        AT **ptrptr = reinterpret_cast<AT**>(NpyIter_GetDataPtrArray(it));
        do
        {
            varrt->append(**ptrptr);
        }
        while (next(it));
        NpyIter_Deallocate(it);

        return varrt;
        )

    // unknown type
    return nullptr;
}

// ****************************************************************************
template <typename NT>
PyArrayObject *new_object(dagr_variant_array_impl<NT> *varrt)
{
    // allocate a buffer
    npy_intp n_elem = varrt->size();
    size_t n_bytes = n_elem*sizeof(NT);
    NT *mem = static_cast<NT*>(malloc(n_bytes));
    if (!mem)
    {
        PyErr_Format(PyExc_RuntimeError,
            "failed to allocate %lu bytes", n_bytes);
        return nullptr;
    }

    // copy the data
    memcpy(mem, varrt->get(), n_bytes);

    // put the buffer in to a new numpy object
    PyArrayObject *arr = reinterpret_cast<PyArrayObject*>(
        PyArray_SimpleNewFromData(1, &n_elem, numpy_tt<NT>::code, mem));
    PyArray_ENABLEFLAGS(arr, NPY_ARRAY_OWNDATA);

    return arr;
}

// ****************************************************************************
PyArrayObject *new_object(dagr_variant_array *varr)
{
    TEMPLATE_DISPATCH(dagr_variant_array_impl, varr,
        TT *varrt = static_cast<TT*>(varr);
        return new_object(varrt);
        )
    return nullptr;
}
};

#endif
