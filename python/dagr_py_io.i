%{
#include "dagr_algorithm.h"
#include "dagr_table_reader.h"
#include "dagr_table_writer.h"
#include "dagr_vtk_cartesian_mesh_writer.h"
%}

/***************************************************************************
 table_reader
 ***************************************************************************/
%ignore dagr_table_reader::shared_from_this;
%shared_ptr(dagr_table_reader)
%ignore dagr_table_reader::operator=;
%include "dagr_table_reader.h"

/***************************************************************************
 table_writer
 ***************************************************************************/
%ignore dagr_table_writer::shared_from_this;
%shared_ptr(dagr_table_writer)
%ignore dagr_table_writer::operator=;
%include "dagr_table_writer.h"

/***************************************************************************
 vtk_cartesian_mesh_writer
 ***************************************************************************/
%ignore dagr_vtk_cartesian_mesh_writer::shared_from_this;
%shared_ptr(dagr_vtk_cartesian_mesh_writer)
%ignore dagr_vtk_cartesian_mesh_writer::operator=;
%include "dagr_vtk_cartesian_mesh_writer.h"
