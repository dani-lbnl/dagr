#ifndef dagr_py_object_h
#define dagr_py_object_h

#include "dagr_common.h"
#include "dagr_variant_array.h"
#include <Python.h>

namespace dagr_py_object
{
/// dagr_py_object::cpp_tt, A traits class for working with PyObject's
/**
if know the Python type tag then this class gives you:

::type -- C++ type that can hold the value of the PyObject.
::is_type -- returns true if the given PyObject has this type
::value -- convert given PyObject to its C++ type

Python type tags and their coresponding PyObject's are:
int --> PyInt, long --> PyLong, bool --> PyBool,
float --> PyFloat, char* --> PyString
*/
template <typename py_t> struct cpp_tt
{};

/*
PY_T -- C-name of python type
CPP_T -- underlying type needed to store it on the C++ side
PY_CHECK -- function that verifies the PyObject is this type
PY_AS_CPP -- function that converts to the C++ type */
#define dagr_py_object_cpp_tt_declare(PY_T, CPP_T, PY_CHECK, PY_AS_CPP) \
template <> struct cpp_tt<PY_T>                                         \
{                                                                       \
    typedef CPP_T type;                                                 \
    static bool is_type(PyObject *obj) { return PY_CHECK(obj); }        \
    static type value(PyObject *obj) { return PY_AS_CPP(obj); }         \
};
dagr_py_object_cpp_tt_declare(int, long, PyInt_Check, PyInt_AsLong)
dagr_py_object_cpp_tt_declare(long, long, PyLong_Check, PyLong_AsLong)
dagr_py_object_cpp_tt_declare(float, double, PyFloat_Check, PyFloat_AsDouble)
dagr_py_object_cpp_tt_declare(char*, std::string, PyString_Check, PyString_AsString)
dagr_py_object_cpp_tt_declare(bool, int, PyBool_Check, PyInt_AsLong)

/// py_tt, traits class for working with PyObject's
/**
if you know the C++ type then this class gives you:

::tag -- Use this in dagr_py_object::cpp_t to find
         the PyObject indentification and conversion
         methods. see example below.

::new_object -- copy construct a new PyObject

here is an example of looking up the PyObject conversion
function(value) from a known C++ type (float).

float val = cpp_tt<py_tt<float>::tag>::value(obj);

py_tt is used to take a C++ type and lookup the Python type
tag. Then the type tag is used to lookup the function.
*/
template <typename type> struct py_tt
{};

/**
CPP_T -- underlying type needed to store it on the C++ side
CPP_AS_PY -- function that converts from the C++ type */
#define dagr_py_object_py_tt_declare(CPP_T, PY_T, CPP_AS_PY)\
template <> struct py_tt<CPP_T>                             \
{                                                           \
    typedef PY_T tag;                                       \
    static PyObject *new_object(CPP_T val)                  \
    { return CPP_AS_PY(val); }                              \
};
dagr_py_object_py_tt_declare(char, int, PyInt_FromLong)
dagr_py_object_py_tt_declare(short, int, PyInt_FromLong)
dagr_py_object_py_tt_declare(int, int, PyInt_FromLong)
dagr_py_object_py_tt_declare(long, int, PyInt_FromLong)
dagr_py_object_py_tt_declare(long long, int, PyInt_FromSsize_t)
dagr_py_object_py_tt_declare(unsigned char, int, PyInt_FromSize_t)
dagr_py_object_py_tt_declare(unsigned short, int, PyInt_FromSize_t)
dagr_py_object_py_tt_declare(unsigned int, int, PyInt_FromSize_t)
dagr_py_object_py_tt_declare(unsigned long, int, PyInt_FromSize_t)
dagr_py_object_py_tt_declare(unsigned long long, int, PyInt_FromSize_t)
dagr_py_object_py_tt_declare(float, float, PyFloat_FromDouble)
dagr_py_object_py_tt_declare(double, float, PyFloat_FromDouble)
// strings are a special case
template <> struct py_tt<std::string>
{
    typedef char* tag;
    static PyObject *new_object(const std::string &s)
    { return PyString_FromString(s.c_str()); }
};
// TODO -- special case for dagr_metadata


// dispatch macro.
// OBJ -- PyObject* instance
// CODE -- code block to execute on match
// OT -- a typedef to the match type available in
//       the code block
#define DAGR_PY_OBJECT_DISPATCH_CASE(CPP_T, PY_OBJ, CODE)   \
    if (dagr_py_object::cpp_tt<CPP_T>::is_type(PY_OBJ))     \
    {                                                       \
        using OT = CPP_T;                                   \
        CODE                                                \
    }

#define DAGR_PY_OBJECT_DISPATCH(PY_OBJ, CODE)               \
    DAGR_PY_OBJECT_DISPATCH_CASE(int, PY_OBJ, CODE)         \
    else DAGR_PY_OBJECT_DISPATCH_CASE(float, PY_OBJ, CODE)  \
    else DAGR_PY_OBJECT_DISPATCH_CASE(char*, PY_OBJ, CODE)  \
    else DAGR_PY_OBJECT_DISPATCH_CASE(long, PY_OBJ, CODE)

// without string
#define DAGR_PY_OBJECT_DISPATCH_NUM(PY_OBJ, CODE)           \
    DAGR_PY_OBJECT_DISPATCH_CASE(int, PY_OBJ, CODE)         \
    else DAGR_PY_OBJECT_DISPATCH_CASE(float, PY_OBJ, CODE)  \
    else DAGR_PY_OBJECT_DISPATCH_CASE(long, PY_OBJ, CODE)

// just string
#define DAGR_PY_OBJECT_DISPATCH_STR(PY_OBJ, CODE)           \
    DAGR_PY_OBJECT_DISPATCH_CASE(char*, PY_OBJ, CODE)

// ****************************************************************************
p_dagr_variant_array new_variant_array(PyObject *obj)
{
    DAGR_PY_OBJECT_DISPATCH(obj,

        p_dagr_variant_array_impl<typename cpp_tt<OT>::type> varr
            = dagr_variant_array_impl<typename cpp_tt<OT>::type>::New(1);

        varr->set(0, cpp_tt<OT>::value(obj));

        return varr;
        )

    return nullptr;
}

// ****************************************************************************
bool copy(dagr_variant_array *varr, PyObject *obj)
{
    TEMPLATE_DISPATCH(dagr_variant_array_impl, varr,
        TT *varrt = static_cast<TT*>(varr);
        DAGR_PY_OBJECT_DISPATCH_NUM(obj,
            varrt->resize(1);
            varrt->set(0, cpp_tt<OT>::value(obj));
            return true;
            )
        )
    else TEMPLATE_DISPATCH_CASE(dagr_variant_array_impl,
        std::string, varr,
        TT *varrt = static_cast<TT*>(varr);
        DAGR_PY_OBJECT_DISPATCH_STR(obj,
            varrt->resize(1);
            varrt->set(0, cpp_tt<OT>::value(obj));
            return true;
            )
        )

    return false;
}

// ****************************************************************************
bool set(dagr_variant_array *varr, unsigned long i, PyObject *obj)
{
    TEMPLATE_DISPATCH(dagr_variant_array_impl, varr,
        TT *varrt = static_cast<TT*>(varr);
        DAGR_PY_OBJECT_DISPATCH_NUM(obj,
            varrt->set(i, cpp_tt<OT>::value(obj));
            return true;
            )
        )
    else TEMPLATE_DISPATCH_CASE(dagr_variant_array_impl,
        std::string, varr,
        TT *varrt = static_cast<TT*>(varr);
        DAGR_PY_OBJECT_DISPATCH_STR(obj,
            varrt->set(i, cpp_tt<OT>::value(obj));
            return true;
            )
        )

    return false;
}

// ****************************************************************************
bool append(dagr_variant_array *varr, PyObject *obj)
{
    TEMPLATE_DISPATCH(dagr_variant_array_impl, varr,
        TT *varrt = static_cast<TT*>(varr);
        DAGR_PY_OBJECT_DISPATCH_NUM(obj,
            varrt->append(static_cast<NT>(cpp_tt<OT>::value(obj)));
            return true;
            )
        )
    else TEMPLATE_DISPATCH_CASE(dagr_variant_array_impl,
        std::string, varr,
        TT *varrt = static_cast<TT*>(varr);
        DAGR_PY_OBJECT_DISPATCH_STR(obj,
            varrt->append(static_cast<NT>(cpp_tt<OT>::value(obj)));
            return true;
            )
         )
    return false;
}

// container that keeps a reference to a PyObject
class dagr_py_object_ptr
{
public:
    dagr_py_object_ptr() : m_obj(nullptr) {}

