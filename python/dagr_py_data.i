%{
#include <memory>
#include <sstream>
#include "dagr_variant_array.h"
#include "dagr_array_collection.h"
#include "dagr_cartesian_mesh.h"
#include "dagr_mesh.h"
#include "dagr_table.h"
#include "dagr_py_object.h"
#include "dagr_table_collection.h"
#include "dagr_database.h"
#include "dagr_py_object.h"
%}

/***************************************************************************
 array_collection
 ***************************************************************************/
%ignore dagr_array_collection::shared_from_this;
%shared_ptr(dagr_array_collection)
%ignore dagr_array_collection::operator=;
%ignore dagr_array_collection::operator[];
%include "dagr_array_collection_fwd.h"
%include "dagr_array_collection.h"
%extend dagr_array_collection
{
    DAGR_PY_STR()

    /* add or replace an array using syntax: col['name'] = array */
    void __setitem__(const std::string &name, PyObject *array)
    {
        dagr_py_gil_state gil;

        p_dagr_variant_array varr;
        if ((varr = dagr_py_array::new_variant_array(array))
            || (varr = dagr_py_sequence::new_variant_array(array))
            || (varr = dagr_py_iterator::new_variant_array(array)))
        {
            self->set(name, varr);
            return;
        }
        PyErr_Format(PyExc_TypeError,
            "Failed to convert array for key \"%s\"", name.c_str());
    }

    /* return an array using the syntax: col['name'] */
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

        TEMPLATE_DISPATCH(dagr_variant_array_impl,
            varr.get(),
            TT *varrt = static_cast<TT*>(varr.get());
            return reinterpret_cast<PyObject*>(
                dagr_py_array::new_object(varrt));
            )

        return PyErr_Format(PyExc_TypeError,
            "Failed to convert array for key \"%s\"", name.c_str());
    }

    /* handle conversion to variant arrays */
    void append(const std::string &name, PyObject *array)
    {
       dagr_array_collection___setitem__(self, name, array);
    }
}

/***************************************************************************
 mesh
 ***************************************************************************/
%ignore dagr_mesh::shared_from_this;
%shared_ptr(dagr_mesh)
%ignore dagr_mesh::operator=;
%include "dagr_mesh_fwd.h"
%include "dagr_mesh.h"
DAGR_PY_DYNAMIC_CAST(dagr_mesh, dagr_dataset)
%extend dagr_mesh
{
    DAGR_PY_STR()
}

/***************************************************************************
 cartesian_mesh
 ***************************************************************************/
%ignore dagr_cartesian_mesh::shared_from_this;
%shared_ptr(dagr_cartesian_mesh)
%ignore dagr_cartesian_mesh::operator=;
%ignore dagr_cartesian_mesh::get_time(double *) const;
%ignore dagr_cartesian_mesh::get_time_step(unsigned long *) const;
%ignore dagr_cartesian_mesh::set_calendar(std::string const *);
%ignore dagr_cartesian_mesh::set_time_units(std::string const *);
%include "dagr_cartesian_mesh_fwd.h"
%include "dagr_cartesian_mesh.h"
DAGR_PY_DYNAMIC_CAST(dagr_cartesian_mesh, dagr_dataset)
%extend dagr_cartesian_mesh
{
    DAGR_PY_STR()

    DAGR_PY_DATASET_METADATA(double, time)
    DAGR_PY_DATASET_METADATA(unsigned long, time_step)
    DAGR_PY_DATASET_METADATA(std::string, calendar)
    DAGR_PY_DATASET_METADATA(std::string, time_units)
    DAGR_PY_DATASET_VECTOR_METADATA(unsigned long, extent)
    DAGR_PY_DATASET_VECTOR_METADATA(unsigned long, whole_extent)
}

/***************************************************************************
 table
 ***************************************************************************/
