/**
* @file CSRParser.cpp
* @brief Implementation of CSRParser class.
* @author Dominique LaSalle <dominique@solidlake.com>
* Copyright 2017, Solid Lake LLC
* @version 1
* @date 2017-07-25
*/




#include "CSRParser.hpp"




namespace WildRiver
{


/******************************************************************************
* CONSTRUCTORS / DESTRUCTOR ***************************************************
******************************************************************************/


CSRParser::CSRParser(
    IRowWiseMatrixReader * reader) :
  m_infoSet(false),
  m_numRows(NULL_DIM),
  m_numCols(NULL_DIM),
  m_nnz(NULL_IND),
  m_currentRow(0),
  m_reader(reader)
{
  // do nothing
}




/******************************************************************************
* PUBLIC FUNCTIONS ************************************************************
******************************************************************************/


void CSRParser::getInfo(
    dim_t & nrows,
    dim_t & ncols,
    ind_t & nnz)
{
  // see if need to read the header
  if (!m_infoSet) {
    m_reader->readHeader();
  }

  // set values
  nrows = m_numRows;
  ncols = m_numCols;
  nnz = m_nnz;

  m_infoSet = true;
}


void CSRParser::read(
    ind_t * rowptr,
    dim_t * rowind,
    val_t * rowval,
    double * progress)
{
  std::vector<matrix_entry_struct> row;

  dim_t const interval = m_numRows > 100 ? m_numRows / 100 : 1;
  double const increment = 1.0/100.0;

  dim_t currentRow = 0; 

  // read in the rows the matrix into our csr
  dim_t j = 0;
  rowptr[0] = j;
  for (dim_t i = 0; i < m_numRows; ++i) {
    dim_t degree;

    dim_t * const rowindStart = rowind+rowptr[i];
    val_t * const rowvalStart = rowval ? rowval+rowptr[i] : nullptr;

    if (!m_reader->getNextRow(&degree,rowindStart,rowvalStart)) {
      // fewer rows than expected
      throw EOFException(std::string("Failed to read row ") + \
          std::to_string(i) + std::string("/") + std::to_string(m_numRows)); 
    }

    rowptr[i+1] = rowptr[i]+degree;

    if (progress != nullptr && i % interval == 0) {
      *progress += increment;
    }
  }

  if (rowptr[m_numRows] != m_nnz) {
    // we read in the wrong number of non-zeroes
    throw EOFException(std::string("Only found ") + std::to_string(j) + \
        std::string("/") + std::to_string(m_nnz) + \
        std::string(" non-zeroes in file"));
  }
}






}

