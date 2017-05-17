%{
#include <vector>

#include "dagr_algorithm_executive.h"
#include "dagr_time_step_executive.h"
#include "dagr_metadata.h"
#include "dagr_algorithm.h"
#include "dagr_threaded_algorithm.h"
#include "dagr_temporal_reduction.h"
#include "dagr_variant_array.h"

#include "dagr_py_object.h"
#include "dagr_py_sequence.h"
#include "dagr_py_array.h"
#include "dagr_py_iterator.h"
#include "dagr_py_gil_state.h"
%}

/***************************************************************************
 variant_array
 ***************************************************************************/
%ignore dagr_variant_array::shared_from_this;
%ignore std::enable_shared_from_this<dagr_variant_array>;
%shared_ptr(std::enable_shared_from_this<dagr_variant_array>)
%shared_ptr(dagr_variant_array)
%shared_ptr(dagr_variant_array_impl<double>)
%shared_ptr(dagr_variant_array_impl<float>)
%shared_ptr(dagr_variant_array_impl<char>)
%shared_ptr(dagr_variant_array_impl<int>)
%shared_ptr(dagr_variant_array_impl<long long>)
%shared_ptr(dagr_variant_array_impl<unsigned char>)
%shared_ptr(dagr_variant_array_impl<unsigned int>)
%shared_ptr(dagr_variant_array_impl<unsigned long long>)
%shared_ptr(dagr_variant_array_impl<std::string>)
class dagr_variant_array;
%template(dagr_variant_array_base) std::enable_shared_from_this<dagr_variant_array>;
%include "dagr_common.h"
%include "dagr_shared_object.h"
%include "dagr_variant_array_fwd.h"
%ignore dagr_variant_array::operator=;
%ignore dagr_variant_array_factory;
%ignore dagr_variant_array::append(const dagr_variant_array &other);
%ignore dagr_variant_array::append(const const_p_dagr_variant_array &other);
%ignore copy(const dagr_variant_array &other);
%include "dagr_variant_array.h"
%template(dagr_double_array) dagr_variant_array_impl<double>;
%template(dagr_float_array) dagr_variant_array_impl<float>;
%template(dagr_int_array) dagr_variant_array_impl<char>;
%template(dagr_char_array) dagr_variant_array_impl<int>;
%template(dagr_long_long_array) dagr_variant_array_impl<long long>;
%template(dagr_unsigned_int_array) dagr_variant_array_impl<unsigned char>;
%template(dagr_unsigned_char_array) dagr_variant_array_impl<unsigned int>;
%template(dagr_unsigned_long_long_array) dagr_variant_array_impl<unsigned long long>;
%extend dagr_variant_array
{
    static
    p_dagr_variant_array New(PyObject *obj)
    {
        dagr_py_gil_state gil;

        p_dagr_variant_array varr;
        if ((varr = dagr_py_object::new_variant_array(obj))
            || (varr = dagr_py_array::new_variant_array(obj))
            || (varr = dagr_py_sequence::new_variant_array(obj))
            || (varr = dagr_py_iterator::new_variant_array(obj)))
            return varr;

        PyErr_Format(PyExc_TypeError, "Failed to convert value");
        return nullptr;
    }

    DAGR_PY_STR()

    unsigned long __len__()
    { return self->size(); }

    void __setitem__(unsigned long i, PyObject *value)
    {
        dagr_py_gil_state gil;

#ifndef NDEBUG
        if (i >= self->size())
        {
            PyErr_Format(PyExc_IndexError,
                "index %lu is out of bounds in dagr_variant_array "
                " with size %lu", i, self->size());
            return;
        }
#endif
        if (dagr_py_object::set(self, i, value))
            return;

        PyErr_Format(PyExc_TypeError,
            "failed to set value at index %lu", i);
    }

    PyObject *__getitem__(unsigned long i)
    {
        dagr_py_gil_state gil;

        if (i >= self->size())
        {
            PyErr_Format(PyExc_IndexError,
                "index %lu is out of bounds in dagr_variant_array "
                " with size %lu", i, self->size());
            return nullptr;
        }

        TEMPLATE_DISPATCH(dagr_variant_array_impl, self,
            TT *varrt = static_cast<TT*>(self);
            return dagr_py_object::py_tt<NT>::new_object(varrt->get(i));
            )
        else TEMPLATE_DISPATCH_CASE(dagr_variant_array_impl,
            std::string, self,
            TT *varrt = static_cast<TT*>(self);
            return dagr_py_object::py_tt<NT>::new_object(varrt->get(i));
            )

        PyErr_Format(PyExc_TypeError,
            "failed to get value at index %lu", i);
        return nullptr;
    }

    PyObject *as_array()
    {
        dagr_py_gil_state gil;

        return reinterpret_cast<PyObject*>(
            dagr_py_array::new_object(self));
    }

    void append(PyObject *obj)
    {
        dagr_py_gil_state gil;

        if (dagr_py_object::append(self, obj)
            || dagr_py_array::append(self, obj)
            || dagr_py_sequence::append(self, obj))
            return;

        PyErr_Format(PyExc_TypeError,
            "Failed to convert value");
    }

    void copy(PyObject *obj)
    {
        dagr_py_gil_state gil;

        if (dagr_py_object::copy(self, obj)
            || dagr_py_array::copy(self, obj)
            || dagr_py_sequence::copy(self, obj))
            return;

        PyErr_Format(PyExc_TypeError,
            "Failed to convert value");
    }
}
DAGR_PY_DYNAMIC_VARIANT_ARRAY_CAST(double, double)
DAGR_PY_DYNAMIC_VARIANT_ARRAY_CAST(float, float)
DAGR_PY_DYNAMIC_VARIANT_ARRAY_CAST(char, char)
DAGR_PY_DYNAMIC_VARIANT_ARRAY_CAST(int, int)
DAGR_PY_DYNAMIC_VARIANT_ARRAY_CAST(long long, long_long)
DAGR_PY_DYNAMIC_VARIANT_ARRAY_CAST(unsigned char, unsigned_char)
DAGR_PY_DYNAMIC_VARIANT_ARRAY_CAST(unsigned int, unsigned_int)
DAGR_PY_DYNAMIC_VARIANT_ARRAY_CAST(unsigned long long, unsigned_long_long)