%ignore dagr_table::shared_from_this;
%shared_ptr(dagr_table)
%ignore dagr_table::operator=;
%ignore dagr_table::set_calendar(std::string const *);
%ignore dagr_table::set_time_units(std::string const *);
%include "dagr_table_fwd.h"
%include "dagr_table.h"
DAGR_PY_DYNAMIC_CAST(dagr_table, dagr_dataset)
%extend dagr_table
{
    DAGR_PY_STR()

    DAGR_PY_DATASET_METADATA(std::string, calendar)
    DAGR_PY_DATASET_METADATA(std::string, time_units)

    /* update the value at r,c. r is a row index and c an
    either be a column index or name */
    void __setitem__(PyObject *idx, PyObject *obj)
    {
        dagr_py_gil_state gil;

        if (!PySequence_Check(idx) || (PySequence_Size(idx) != 2))
        {
            PyErr_Format(PyExc_KeyError,
                "Requires a 2 element sequence specifying "
                "desired row and column indices");
            return;
        }

        unsigned long r = PyInt_AsLong(PySequence_GetItem(idx, 0));
        unsigned long c = PyInt_AsLong(PySequence_GetItem(idx, 1));

        p_dagr_variant_array col = self->get_column(c);
        if (!col)
        {
            PyErr_Format(PyExc_IndexError,
                "Column %lu is out of bounds", c);
            return;
        }

        if (r >= col->size())
            col->resize(r+1);

        DAGR_PY_OBJECT_DISPATCH_NUM(obj,
            dagr_py_object::cpp_tt<OT>::type val
                = dagr_py_object::cpp_tt<OT>::value(obj);
            TEMPLATE_DISPATCH(
                dagr_variant_array_impl,
                col.get(),
                TT *arr = static_cast<TT*>(col.get());
                arr->set(r, val);
                return;
                )
            )
        DAGR_PY_OBJECT_DISPATCH_STR(obj,
            dagr_py_object::cpp_tt<OT>::type val
                = dagr_py_object::cpp_tt<OT>::value(obj);
            TEMPLATE_DISPATCH_CASE(
                dagr_variant_array_impl,
                std::string,
                col.get(),
                TT *arr = static_cast<TT*>(col.get());
                arr->set(r, val);
                return;
                )
            )

        PyErr_Format(PyExc_TypeError,
            "Failed to convert value at %ld,%ld", r, c);
    }

    /* look up the value at r,c. r is a row index and c an
    either be a column index or name */
    PyObject *__getitem__(PyObject *idx)
    {
        dagr_py_gil_state gil;

        if (!PySequence_Check(idx) || (PySequence_Size(idx) != 2))
        {
            PyErr_Format(PyExc_KeyError,
                "Requires a 2 element sequence specifying "
                "desired row and column indices");
            return nullptr;
        }

        unsigned long r = PyInt_AsLong(PySequence_GetItem(idx, 0));
        unsigned long c = PyInt_AsLong(PySequence_GetItem(idx, 1));

        p_dagr_variant_array col = self->get_column(c);
        if (!col)
        {
            PyErr_Format(PyExc_IndexError,
                "Column %lu is out of bounds", c);
            return nullptr;
        }

        if (r >= col->size())
        {
            PyErr_Format(PyExc_IndexError,
                "Row %lu is out of bounds", r);
            return nullptr;
        }

        TEMPLATE_DISPATCH(dagr_variant_array_impl,
            col.get(),
            TT *arr = static_cast<TT*>(col.get());
            return reinterpret_cast<PyObject*>(
                dagr_py_object::py_tt<NT>::new_object(arr->get(r)));
            )
        TEMPLATE_DISPATCH_CASE(dagr_variant_array_impl,
            std::string,
            col.get(),
            TT *arr = static_cast<TT*>(col.get());
            return reinterpret_cast<PyObject*>(
                dagr_py_object::py_tt<NT>::new_object(arr->get(r)));
            )

        return PyErr_Format(PyExc_TypeError,
            "Failed to convert value at %lu, %lu", r, c);
    }

    /* replace existing column in a single shot */
    void set_column(PyObject *id, PyObject *array)
    {
        dagr_py_gil_state gil;

        p_dagr_variant_array col;

        if (PyInt_Check(id))
            col = self->get_column(PyInt_AsLong(id));
        else
        if (PyString_Check(id))
            col = self->get_column(PyString_AsString(id));

        if (!col)
        {
            PyErr_Format(PyExc_KeyError, "Invalid column id.");
            return;
        }

        p_dagr_variant_array varr;
        if ((varr = dagr_py_array::new_variant_array(array))
            || (varr = dagr_py_sequence::new_variant_array(array))
            || (varr = dagr_py_iterator::new_variant_array(array)))
        {
            col->copy(varr);
            return;
        }

        PyErr_Format(PyExc_TypeError, "Failed to convert array");
    }

    /* declare a column */
    void declare_column(const char *name, const char *type)
    {
        using u_char_t = unsigned char;
        using u_int_t = unsigned int;
        using u_long_t = unsigned long;
        using long_long_t = long long;
        using u_long_long_t = unsigned long long;

        if (!strcmp(type, "c"))
            self->declare_column(name, char());
        else
        if (!strcmp(type, "uc"))
            self->declare_column(name, u_char_t());
        else
        if (!strcmp(type, "i"))
            self->declare_column(name, int());
        else
        if (!strcmp(type, "ui"))
            self->declare_column(name, u_int_t());
        else
        if (!strcmp(type, "l"))
            self->declare_column(name, long());
        else
        if (!strcmp(type, "ul"))
            self->declare_column(name, u_long_t());
        else
        if (!strcmp(type, "ll"))
            self->declare_column(name, long_long_t());
        else
        if (!strcmp(type, "ull"))
            self->declare_column(name, u_long_long_t());
        else
        if (!strcmp(type, "f"))
            self->declare_column(name, float());
        else
        if (!strcmp(type, "d"))
            self->declare_column(name, double());
        else
        if (!strcmp(type, "s"))
            self->declare_column(name, std::string());
        else
            PyErr_Format(PyExc_RuntimeError,
                "Bad type code \"%s\" for column \"%s\". Must be one of: "
                "c,uc,i,ui,l,ul,ll,ull,f,d,s", type, name);
    }

    /* declare a set of columns */
    void declare_columns(PyObject *names, PyObject *types)
    {
        dagr_py_gil_state gil;

        if (!PyList_Check(names))
        {
            PyErr_Format(PyExc_TypeError,
                "names argument must be a list.");
            return;
        }

        if (!PyList_Check(types))
        {
            PyErr_Format(PyExc_TypeError,
                "types argument must be a list.");
            return;
        }

        Py_ssize_t n_names = PyList_Size(names);
        Py_ssize_t n_types = PyList_Size(types);

        if (n_names != n_types)
        {
            PyErr_Format(PyExc_RuntimeError,
                "names and types arguments must have same length.");
            return;
        }

        for (Py_ssize_t i = 0; i < n_names; ++i)
        {
            const char *name = PyString_AsString(PyList_GetItem(names, i));
            if (!name)
            {
                PyErr_Format(PyExc_TypeError,
                    "item at index %ld in names is not a string.", i);
                return;
            }

            const char *type = PyString_AsString(PyList_GetItem(types, i));
            if (!type)
            {
                PyErr_Format(PyExc_TypeError,
                    "item at index %ld in types is not a string.", i);
                return;

            }

            dagr_table_declare_column(self, name, type);
        }
    }

    /* append sequence,array, or object in column order */
    void append(PyObject *obj)
    {
        dagr_py_gil_state gil;

        // arrays
        if (PyArray_Check(obj))
        {
            PyArrayObject *arr = reinterpret_cast<PyArrayObject*>(obj);
            DAGR_PY_ARRAY_DISPATCH(arr,
                NpyIter *it = NpyIter_New(arr, NPY_ITER_READONLY,
                        NPY_KEEPORDER, NPY_NO_CASTING, nullptr);
                NpyIter_IterNextFunc *next = NpyIter_GetIterNext(it, nullptr);
                AT **ptrptr = reinterpret_cast<AT**>(NpyIter_GetDataPtrArray(it));
                do
                {
                    self->append(**ptrptr);
                }
                while (next(it));
                NpyIter_Deallocate(it);
                return;
                )
            PyErr_Format(PyExc_TypeError, "failed to append array");
            return;
        }

        // sequences
        if (PySequence_Check(obj) && !PyString_Check(obj))
        {
            long n_items = PySequence_Size(obj);
            for (long i = 0; i < n_items; ++i)
            {
                PyObject *obj_i = PySequence_GetItem(obj, i);
                DAGR_PY_OBJECT_DISPATCH_NUM(obj_i,
                    self->append(dagr_py_object::cpp_tt<OT>::value(obj_i));
                    continue;
                    )
                DAGR_PY_OBJECT_DISPATCH_STR(obj_i,
                    self->append(dagr_py_object::cpp_tt<OT>::value(obj_i));
                    continue;
                    )
                PyErr_Format(PyExc_TypeError,
                    "failed to append sequence element %ld ", i);
            }
            return;
        }

        // objects
        DAGR_PY_OBJECT_DISPATCH_NUM(obj,
            self->append(dagr_py_object::cpp_tt<OT>::value(obj));
            return;
            )
        DAGR_PY_OBJECT_DISPATCH_STR(obj,
            self->append(dagr_py_object::cpp_tt<OT>::value(obj));
            return;
            )
        PyErr_Format(PyExc_TypeError, "failed to append object");
    }

    /* stream insertion operator */
    p_dagr_table __lshift__(PyObject *obj)
    {
        dagr_table_append(self, obj);
        return self->shared_from_this();
    }
}