    dagr_py_object_ptr(PyObject *obj)
        : m_obj(obj) { Py_XINCREF(m_obj); }

     virtual ~dagr_py_object_ptr() { Py_XDECREF(m_obj); }

     dagr_py_object_ptr(dagr_py_object_ptr &&o)
        : m_obj(o.m_obj) { o.m_obj = nullptr; }

     dagr_py_object_ptr &operator=(dagr_py_object_ptr &&o)
     {
         PyObject *tmp = m_obj;
         m_obj = o.m_obj;
         o.m_obj = tmp;
         return *this;
     }

     dagr_py_object_ptr(const dagr_py_object_ptr &o)
        : m_obj(o.m_obj) { Py_XINCREF(m_obj); }

     dagr_py_object_ptr &operator=(const dagr_py_object_ptr &o)
     {
         Py_XINCREF(o.m_obj);
         Py_XDECREF(m_obj);
         m_obj = o.m_obj;
         return *this;
     }

     explicit operator bool () const
     { return m_obj != nullptr; }

    PyObject *get_object(){ return m_obj; }

    virtual void set_object(PyObject *obj)
    {
        Py_XINCREF(obj);
        Py_XDECREF(m_obj);
        m_obj = obj;
    }

private:
    PyObject *m_obj;
};

class dagr_py_callable : public dagr_py_object_ptr
{
public:
    dagr_py_callable() : dagr_py_object_ptr() {}

    virtual ~dagr_py_callable()
    { this->dagr_py_object_ptr::set_object(nullptr); }

    dagr_py_callable(PyObject *f) : dagr_py_object_ptr()
    { this->dagr_py_callable::set_object(f); }

    dagr_py_callable(const dagr_py_callable &&o)
        : dagr_py_object_ptr(std::move(o)) {}

    dagr_py_callable &operator=(const dagr_py_callable &&o)
    {
        this->dagr_py_object_ptr::operator=(std::move(o));
        return *this;
    }

    dagr_py_callable(const dagr_py_callable &o)
        : dagr_py_object_ptr(o) {}

    dagr_py_callable &operator=(const dagr_py_callable &o)
    {
        this->dagr_py_object_ptr::operator=(o);
        return *this;
    }

    virtual void set_object(PyObject *f)
    {
        if (PyCallable_Check(f))
            this->dagr_py_object_ptr::set_object(f);
        else
            PyErr_Format(PyExc_TypeError,
                "object is not callable");
    }
};

};

#endif
