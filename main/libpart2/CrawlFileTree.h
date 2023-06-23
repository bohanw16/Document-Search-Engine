/*
 * Copyright ©2022 Dylan Hartono.  All rights reserved.
 */

#ifndef HW2_PUBLIC_CRAWLFILETREE_H_
#define HW2_PUBLIC_CRAWLFILETREE_H_

#include <stdbool.h>

#include "DocTable.h"
#include "MemIndex.h"

// Crawls a directory, indexing ASCII text files.
//
// CrawlFileTree crawls the filesystem subtree rooted at directory "rootdir".
// For each file that it encounters, it scans the file to test whether it
// contains ASCII text data.  If so, it indexes the file.
//
// Arguments:
// - rootdir: the name of the directory which is the root of the crawl.
//
// Returns:
// - doctable: an output parameter through which a populated DocTable is
//   returned.  All indexed files are represented in the DocTable.  If
//   populated, the caller is responsible for deallocating the returned
//   DocTable.
// - index: an output parameter through which an inverted index is returned.
//   All indexed files are represented in the inverted index.  If populated,
//   the caller is responsible for deallocating the returned MemIndex.
//
// - Returns false on failure (nothing is allocated), true on success.
bool CrawlFileTree(char* root_dir, DocTable** doctable, MemIndex** index);

#endif  // HW2_PUBLIC_CRAWLFILETREE_H_
