/*
 * Copyright ©2022 Dylan Hartono.  All rights reserved.
 */

#ifndef QUERYPROCESSOR_H_
#define QUERYPROCESSOR_H_

#include <list>
#include <string>
#include <vector>

#include "DocIDTableReader.h"
#include "DocTableReader.h"
#include "FileIndexReader.h"
#include "IndexTableReader.h"
#include "Utils.h"

using std::list;
using std::vector;
using std::string;

namespace hw3 {

// A QueryProcessor is a class that is given a set of names of index
// files, and uses the various FileIndexReader and HashTableReader
// classes to process queries against the indices.
class QueryProcessor {
 public:
  // Construct a QueryProcessor.
  // Arguments:
  // - indexlist: a list<string> containing a list of index
  //   file names that the QueryProcessor should use.
  // - validate: a bool indicating whether or not to validate the
  //   checksums in the index files.  Defaults to true.
  explicit QueryProcessor(const list<string>& index_list,
                          bool validate = true);

  // The destructor.
  ~QueryProcessor();

  // This structure defines a single query result.  As with part2,
  // the rank of a query result is the sum of the number of occurrences
  // of query words within the document.
  class QueryResult {
   public:
    bool operator<(const QueryResult& rhs) const { return rank > rhs.rank; }

    string document_name;  // The name of a matching document.
    int    rank;           // The rank of the matching document.
  };

  // This method processes a query against the indices and returns a
  // vector of QueryResults, sorted in descending order of rank.  If no
  // documents match the query, then a valid but empty vector will be
  // returned.
  vector<QueryResult> ProcessQuery(const vector<string>& query) const;

 protected:
  // The list of index files we process.
  list<string> index_list_;

  // The arrays of pointers to DocTableReader and IndexTableReader
  // objects.
  int                 array_len_;
  DocTableReader**    dtr_array_;
  IndexTableReader**  itr_array_;

 private:
  DISALLOW_COPY_AND_ASSIGN(QueryProcessor);
};

}  // namespace hw3

#endif  // QUERYPROCESSOR_H_
