#ifndef dagr_py_iterator_h
#define dagr_py_iterator_h

#include "dagr_common.h"
#include "dagr_variant_array.h"
#include "dagr_py_object.h"
#include <Python.h>

// this macro is used to build up dispatchers
// PYT - type tag idnetifying the PyObject
// ITER - PySequence* instance
// CODE - code to execute on match
// ST - typedef coresponding to matching tag
#define DAGR_PY_ITERATOR_DISPATCH_CASE(PYT, ITER, CODE) \
    if (dagr_py_iterator::is_type<PYT>(ITER))           \
    {                                                   \
        using ST = PYT;                                 \
        CODE                                            \
    }

// the macro dispatches for all the Python types
#define DAGR_PY_ITERATOR_DISPATCH(ITER, CODE)               \
    DAGR_PY_ITERATOR_DISPATCH_CASE(bool, ITER, CODE)        \
    else DAGR_PY_ITERATOR_DISPATCH_CASE(int, ITER, CODE)    \
    else DAGR_PY_ITERATOR_DISPATCH_CASE(float, ITER, CODE)  \
    else DAGR_PY_ITERATOR_DISPATCH_CASE(char*, ITER, CODE)  \
    else DAGR_PY_ITERATOR_DISPATCH_CASE(long, ITER, CODE)

// this one just the numeric types
#define DAGR_PY_ITERATOR_DISPATCH_NUM(ITER, CODE)           \
    DAGR_PY_ITERATOR_DISPATCH_CASE(int, ITER, CODE)         \
    else DAGR_PY_ITERATOR_DISPATCH_CASE(float, ITER, CODE)  \
    else DAGR_PY_ITERATOR_DISPATCH_CASE(long, ITER, CODE)

// this one just strings
#define DAGR_PY_ITERATOR_DISPATCH_STR(ITER, CODE)   \
    DAGR_PY_ITERATOR_DISPATCH_CASE(char*, ITER, CODE)


namespace dagr_py_iterator
{

// ****************************************************************************
bool is_iterable(PyObject *obj)
{
    PyObject *iter = nullptr;
    if (PyString_Check(obj)|| !(iter = PyObject_GetIter(obj)))
    {
        PyErr_Clear();
        return false;
    }
    Py_DECREF(iter);
    return true;
}


// ****************************************************************************
template <typename py_t>
bool is_type(PyObject *obj)
{
    // all items must have same type and it must match
    // the requested type
    PyObject *iter = PyObject_GetIter(obj);
    PyObject *item = nullptr;
    size_t i = 0;
    while ((item = PyIter_Next(iter)))
    {
        if (!dagr_py_object::cpp_tt<py_t>::is_type(item))
        {
            if (i)
            {
                DAGR_ERROR("mixed types are not supported. "
                    " Failed at element " <<  i)
            }
            return false;
        }
        Py_DECREF(item);
        ++i;
    }
    Py_DECREF(iter);
    // type matches
    return true;
}

// ****************************************************************************
bool append(dagr_variant_array *va, PyObject *obj)
{
    // not a iterator
    if (!is_iterable(obj))
        return false;

    // append numeric types
    TEMPLATE_DISPATCH(dagr_variant_array_impl, va,
        TT *vat = static_cast<TT*>(va);
        DAGR_PY_ITERATOR_DISPATCH_NUM(obj,
            PyObject *iter = PyObject_GetIter(obj);
            PyObject *item = nullptr;
            while((item = PyIter_Next(iter)))
            {
                vat->append(dagr_py_object::cpp_tt<ST>::value(item));
                Py_DECREF(item);
            }
            Py_DECREF(iter);
            return true;
            )
        )

    // append strings
    else TEMPLATE_DISPATCH_CASE(dagr_variant_array_impl,
        std::string, va,
        TT *vat = static_cast<TT*>(va);
        DAGR_PY_ITERATOR_DISPATCH_STR(obj,
            PyObject *iter = PyObject_GetIter(obj);
            PyObject *item = nullptr;
            while((item = PyIter_Next(iter)))
            {
                vat->append(dagr_py_object::cpp_tt<ST>::value(item));
                Py_DECREF(item);
            }
            Py_DECREF(iter);
            return true;
            )
        )

    // unknown type
    return false;
}

// ****************************************************************************
bool copy(dagr_variant_array *va, PyObject *obj)
{
    // not a iterator
    if (!is_iterable(obj))
        return false;

    // copy numeric types
    TEMPLATE_DISPATCH(dagr_variant_array_impl, va,
        TT *vat = static_cast<TT*>(va);
        DAGR_PY_ITERATOR_DISPATCH_NUM(obj,
            vat->clear();
            PyObject *iter = PyObject_GetIter(obj);
            PyObject *item = nullptr;
            while((item = PyIter_Next(iter)))
            {
                vat->append(dagr_py_object::cpp_tt<ST>::value(item));
                Py_DECREF(item);
            }
            Py_DECREF(iter);
            return true;
            )
        )

    // copy strings
    else TEMPLATE_DISPATCH_CASE(dagr_variant_array_impl,
        std::string, va,
        TT *vat = static_cast<TT*>(va);
        DAGR_PY_ITERATOR_DISPATCH_STR(obj,
            vat->clear();
            PyObject *iter = PyObject_GetIter(obj);
            PyObject *item = nullptr;
            while((item = PyIter_Next(iter)))
            {
                vat->append(dagr_py_object::cpp_tt<ST>::value(item));
                Py_DECREF(item);
            }
            Py_DECREF(iter);
            return true;
            )
        )

    // unknown type
    return false;
}

// ****************************************************************************
p_dagr_variant_array new_variant_array(PyObject *obj)
{
    // not a iterator
    if (!is_iterable(obj))
        return nullptr;

    // copy into a new array
    DAGR_PY_ITERATOR_DISPATCH(obj,

        p_dagr_variant_array_impl<typename dagr_py_object::cpp_tt<ST>::type> vat
            = dagr_variant_array_impl<typename dagr_py_object::cpp_tt<ST>::type>::New();

        PyObject *iter = PyObject_GetIter(obj);
        PyObject *item = nullptr;
        while((item = PyIter_Next(iter)))
        {
            vat->append(dagr_py_object::cpp_tt<ST>::value(item));
            Py_DECREF(item);
        }
        Py_DECREF(iter);

        return vat;
        )

    // unknown type
    return nullptr;
}
};

#endif