/***************************************************************************
 metadata
 ***************************************************************************/
%ignore dagr_metadata::dagr_metadata(dagr_metadata &&);
%ignore dagr_metadata::operator=;
%ignore operator<(const dagr_metadata &, const dagr_metadata &);
%ignore operator&(const dagr_metadata &, const dagr_metadata &);
%ignore operator==(const dagr_metadata &, const dagr_metadata &);
%ignore operator!=(const dagr_metadata &, const dagr_metadata &);
%ignore dagr_metadata::insert;
%ignore dagr_metadata::set; /* use __setitem__ instead */
%ignore dagr_metadata::get; /* use __getitem__ instead */
%include "dagr_metadata.h"
%extend dagr_metadata
{
    DAGR_PY_STR()

    /* md['name'] = value */
    void __setitem__(const std::string &name, PyObject *value)
    {
        dagr_py_gil_state gil;

        p_dagr_variant_array varr;
        if ((varr = dagr_py_object::new_variant_array(value))
            || (varr = dagr_py_array::new_variant_array(value))
            || (varr = dagr_py_sequence::new_variant_array(value))
            || (varr = dagr_py_iterator::new_variant_array(value)))
        {
            self->insert(name, varr);
            return;
        }
        PyErr_Format(PyExc_TypeError,
            "Failed to convert value for key \"%s\"", name.c_str());
    }

    /* return md['name'] */
    PyObject *__getitem__(const std::string &name)
    {
        dagr_py_gil_state gil;

        p_dagr_variant_array varr = self->get(name);
        if (!varr)
        {
            PyErr_Format(PyExc_KeyError,
                "key \"%s\" not found", name.c_str());
            return nullptr;
        }

        size_t n_elem = varr->size();
        if (n_elem < 1)
        {
            return PyList_New(0);
        }
        else if (n_elem == 1)
        {
            TEMPLATE_DISPATCH(dagr_variant_array_impl,
                varr.get(),
                TT *varrt = static_cast<TT*>(varr.get());
                return dagr_py_object::py_tt<NT>::new_object(varrt->get(0));
                )
            else TEMPLATE_DISPATCH_CASE(const dagr_variant_array_impl,
                std::string, varr.get(),
                TT *varrt = static_cast<TT*>(varr.get());
                return dagr_py_object::py_tt<NT>::new_object(varrt->get(0));
                )
            else TEMPLATE_DISPATCH_CASE(const dagr_variant_array_impl,
                dagr_metadata, varr.get(),
                TT *varrt = static_cast<TT*>(varr.get());
                return SWIG_NewPointerObj(new dagr_metadata(varrt->get(0)),
                     SWIGTYPE_p_dagr_metadata, SWIG_POINTER_OWN);
                )
        }
        else if (n_elem > 1)
        {
            TEMPLATE_DISPATCH(dagr_variant_array_impl,
                varr.get(),
                TT *varrt = static_cast<TT*>(varr.get());
                return reinterpret_cast<PyObject*>(
                    dagr_py_array::new_object(varrt));
                )
            else TEMPLATE_DISPATCH_CASE(const dagr_variant_array_impl,
                std::string, varr.get(),
                TT *varrt = static_cast<TT*>(varr.get());
                PyObject *list = PyList_New(n_elem);
                for (size_t i = 0; i < n_elem; ++i)
                {
                    PyList_SET_ITEM(list, i,
                        dagr_py_object::py_tt<NT>::new_object(varrt->get(i)));
                }
                return list;
                )
            else TEMPLATE_DISPATCH_CASE(const dagr_variant_array_impl,
                dagr_metadata, varr.get(),
                TT *varrt = static_cast<TT*>(varr.get());
                PyObject *list = PyList_New(n_elem);
                for (size_t i = 0; i < n_elem; ++i)
                {
                    PyList_SET_ITEM(list, i,
                        SWIG_NewPointerObj(new dagr_metadata(varrt->get(i)),
                            SWIGTYPE_p_dagr_metadata, SWIG_POINTER_OWN));
                }
                return list;
                )
        }

        return PyErr_Format(PyExc_TypeError,
            "Failed to convert value for key \"%s\"", name.c_str());
    }

    void append(const std::string &name, PyObject *obj)
    {
        dagr_py_gil_state gil;

        dagr_variant_array *varr = self->get(name).get();
        if (!varr)
        {
            PyErr_Format(PyExc_KeyError,
                "key \"%s\" not found", name.c_str());
            return;
        }

        if (dagr_py_object::append(varr, obj)
            || dagr_py_array::append(varr, obj)
            || dagr_py_sequence::append(varr, obj)
            || dagr_py_iterator::append(varr, obj))
            return;

        PyErr_Format(PyExc_TypeError,
            "Failed to convert value");
    }
}
%template(std_vector_metadata) std::vector<dagr_metadata>;

