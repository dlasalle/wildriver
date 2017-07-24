/**
 * @file GraphWriterFactory.cpp
 * @brief Implementation of GraphWriterFactory for instantiating graph writers.
 * @author Dominique LaSalle <wildriver@domnet.org>
 * Copyright 2015-2016
 * @version 1
 * @date 2015-02-05
 */



#include "GraphWriterFactory.hpp"
#include "MetisFile.hpp"
#include "CSRFile.hpp"
#include "MatrixWriterFactory.hpp"
#include "MatrixGraphWriter.hpp"




namespace WildRiver
{


/******************************************************************************
* STATIC FUNCTIONS ************************************************************
******************************************************************************/


std::unique_ptr<IGraphWriter> GraphWriterFactory::make(
    std::string const & name)
{
  std::unique_ptr<IGraphWriter> file;
  // determine what type of reader to instantiate based on extension
  if (MetisFile::hasExtension(name)) {
    file.reset(new MetisFile(name));
  } else {
    // need to wrap it with an adapter
    std::unique_ptr<IMatrixWriter> matPtr(MatrixFactory::make(name));
    file.reset(new MatrixGraphWriter(matPtr));
  }
 
  return file;
}




}
