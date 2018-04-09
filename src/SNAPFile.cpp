/**
* @file SNAPFile.cpp
* @brief Implementation of the SNAPFile class.
* @author Dominique LaSalle <dominique@solidlake.com>
* Copyright 2018
* @version 1
* @date 2018-04-09
*/




#include "SNAPFile.hpp"
#include "CoordinateWriter.hpp"



namespace WildRiver
{


/******************************************************************************
* PRIVATE METHODS *************************************************************
******************************************************************************/


void SNAPFile::writeHeader()
{
  // set header to write mode if not already
  if (!m_file.isOpenWrite()) {
    m_file.openWrite();
  }

  // for now we assume this is an undirected graph
  m_file.setNextLine(std::string("# Undirected graph (each unordered pair " \
      "of nodes is saved once): ") + m_file.getFilename()); 
  m_file.setNextLine("# A graph.");
  m_file.setNextLine(std::string("# Nodes: ") + \
      std::to_string(m_numVertices) + \
      std::string(" Edges: ") + std::to_string(m_numEdges));
  if (m_hasEdgeWeights) {
    m_file.setNextLine("# FromNodeId\tToNodeId\tWeight");
  } else {
    m_file.setNextLine("# FromNodeId\tToNodeId");
  }
}

void SNAPFile::readHeader()
{
  if (!m_file.isOpenRead()) {
    // open our file for reading if not already
    m_file.openRead();
  }

  // get the first line
  std::string line;
  if (!m_file.nextLine(line)) {
    throw 
  }

  // default weight information uncase its not preset
  m_hasEdgeWeights = false;

  m_numVertices =
  m_numEdges =


}

/******************************************************************************
* CONSTRUCTORS / DESTRUCTOR ***************************************************
******************************************************************************/


SNAPFile::SNAPFile(
    std::string const & filename) :
  m_infoSet(false),
  m_numVertices(0),
  m_numEdges(0),
  m_hasEdgeWeights(false),
  m_line(),
  m_file(filename)
{
  // do nothing
}


SNAPFile::~SNAPFile()
{
  // do nothing
}


/******************************************************************************
* PUBLIC METHODS **************************************************************
******************************************************************************/


void SNAPFile::read(
    ind_t * const xadj,
    dim_t * const adjncy,
    val_t * const vwgt,
    val_t * const adjwgt,
    double * progress)
{
}


void SNAPFile::getInfo(
    dim_t & nvtxs,
    ind_t & nedges,
    int & nvwgt,
    bool & ewgts)
{
  readHeader();

  nvtxs = m_numVertices;
  nedges = m_numEdges;
  nvwgt = 0;
  ewgts = m_hasEdgeWeights;

  m_infoSet(true);
}


void SNAPFile::write(
    ind_t const * const xadj,
    dim_t const * const adjncy,
    val_t const *,
    val_t const * const adjwgt)
{
  CoordinateWriter writer(&m_file);

  std::vector<dim_t> neighbors;
  std::vector<val_t> weights;
  for (dim_t i = 0; i < m_numVertices; ++i) {
    neighbors.clear();
    weights.clear();

    // build edges
    for (ind_t j=xadj[i]; j<xadj[i+1]; ++j) {
      neighbors.emplace_back(adjncy[j]);
      if (m_hasEdgeWeights) {
        if (adjwgt) {
          weights.emplace_back(adjwgt[j]);
        } else {
          weights.emplace_back(1);
        }
      }
    }

    // set the vertex
    if (m_hasEdgeWEights) {
      writer.setNextRow(xadj[i+1]-xadj[i], neighbors.data(), weights.data());
    } else {
      writer.setNextRow(xadj[i+1]-xadj[i], neighbors.data(), nullptr);
    }
  }
}


void SNAPFile::setInfo(
    dim_t const nvtxs,
    ind_t const nedges,
    int const,
    bool const ewgts)
{
  m_numVertices = nvtxs;
  m_numEdges = nedges;
  m_hasEdgeWeights = ewgts;

  m_infoSet = true;

  writeHeader();
}



}
