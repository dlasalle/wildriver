/**
 * @file MatrixMarketFile_test.cpp
 * @brief Test for reading and writing CSR formatted csrs.
 * @author Dominique LaSalle <wildriver@domnet.org>
 * Copyright 2015
 * @version 1
 *
 */




#include <iostream>
#include <fstream>
#include <memory>

#include "MatrixMarketFile.hpp"
#include "DomTest.hpp"




using namespace WildRiver;




namespace DomTest
{


static void writeInitial(
    std::string const & testFile)
{
  std::ofstream f(testFile);

  f << "%%MatrixMarket matrix coordinate real general" << std::endl;
  f << "%=================================================================================" << std::endl;
  f << "%" << std::endl;
  f << "% This ASCII file represents a sparse MxN matrix with L " << std::endl;
  f << "% nonzeros in the following Matrix Market format:" << std::endl;
  f << "%" << std::endl;
  f << "% +----------------------------------------------+" << std::endl;
  f << "% |%%MatrixMarket matrix coordinate real general | <--- header line" << std::endl;
  f << "% |%                                             | <--+" << std::endl;
  f << "% |% comments                                    |    |-- 0 or more comment lines" << std::endl;
  f << "% |%                                             | <--+         " << std::endl;
  f << "% |    M  N  L                                   | <--- rows, columns, entries" << std::endl;
  f << "% |    I1  J1  A(I1, J1)                         | <--+" << std::endl;
  f << "% |    I2  J2  A(I2, J2)                         |    |" << std::endl;
  f << "% |    I3  J3  A(I3, J3)                         |    |-- L lines" << std::endl;
  f << "% |        . . .                                 |    |" << std::endl;
  f << "% |    IL JL  A(IL, JL)                          | <--+" << std::endl;
  f << "% +----------------------------------------------+   " << std::endl;
  f << "%" << std::endl;
  f << "% Indices are 1-based, i.e. A(1,1) is the first element." << std::endl;
  f << "%" << std::endl;
  f << "%=================================================================================" << std::endl;
  f << "  6  6  14" << std::endl;
  f << "    1     2   1" << std::endl;
  f << "    1     3   2" << std::endl;
  f << "    2     1   3" << std::endl;
  f << "    2     3   4" << std::endl;
  f << "    3     1   5" << std::endl;
  f << "    3     2   6" << std::endl;
  f << "    3     4   7" << std::endl;
  f << "    4     3   8" << std::endl;
  f << "    4     5   9" << std::endl;
  f << "    4     6   1" << std::endl;
  f << "    5     4   2" << std::endl;
  f << "    5     6   3" << std::endl;
  f << "    6     4   4" << std::endl;
  f << "    6     5   5" << std::endl;
}


static void writeTest(
    std::string const & testFile)
{
  MatrixMarketFile mm(testFile);

  mm.setInfo(6,6,14);

  wildriver_ind_t rowptr[] = {0,2,4,7,10,12,14};
  wildriver_dim_t rowind[] = {1,2, 0,2, 0,1,3, 2,4,5, 3,5, 3,4};
  wildriver_val_t rowval[] = {1,2, 3,4, 5,6,7, 8,9,1, 2,3, 4,5};

  mm.write(rowptr,rowind,rowval);
}


static void readTest(
    std::string const & testFile)
{
  MatrixMarketFile mm(testFile);

  wildriver_dim_t nrows, ncols;
  wildriver_ind_t nnz;

  mm.getInfo(nrows,ncols,nnz);

  testEquals(nrows,6);
  testEquals(ncols,6);
  testEquals(nnz,14);

  std::unique_ptr<wildriver_ind_t[]> rowptr(new wildriver_ind_t[nrows+1]);
  std::unique_ptr<wildriver_dim_t[]> rowind(new wildriver_dim_t[nnz]);
  std::unique_ptr<wildriver_val_t[]> rowval(new wildriver_val_t[nnz]);

  mm.read(rowptr.get(),rowind.get(),rowval.get(),nullptr);

  // test rowptr
  testEquals(rowptr[0],0);
  testEquals(rowptr[1],2);
  testEquals(rowptr[2],4);
  testEquals(rowptr[3],7);
  testEquals(rowptr[4],10);
  testEquals(rowptr[5],12);
  testEquals(rowptr[6],14);

  // test rowind
  testEquals(rowind[0],1);
  testEquals(rowind[1],2);
  testEquals(rowval[0],1);
  testEquals(rowval[1],2);

  testEquals(rowind[2],0);
  testEquals(rowind[3],2);
  testEquals(rowval[2],3);
  testEquals(rowval[3],4);

  testEquals(rowind[4],0);
  testEquals(rowind[5],1);
  testEquals(rowind[6],3);
  testEquals(rowval[4],5);
  testEquals(rowval[5],6);
  testEquals(rowval[6],7);

  testEquals(rowind[7],2);
  testEquals(rowind[8],4);
  testEquals(rowind[9],5);
  testEquals(rowval[7],8);
  testEquals(rowval[8],9);
  testEquals(rowval[9],1);

  testEquals(rowind[10],3);
  testEquals(rowind[11],5);
  testEquals(rowval[10],2);
  testEquals(rowval[11],3);

  testEquals(rowind[12],3);
  testEquals(rowind[13],4);
  testEquals(rowval[12],4);
  testEquals(rowval[13],5);
}


void Test::run()
{
  std::string testFile("/tmp/MatrixMarketFile_test.mtx");

  writeInitial(testFile);
  readTest(testFile);

  removeFile(testFile);

  writeTest(testFile);
  readTest(testFile);

  removeFile(testFile);
}




}