/***************************************************************************
 table_collection
 ***************************************************************************/
%ignore dagr_table_collection::shared_from_this;
%shared_ptr(dagr_table_collection)
%ignore dagr_table_collection::operator=;
%ignore dagr_table_collection::operator[];
%include "dagr_table_collection_fwd.h"
%include "dagr_table_collection.h"
%extend dagr_table_collection
{
    DAGR_PY_STR()

    /* add or replace a table using syntax: col['name'] = table */
    void __setitem__(const std::string &name, p_dagr_table table)
    {
        self->set(name, table);
    }

    /* return an array using the syntax: col['name'] */
    p_dagr_table __getitem__(const std::string &name)
    {
        p_dagr_table table = self->get(name);
        if (!table)
        {
            PyErr_Format(PyExc_KeyError,
                "key \"%s\" not found", name.c_str());
            return nullptr;
        }

        return table;
    }
}

/***************************************************************************
 database
 ***************************************************************************/
%ignore dagr_database::shared_from_this;
%shared_ptr(dagr_database)
%ignore dagr_database::operator=;
%ignore dagr_database::get_table_name(unsigned int);
%include "dagr_database_fwd.h"
%include "dagr_database.h"
DAGR_PY_DYNAMIC_CAST(dagr_database, dagr_dataset)
%extend dagr_database
{
    DAGR_PY_STR()
}
