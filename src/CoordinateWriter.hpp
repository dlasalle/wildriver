/**
* @file CoordinateWriter.hpp
* @brief The CoordinateWriter class.
* @author Dominique LaSalle <dominique@solidlake.com>
* Copyright 2018
* @version 1
* @date 2018-04-07
*/




#ifndef WILDRIVER_COORDINATEENCODER_HPP
#define WILDRIVER_COORDINATEENCODER_HPP




#include "IRowMatrixWriter.hpp"
#include "Text.File.hpp"
#include <memory>




namespace WildRiver
{



/**
* @brief A class for writing a matrix row by row from a CSR data structure into
* a ijv style text format:
*
* ```
* row_1 col_1 val_1
* row_2 col_2 val_2
* ```
*/
class CoordinateWriter : public IRowMatrixWriter
{
  public:
    /**
     * @brief Create a new CoordinateWriter.
     *
     * @param file The file to write to.
     */
    CoordinateWriter(
        TextFile * file);


    /**
     * @brief Write the header of this matrix file. The header consists of
     * internal fields set by "setInfo()".
     */
    virtual void writeHeader(
        dim_t nrows,
        dim_t ncols,
        ind_t nnz) override;


    /**
     * @brief Set the next row in the matrix file.
     *
     * @param numNonZeros The number of non-zeros in the row.
     * @param columns The column IDs.
     * @param values The values.
     */
    virtual void setNextRow(
        dim_t numNonZeros,
        dim_t const * columns,
        val_t const * values) override;




  private:
    /**
    * @brief The text file.
    */
    TextFile m_file;



};


}

#endif
