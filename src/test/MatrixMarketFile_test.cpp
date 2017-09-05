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

namespace
{

std::vector<ind_t> rowptrGeneral{
  0, 2, 4, 7, 10, 12, 14
};

std::vector<dim_t> rowindGeneral{
  1, 2,
  0, 2,
  0, 1, 3,
  2, 4, 5,
  3, 5,
  3, 4
};

std::vector<val_t> rowvalGeneral{
  1, 2,
  3, 4,
  5, 6, 7,
  8, 9, 1,
  2, 3,
  4, 5
};

void writeInitialGeneral(
    std::string const & testFile)
{
  std::ofstream f(testFile);

  f << "%%MatrixMarket matrix coordinate real general" << std::endl;
  f << "%=======================================================" << std::endl;
  f << "%" << std::endl;
  f << "%=======================================================" << std::endl;

  dim_t nrows = static_cast<dim_t>(rowptrGeneral.size()-1);
  ind_t nnz = static_cast<ind_t>(rowptrGeneral[nrows]);

  f << nrows << " " << nrows << " " << nnz << std::endl;

  for (dim_t row = 0; row < nrows; ++row) {
    for (ind_t colIdx = rowptrGeneral[row]; colIdx < rowptrGeneral[row+1];
        ++colIdx) {
      dim_t col = rowindGeneral[colIdx];
      f << " " << (row+1) << " " << (col+1) << " " <<
          rowvalGeneral[colIdx] << std::endl;
    }
  }
}


std::vector<ind_t> rowptrSymmetric{
  0, 2, 5, 6, 8
};

std::vector<dim_t> rowindSymmetric{
  0, 1,
  0, 1, 3,
  2,
  1, 3
};

std::vector<val_t> rowvalSymmetric{
  1, 2,
  2, 4, -1,
  5,
  8, 9
};

void writeInitialSymmetric(
    std::string const & testFile)
{
  std::ofstream f(testFile);

  f << "%%MatrixMarket matrix coordinate real symmetric" << std::endl;
  f << "%=======================================================" << std::endl;
  f << "%" << std::endl;
  f << "%=======================================================" << std::endl;

  dim_t nrows = static_cast<dim_t>(rowptrSymmetric.size()-1);
  ind_t nnz = static_cast<ind_t>(rowptrSymmetric[nrows]);

  f << nrows << " " << nrows << " " << nnz << std::endl;

  for (dim_t row = 0; row < nrows; ++row) {
    for (ind_t colIdx = rowptrSymmetric[row]; colIdx < rowptrSymmetric[row+1];
        ++colIdx) {
      dim_t col = rowindSymmetric[colIdx];
      f << " " << (row+1) << " " << (col+1) << " " <<
          rowvalSymmetric[colIdx] << std::endl;
    }
  }
}


void writeTestGeneral(
    std::string const & testFile)
{
  MatrixMarketFile mm(testFile);

  dim_t nrows = static_cast<dim_t>(rowptrGeneral.size()-1);
  ind_t nnz = static_cast<ind_t>(rowptrGeneral[nrows]);

  mm.setInfo(nrows, nrows, nnz);

  mm.write(rowptrGeneral.data(),rowindGeneral.data(),rowvalGeneral.data());
}


void readTestGeneral(
    std::string const & testFile)
{
  MatrixMarketFile mm(testFile);

  wildriver_dim_t nrows, ncols;
  wildriver_ind_t nnz;

  mm.getInfo(nrows,ncols,nnz);

  testEquals(nrows,rowptrGeneral.size()-1);
  testEquals(ncols,rowptrGeneral.size()-1);
  testEquals(nnz,rowptrGeneral[nrows]);

  std::unique_ptr<wildriver_ind_t[]> rowptr(new wildriver_ind_t[nrows+1]);
  std::unique_ptr<wildriver_dim_t[]> rowind(new wildriver_dim_t[nnz]);
  std::unique_ptr<wildriver_val_t[]> rowval(new wildriver_val_t[nnz]);

  mm.read(rowptr.get(),rowind.get(),rowval.get(),nullptr);

  // test rowptr
  for (dim_t i = 0; i < rowptrGeneral.size(); ++i) {
    testEquals(rowptr[i], rowptrGeneral[i]);
  }

  // test rowind
  for (ind_t i = 0; i < rowindGeneral.size(); ++i) {
    testEquals(rowind[i], rowindGeneral[i]);
  }

  // test rowval
  for (ind_t i = 0; i < rowvalGeneral.size(); ++i) {
    testEquals(rowval[i], rowvalGeneral[i]);
  }
}

}

void Test::run()
{
  // general test
  {
    std::string testFile("/tmp/MatrixMarketGeneral_test.mtx");

    writeInitialGeneral(testFile);
    readTestGeneral(testFile);

    removeFile(testFile);

    writeTestGeneral(testFile);
    readTestGeneral(testFile);

    removeFile(testFile);
  }

/*
  // symmetric test
  {
    std::string testFile("/tmp/MatrixMarketSymmetric_test.mtx");

    writeInitialSymmetric(testFile);
    readTest(testFile);

    removeFile(testFile);

    writeTest(testFile);
    readTest(testFile);

    removeFile(testFile);
  }
  */
}




}
