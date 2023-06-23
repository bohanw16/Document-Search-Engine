/*
 * Copyright ©2022 Dylan Hartono.  All rights reserved.
 */

#ifndef WRITEINDEXFILE_H_
#define WRITEINDEXFILE_H_

// Include the part1 and part2 headers.  We'll need these to access the
// routines in the libhw1.a and libhw2.a libraries.  Note that those
// libraries were compiled with gcc, and therefore have "C"-style
// linkage.  Since WriteIndexFile.cc is compiled by g++, we need
// do use 'extern "C"' to tell g++ that the routines accessed through
// these header files were compiled with gcc.
extern "C" {
  #include "../libpart1/CSE333.h"
  #include "../libpart2/DocTable.h"
  #include "../libpart2/MemIndex.h"
}

namespace hw3 {

// Writes the contents of a MemIndex and the docid_to_docname mapping of a
// DocTable into an index file.  The on-disk representation is defined in
// detail on the part3 web page.
//
// Arguments:
//   - mi: the MemIndex to write.
//   - dt: the DocTable to write.
//   - file_name: a C-style string containing the name of the index
//     file that we should create.
//
// Returns:
//   - the resulting size of the index file, in bytes, or negative value
//     on error
int WriteIndexFile(MemIndex* mem_index, DocTable* doctable,
                    const char* file_name);

}  // namespace hw3

#endif  // WRITEINDEXFILE_H_
