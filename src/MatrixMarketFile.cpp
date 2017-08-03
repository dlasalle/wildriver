/**
* @file MatrixMarketFile.cpp
* @brief Implementation of the MatrixMarketFile class.
* @author Dominique LaSalle <dominique@solidlake.com>
* Copyright 2017
* @version 1
* @date 2017-07-28
*/




#include "MatrixMarketFile.hpp"
#include <map>
#include "Exception.hpp"
#include "Util.hpp"




namespace WildRiver
{


/******************************************************************************
* CONSTANTS *******************************************************************
******************************************************************************/


namespace
{


size_t const BUFFER_SIZE = 1024;

std::vector<std::string> const EXTENSIONS{".csr"};


enum matrix_market_attributes {
  MATRIX_MARKET_NULL,
  MATRIX_MARKET_MATRIX,
  MATRIX_MARKET_VECTOR,
  MATRIX_MARKET_GENERAL,
  MATRIX_MARKET_SYMMETRIC,
  MATRIX_MARKET_PATTERN,
  MATRIX_MARKET_REAL,
  MATRIX_MARKET_INTEGER,
  MATRIX_MARKET_COMPLEX,
  MATRIX_MARKET_ARRAY,
  MATRIX_MARKET_COORDINATE
};


std::string const BASE_HEADER("%%MatrixMarket");


std::map<std::string, int> const ENTITY_MAPPING{
  {"matrix", MATRIX_MARKET_MATRIX},
  {"vector", MATRIX_MARKET_VECTOR}
};


std::map<std::string, int> const STORAGE_MAPPING{
  {"general", MATRIX_MARKET_GENERAL},
  {"symmetric", MATRIX_MARKET_SYMMETRIC}
};


std::map<std::string, int> const FORMAT_MAPPING{
  {"array", MATRIX_MARKET_ARRAY},
  {"coordinate", MATRIX_MARKET_COORDINATE}
};


std::map<std::string, int> const TYPE_MAPPING{
  {"real", MATRIX_MARKET_REAL},
  {"complex", MATRIX_MARKET_REAL},
  {"integer", MATRIX_MARKET_INTEGER},
  {"pattern", MATRIX_MARKET_PATTERN}
};




}




/******************************************************************************
* HELPER FUNCTIONS ************************************************************
******************************************************************************/


namespace
{


bool isComment(
    std::string const & line) noexcept
{
  return line.size() > 0 && line[0] == '%';
}


/**
 * @brief Parse a line containing a triplet (row, column, value).
 *
 * @tparam D The type of the 'row' and 'column'.
 * @tparam V The type of the 'value'.
 * @param line The line.
 * @param row The row (output).
 * @param col The column (output).
 * @param val The value (output).
 */
template<typename D, typename V>
void parseTriplet(
    std::string const & line,
    D & row,
    D & col,
    V & val)
{
  char * sptr;
  std::vector<char> stringBuffer(line.c_str(), line.c_str() + line.length()+1);
  char * eptr = stringBuffer.data();
  while (true) {
    sptr = eptr;
    row = static_cast<D>(std::strtoull(sptr, &eptr, 10)-1);
    if (eptr == sptr) {
      throw BadFileException(std::string("Unable to parse triplet row: ") + \
          line);
    }

    sptr = eptr;
    col = static_cast<D>(std::strtoull(sptr, &eptr, 10)-1);
    if (eptr == sptr) {
      throw BadFileException(std::string("Unable to parse triplet " \
          "column: ") + line);
    }

    sptr = eptr;
    if (std::is_floating_point<V>::value) {
      val = static_cast<V>(std::strtod(sptr, &eptr));
    } else {
      val = static_cast<V>(std::strtoll(sptr, &eptr, 10));
    }
    if (eptr == sptr) {
      throw BadFileException(std::string("Unable to parse triplet value: ") + \
          line);
    }
  }
}




}




/******************************************************************************
* PUBLIC STATIC FUNCTIONS *****************************************************
******************************************************************************/


bool MatrixMarketFile::hasExtension(
    std::string const & f)
{
  return TextFile::matchExtension(f, EXTENSIONS);
}




/******************************************************************************
* CONSTRUCTORS / DESTRUCTOR ***************************************************
******************************************************************************/


MatrixMarketFile::MatrixMarketFile(
    std::string const & filename) :
  m_infoSet(false),
  m_line(BUFFER_SIZE,'\0'),
  m_file(filename),
  m_entity(MATRIX_MARKET_NULL),
  m_format(MATRIX_MARKET_NULL),
  m_type(MATRIX_MARKET_NULL),
  m_symmetric(false)
{
  // do nothing
}



MatrixMarketFile::~MatrixMarketFile()
{
  // do nothing
}




/******************************************************************************
* PUBLIC FUNCTIONS ************************************************************
******************************************************************************/


void MatrixMarketFile::getInfo(
    dim_t & nrows,
    dim_t & ncols,
    ind_t & nnz)
{
  // read the special header
  if (!m_file.nextLine(m_line)) {
    throw BadFileException(std::string("Not a valid MatrixMarket file '") + \
        m_file.getFilename() + std::string("': is empty."));
  }
  if (m_line.find(BASE_HEADER + std::string(" ")) != 0) {
    throw BadFileException(std::string("Not a valid MatrixMarket file '") + \
        m_file.getFilename() + std::string("': first line is: '") + m_line + \
      std::string("'."));
  }
  
  // discard base header
  m_line = m_line.substr(BASE_HEADER.length()+1);

  // break rest of line up into whitespace delimited tokens
  const std::vector<std::string> chunks = Util::split(m_line);

  if (chunks.size() == 0) {
    throw BadFileException(std::string("Not a valid MatrixMarket file '") + \
        m_file.getFilename() + std::string("': first line is: '") + m_line + \
      std::string("'."));
  }

  // figure out the entity 
  m_entity = ENTITY_MAPPING.at(chunks[0]);

  if (m_entity == MATRIX_MARKET_MATRIX) {
    // parse out matrix data
    if (chunks.size() < 2) {
      throw BadFileException(std::string("Missing 'coordinate' or 'array' " \
            "specifier for matrix '") + m_file.getFilename() + \
            std::string("'."));
    }
    m_format = FORMAT_MAPPING.at(chunks[1]);

    if (chunks.size() < 3) {
      throw BadFileException(std::string("Missing 'real', " \
            "'integer', or 'pattern' specifier for matrix '") + \
          m_file.getFilename() + std::string("'."));
    }
    m_type = FORMAT_MAPPING.at(chunks[2]);
    if (m_type == MATRIX_MARKET_COMPLEX) {
      throw BadFileException("Complex numbers are not supported");
    }

    if (chunks.size() < 4) {
      throw BadFileException(std::string("Missing 'general' or " \
          "'symmetric' specifier for matrix '") + m_file.getFilename() + \
          std::string("'."));
    }
    m_symmetric = STORAGE_MAPPING.at(chunks[3]) == MATRIX_MARKET_SYMMETRIC; 

    if (m_format == MATRIX_MARKET_COORDINATE) {
      // read past all comments until we get to the size line
      do {
        if (!m_file.nextLine(m_line)) {
          throw BadFileException(std::string("Failed to find header line " \
              "in '") + m_file.getFilename() + std::string("'."));
        }
      } while (isComment(m_line));

      parseTriplet(m_line, nrows, ncols, nnz);
    } else {
      throw BadFileException("Array matrices are not yet supported.");
    }
  } else if (m_entity == MATRIX_MARKET_VECTOR) {
    // parse out vector data
    throw BadFileException("Vectors are currently unsupported.");
  } else {
    throw BadFileException(std::string("Unknown matrix type code ") + \
        std::to_string(m_type));
  }

  m_infoSet = true;
}


void MatrixMarketFile::read(
    ind_t * const rowptr,
    dim_t * const rowind,
    val_t * const rowval,
    double * progress)
{

}


void MatrixMarketFile::setInfo(
    dim_t const nrows,
    dim_t const ncols,
    ind_t const nnz)
{

}


void MatrixMarketFile::write(
    ind_t const * const rowptr,
    dim_t const * const rowind,
    val_t const * const rowval)
{

}






}
