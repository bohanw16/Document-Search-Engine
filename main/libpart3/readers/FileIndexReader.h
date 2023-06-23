/*
 * Copyright ©2022 Dylan Hartono.  All rights reserved.
 */

#ifndef FILEINDEXREADER_H_
#define FILEINDEXREADER_H_

#include <string>    // for std::string
#include <cstdio>    // for (FILE*)

#include "DocTableReader.h"
#include "IndexTableReader.h"
#include "LayoutStructs.h"
#include "Utils.h"

using std::string;

namespace hw3 {

// A FileIndexReader is used to access an index file, read the header
// information out of it,  and manufacture DocTableReader and
// IndexTableReader hash table accessors.  (To manufacture a
// DocIDTableReader, you use a manufactured IndexTableReader.)
class FileIndexReader {
 public:
  // Arguments:
  // - file_name: the index file to load.
  // - validate: whether to validate the checksums (default=true).
  explicit FileIndexReader(const string& file_name, bool validate = true);
  ~FileIndexReader();

  // Manufactures and returns a DocTableReader for this index file.
  // A DocTableReader is a HashTableReader subclass that is
  // specialized to read the docid-->filename hashtable within this
  // index file. (See DocTableReader.h for details.)
  DocTableReader* NewDocTableReader() const;

  // Manufactures and returns an IndexTableReader for this index file.
  // An IndexTableReader is a HashTableReader subclass that is
  // specialized to read the word-->docidtable hashtable within this
  // index file. (See IndexTableReader.h for details.)
  IndexTableReader* NewIndexTableReader() const;

  // Returns a const reference to the file header information.
  const IndexFileHeader& getHeader() const { return header_; }

 protected:
  // The name of the index file we're reading.
  string file_name_;

  // The stdio.h (FILE*) for the file.
  FILE* file_;

  // A cached copy of file header.
  IndexFileHeader header_;

 private:
  // This friend declaration is here so that the Test_FileIndexReader
  // unit test fixture can access protected member variables of
  // FileIndexReader.
  friend class Test_FileIndexReader;

  DISALLOW_COPY_AND_ASSIGN(FileIndexReader);
};

}  // namespace hw3

#endif  // FILEINDEXREADER_H_
