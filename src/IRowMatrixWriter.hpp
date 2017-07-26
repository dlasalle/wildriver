/**
* @file IRowMatrixWriter.hpp
* @brief An interface for writing out matrices row by row.
* @author Dominique LaSalle <dominique@solidlake.com>
* Copyright 2017, Solid Lake LLC
* @version 1
* @date 2017-07-26
*/





#ifndef WILDRIVER_IROWMATRIXWRITER_HPP
#define WILDRIVER_IROWMATRIXWRITER_HPP



namespace WildRiver
{


class IRowMatrixWriter
{
  public:
    /**
    * @brief Destructor.
    */
    virtual ~IRowMatrixWriter()
    {
      // do nothing
    }

    /**
     * @brief Write the header of this matrix file. The header consists of
     * internal fields set by "setInfo()".
     */
    virtual void writeHeader(
        dim_t nrows,
        dim_t ncols,
        ind_t nnz) = 0;


    /**
     * @brief Set the next row in the matrix file.
     *
     * @param next The row to set.
     */
    virtual void setNextRow(
        std::vector<matrix_entry_struct> const & next) = 0;




}




}




#endif
