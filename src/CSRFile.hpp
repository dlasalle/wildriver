/**
 * @file CSRFile.hpp
 * @brief Class for reading/writing metis files.
 * @author Dominique LaSalle <wildriver@domnet.org>
 * Copyright 2015-2016
 * @version 1
 *
 */




#ifndef WILDRIVER_CSRFILE_HPP
#define WILDRIVER_CSRFILE_HPP




#include "IMatrixReader.hpp"
#include "IMatrixWriter.hpp"
#include "TextFile.hpp"




namespace WildRiver {


class CSRFile : 
  public IMatrixReader,
  public IMatrixWriter
{
  public:
    /**
     * @brief Name of this filetype.
     */
    static std::string const NAME;


    /**
     * @brief Check if the given filename matches an extension for this 
     * filetype.
     *
     * @param f The filename.
     *
     * @return True if the extension matches this filetype.
     */
    static bool hasExtension(
        std::string const & f);


    /**
     * @brief Create a new CSRFile for reading and writing.
     *
     * @param fname The filename/path.
     */
    CSRFile(
        std::string const & fname);


    /**
     * @brief Close file and free any memory.
     */
    virtual ~CSRFile();


    /**
     * @brief Get the sparse matrix in CSR form. The pointers must be
     * pre-allocated to the sizes required by the info of the matrix 
     *
     * |rowptr| = nrows + 1
     * |rowind| = nnz
     * |rowval| = nnz
     *
     * @param rowptr The row pointer indicating the start of each row.
     * @param rowind The row column indexs (i.e., for each element in a row,
     * the column index corresponding to that element).
     * @param rowval The row values.
     * @param progress The variable to update as the matrix is loaded (may be
     * null).
     */
    virtual void read(
        ind_t * rowptr,
        dim_t * rowind,
        val_t * rowval,
        double * progress) override;


    /**
     * @brief Write the given CSR structure to teh file. The information for
     * the matrix must already be set.
     *
     * @param rowptr The row pointer indicating the start of each row.
     * @param rowind The row column indexs (i.e., for each element in a row,
     * the column index corresponding to that element).
     * @param rowval The row values.
     */
    virtual void write(
        ind_t const * rowptr,
        dim_t const * rowind,
        val_t const * rowval) override;


  private:
    /**
     * @brief The number of rows in the matrix.
     */
    dim_t m_numRows;


    /**
     * @brief The number of columns in the matrix.
     */
    dim_t m_numCols;


    /**
     * @brief The number of non-zeros in the matrix.
     */
    ind_t m_nnz;


    /**
    * @brief The current row being processed.
    */
    dim_t m_currentRow;


    /**
     * @brief A buffer for reading each line.
     */
    std::string m_line;


    /**
     * @brief The underlying text file.
     */
    TextFile m_file;


    /**
    * @brief Get the next non-comment line from the file.
    *
    * @param line The line buffer.
    *
    * @return True if a line filled the buffer.
    */
    bool nextNoncommentLine(
        std::string & line);


    /**
     * @brief Reset the current position in the matrix file to the first row.
     */
    void firstRow();


    /**
     * @brief Get the next row in the matrix (adjacecny list in the graph).
     *
     * @param numNonZeros The number of non-zeros in the row (output).
     * @param columns The column of each non-zero entry (must be of length at
     * least the number of non-zero entries).
     * @param values The value of each non-zero entry (must be null or of 
     * length at least the number of non-zero entries).
     *
     * @return True if another row was found in the file.
     */
    bool getNextRow(
        dim_t * numNonZeros,
        dim_t * columns,
        val_t * values);


    /**
     * @brief Set the next row in the matrix file.
     *
     * @param next The row to set.
     */
    void setNextRow(
        std::vector<matrix_entry_struct> const & next);


    /**
     * @brief Read the header of this matrix file. Populates internal fields
     * with the header information.
     */
    void readHeader();


    /**
     * @brief Write the header of this matrix file. The header consists of
     * internal fields set by "setInfo()".
     */
    void writeHeader(); 


    /**
     * @brief Determine the given line is a comment.
     *
     * @param line The line.
     *
     * @return True if the line is a comment.
     */
    bool isComment(
        std::string const & line) const noexcept;







};




}




#endif