/***************************************************************************
 dataset
 ***************************************************************************/
%ignore dagr_dataset::shared_from_this;
%ignore std::enable_shared_from_this<dagr_dataset>;
%shared_ptr(std::enable_shared_from_this<dagr_dataset>)
%shared_ptr(dagr_dataset)
class dagr_dataset;
%template(dagr_dataset_base) std::enable_shared_from_this<dagr_dataset>;
%ignore dagr_dataset::operator=;
%include "dagr_dataset_fwd.h"
%include "dagr_dataset.h"
%extend dagr_dataset
{
    DAGR_PY_STR()
}
%template(std_vector_dataset) std::vector<std::shared_ptr<dagr_dataset>>;

/***************************************************************************/
%define DAGR_PY_DATASET_METADATA(_type, _name)
    void set_## _name(PyObject *obj)
    {
        dagr_py_gil_state gil;

        DAGR_PY_OBJECT_DISPATCH_NUM(obj,
            self->set_ ## _name(dagr_py_object::cpp_tt<
                dagr_py_object::py_tt<_type>::tag>::value(obj));
            return;
            )

        PyErr_Format(PyExc_TypeError,
            "Failed to set property \"%s\"", #_name);
    }

    PyObject *get_## _name()
    {
        dagr_py_gil_state gil;

        _type val;
        self->get_ ## _name(val);

        return dagr_py_object::py_tt<_type>::new_object(val);
    }
%enddef

