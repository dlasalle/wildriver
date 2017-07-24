/**
 * @file CSRFile.cpp
 * @brief Class functions for reading and writing metis graph files.
 * @author Dominique LaSalle <wildriver@domnet.org>
 * Copyright 2015-2016
 * @version 1
 *
 */




#include <sstream>
#include <cstddef>
#include "CSRFile.hpp"




namespace WildRiver
{


/******************************************************************************
* CONSTANTS *******************************************************************
******************************************************************************/


namespace
{


static size_t const BUFFER_SIZE = 4096;


}


std::string const CSRFile::NAME = "CSR";




/******************************************************************************
* PUBLIC STATIC FUNCTIONS *****************************************************
******************************************************************************/


bool CSRFile::hasExtension(
    std::string const & f)
{
  std::vector<std::string> extensions;

  extensions.push_back(".csr");

  return TextFile::matchExtension(f,extensions);
}




/******************************************************************************
* PRIVATE FUNCTIONS ***********************************************************
******************************************************************************/


bool CSRFile::nextNoncommentLine(
    std::string & line)
{
  do {
    if (!m_file.nextLine(line)) {
      return false;
    }
  } while (isComment(line));

  return true;
}


bool CSRFile::isComment(
    std::string const & line) const noexcept
{
  // awful solution since I can't declare this statically in c++ -- at
  // somepoint generate all 256 entries
  bool comment_chars[256] = {false};
  comment_chars['#'] = true;
  comment_chars['%'] = true;
  comment_chars['"'] = true;
  comment_chars['/'] = true;

  return line.size() > 0 && comment_chars[static_cast<uint8_t>(line[0])];
}


void CSRFile::readHeader()
{
  // open our file for reading
  m_file.openRead();

  // go to the start of the file
  firstRow();

  m_numRows = 0;
  m_numRows = 0;
  m_nnz = 0;

  while (nextNoncommentLine(m_line)) {
    char const * const lineEnd = m_line.data() + m_line.length();

    char * sptr;
    char * eptr = (char*)m_line.data();

    // parse the line
    dim_t degree = 0;
    while (true) {
      sptr = eptr;
      dim_t col = static_cast<dim_t>(std::strtoull(sptr,&eptr,10));
      if (eptr == sptr) {
        // nothing left to read
        break;
      }

      if (col > m_numCols) {
        m_numCols = col;
      }

      // skip value without converting
      do {
        ++eptr;
      } while (eptr < lineEnd && *eptr != ' ');
      ++degree;
    }

    m_nnz += degree;
    ++m_numRows;
  }

  // go back to the start
  firstRow();
}


void CSRFile::writeHeader()
{
  // open for writing
  m_file.openWrite();
}




/******************************************************************************
* CONSTRUCTORS / DESTRUCTORS **************************************************
******************************************************************************/


CSRFile::CSRFile(
    std::string const & fname) :
  m_numRows(NULL_DIM),
  m_numCols(NULL_DIM),
  m_nnz(NULL_IND),
  m_currentRow(0),
  m_line(BUFFER_SIZE,'\0'),
  m_file(fname)
{
  // do nothing
}


CSRFile::~CSRFile()
{
  // do nothing
}




/******************************************************************************
* PUBLIC FUNCTIONS ************************************************************
******************************************************************************/


void CSRFile::read(
    ind_t * const rowptr,
    dim_t * const rowind,
    val_t * const rowval,
    double * const progress)
{
  std::vector<matrix_entry_struct> row;

  dim_t const interval = m_numRows > 100 ? m_numRows / 100 : 1;
  double const increment = 1.0/100.0;

  // read in the rows the matrix into our csr
  dim_t j = 0;
  rowptr[0] = j;
  for (dim_t i = 0; i < m_numRows; ++i) {
    dim_t degree;

    dim_t * const rowindStart = rowind+rowptr[i];
    val_t * const rowvalStart = rowval ? rowval+rowptr[i] : nullptr;

    if (!getNextRow(&degree,rowindStart,rowvalStart)) {
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


void CSRFile::write(
    ind_t const * const rowptr,
    dim_t const * const rowind,
    val_t const * const rowval)
{
  std::vector<matrix_entry_struct> row;


  for (dim_t i = 0; i < m_numRows; ++i) {
    // build and insert a new row
    row.clear();
    for (ind_t j=rowptr[i];j<rowptr[i+1];++j) {
      matrix_entry_struct entry;

      entry.ind = rowind[j];
      if (rowval) {
        entry.val = rowval[j];
      } else {
        entry.val = 1;
      }
      row.push_back(entry);
    }
    setNextRow(row);
  }
}




/******************************************************************************
* PROTECTED FUNCTIONS *********************************************************
******************************************************************************/


void CSRFile::firstRow()
{
  // go to the start of the file
  m_file.resetStream();
  m_currentRow = 0; 
}


bool CSRFile::getNextRow(
    dim_t * const numNonZeros,
    dim_t * const columns,
    val_t * const values)
{
  if (!nextNoncommentLine(m_line)) {
    return false;
  }

  // TOOD: don't cast constness away
  char * sptr;
  char * eptr = (char*)m_line.data();

  dim_t degree = 0;
  dim_t col;
  val_t val;

  // Loop through row until we streamed to the end
  while (true) {
    sptr = eptr;
    col = static_cast<dim_t>(std::strtoull(sptr,&eptr,10) - 1);
    if (eptr == sptr) {
      // nothing left to read
      break;
    }

    sptr = eptr;

    val = std::strtod(sptr,&eptr);
    if (eptr == sptr) {
      throw BadFileException(std::string("Failed to read column on "
            "line ") + std::to_string(m_file.getCurrentLine()));
    }

    columns[degree] = col;
    if (values) {
      values[degree] = val;
    }
    ++degree;
  }

  ++m_currentRow;

  *numNonZeros = degree;

  return true;
}


void CSRFile::setNextRow(
    std::vector<matrix_entry_struct> const & next)
{
  size_t const size = next.size();
  std::stringstream streamBuffer;
  if (size > 0) {
    for (size_t i=0; i<size-1; ++i) {
      matrix_entry_struct const & e = next[i];
      streamBuffer << (e.ind+1) << " " << e.val << " ";
    }
    matrix_entry_struct const & e = next.back();
    streamBuffer << (e.ind+1) << " " << e.val;
  }

  m_file.setNextLine(streamBuffer.str());

  ++m_currentRow;
}




}
