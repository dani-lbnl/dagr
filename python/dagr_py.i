%define DAGR_PY_DOC
"DAGR Python module

The core module contains the pipeline and executive
as well as metadata object, variant array and abstract
datasets.

The data module provides high-level data structures that
are produced and consumed by dagr_algorithms such as
Cartesian meshes, AMR datasets, and tables.

The alg module contains data processing, analysis, remeshing,
and detectors.

The io module contains readers and writers.
"
%enddef
%module (docstring=DAGR_PY_DOC) dagr_py
%feature("autodoc", "3");

%{
/* core */
#define SWIG_FILE_WITH_INIT
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#define PY_ARRAY_UNIQUE_SYMBOL  PyArray_API_dagr_py
#include <numpy/arrayobject.h>
#include <Python.h>
%}

%include <std_pair.i>
%include <std_string.i>
%include "dagr_py_vector.i"
%include "dagr_py_common.i"
%include "dagr_py_shared_ptr.i"
%include "dagr_py_core.i"
%include "dagr_py_data.i"
%include "dagr_py_alg.i"
%include "dagr_py_io.i"
%include "dagr_py_system.i"

%init %{
PyEval_InitThreads();
import_array();
%}