/***************************************************************************/
%define DAGR_PY_DATASET_VECTOR_METADATA(_type, _name)
    PyObject *get_## _name()
    {
        dagr_py_gil_state gil;

        p_dagr_variant_array_impl<_type> varr =
             dagr_variant_array_impl<_type>::New();

        self->get_## _name(varr);

        PyObject *list = dagr_py_sequence::new_object(varr);
        if (!list)
        {
            PyErr_Format(PyExc_TypeError,
                "Failed to get property \"%s\"", # _name);
        }

        return list;
    }

    void set_## _name(PyObject *array)
    {
        dagr_py_gil_state gil;

        p_dagr_variant_array varr;
        if ((varr = dagr_py_array::new_variant_array(array))
            || (varr = dagr_py_sequence::new_variant_array(array))
            || (varr = dagr_py_iterator::new_variant_array(array)))
        {
            self->set_## _name(varr);
            return;
        }

        PyErr_Format(PyExc_TypeError,
            "Failed to set property \"%s\"", # _name);
    }
%enddef

/***************************************************************************
 algorithm_executive
 ***************************************************************************/
%ignore dagr_algorithm_executive::shared_from_this;
%ignore std::enable_shared_from_this<dagr_algorithm_executive>;
%shared_ptr(std::enable_shared_from_this<dagr_algorithm_executive>)
%shared_ptr(dagr_algorithm_executive)
class dagr_algorithm_executive;
%template(dagr_algorithm_executive_base) std::enable_shared_from_this<dagr_algorithm_executive>;
%ignore dagr_algorithm_executive::operator=;
%include "dagr_common.h"
%include "dagr_shared_object.h"
%include "dagr_algorithm_executive_fwd.h"
%include "dagr_algorithm_executive.h"

/***************************************************************************
 time_step_executive
 ***************************************************************************/
%ignore dagr_time_step_executive::shared_from_this;
%shared_ptr(dagr_time_step_executive)
%ignore dagr_time_step_executive::operator=;
%include "dagr_time_step_executive.h"

/***************************************************************************
 algorithm
 ***************************************************************************/
%ignore dagr_algorithm::shared_from_this;
%ignore std::enable_shared_from_this<dagr_algorithm>;
%shared_ptr(std::enable_shared_from_this<dagr_algorithm>)
%shared_ptr(dagr_algorithm)
class dagr_algorithm;
%template(dagr_algorithm_base) std::enable_shared_from_this<dagr_algorithm>;
typedef std::pair<std::shared_ptr<dagr_algorithm>, unsigned int> dagr_algorithm_output_port;
%template(dagr_output_port_type) std::pair<std::shared_ptr<dagr_algorithm>, unsigned int>;
%include "dagr_common.h"
%include "dagr_shared_object.h"
%include "dagr_algorithm_fwd.h"
%include "dagr_program_options.h"
%include "dagr_algorithm.h"

/***************************************************************************/
%define DAGR_PY_ALGORITHM_PROPERTY(_type, _name)
    void set_## _name(PyObject *obj)
    {
        dagr_py_gil_state gil;

        DAGR_PY_OBJECT_DISPATCH_NUM(obj,
            self->set_ ## _name(dagr_py_object::cpp_tt<
                dagr_py_object::py_tt<_type>::tag>::value(obj));
            return;
            )

        PyErr_Format(PyExc_TypeError,
            "Failed to set property \"%s\"", #_name);
    }

    PyObject *get_## _name()
    {
        dagr_py_gil_state gil;

        _type val;
        self->get_ ## _name(val);

        return dagr_py_object::py_tt<_type>::new_object(val);
    }
%enddef

/***************************************************************************/
%define DAGR_PY_ALGORITHM_VECTOR_PROPERTY(_type, _name)
    PyObject *get_## _name ##s()
    {
        dagr_py_gil_state gil;

        p_dagr_variant_array_impl<_type> varr =
             dagr_variant_array_impl<_type>::New();

        self->get_## _name ##s(varr);

        PyObject *list = dagr_py_sequence::new_object(varr);
        if (!list)
        {
            PyErr_Format(PyExc_TypeError,
                "Failed to get property \"%s\"", # _name);
        }

        return list;
    }

    void set_## _name ##s(PyObject *array)
    {
        dagr_py_gil_state gil;

        p_dagr_variant_array varr;
        if ((varr = dagr_py_array::new_variant_array(array))
            || (varr = dagr_py_sequence::new_variant_array(array))
            || (varr = dagr_py_iterator::new_variant_array(array)))
        {
            self->set_## _name ##s(varr);
            return;
        }

        PyErr_Format(PyExc_TypeError,
            "Failed to set property \"%s\"", # _name);
    }
%enddef

/***************************************************************************
 threaded_algorithm
 ***************************************************************************/
%ignore dagr_threaded_algorithm::shared_from_this;
%shared_ptr(dagr_threaded_algorithm)
%ignore dagr_threaded_algorithm::operator=;
%include "dagr_threaded_algorithm_fwd.h"
%include "dagr_threaded_algorithm.h"

/***************************************************************************
 temporal_reduction
 ***************************************************************************/
%ignore dagr_temporal_reduction::shared_from_this;
%shared_ptr(dagr_temporal_reduction)
%ignore dagr_temporal_reduction::operator=;
%include "dagr_temporal_reduction_fwd.h"
%include "dagr_temporal_reduction.h"
