/**
 * @file GraphWriterFactory.cpp
 * @brief Implementation of GraphWriterFactory for instantiating graph writers.
 * @author Dominique LaSalle <wildriver@domnet.org>
 * Copyright 2015-2016
 * @version 1
 * @date 2015-02-05
 */


#include <map>

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
    std::string const & name,
    bool useAdapter)
{
  std::unique_ptr<IGraphWriter> file;
  // determine what type of reader to instantiate based on extension
  if (MetisFile::hasExtension(name)) {
    file.reset(new MetisFile(name));
  } else if (useAdapter) {
    // need to wrap it with an adapter
    std::unique_ptr<IMatrixWriter> matPtr(MatrixWriterFactory::make(name, \
        false));
    file.reset(new MatrixGraphWriter(matPtr));
  } else {
    throw UnknownExtensionException(std::string("Unknown filetype: ") + name);
  }
 
  return file;
}




}
