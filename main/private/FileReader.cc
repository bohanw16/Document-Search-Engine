/*
 * Copyright ©2022 Dylan Hartono.  All rights reserved.
 */

#include <stdio.h>
#include <string>
#include <cstdlib>
#include <iostream>
#include <sstream>

extern "C" {
  #include "libpart2/FileParser.h"
}

#include "./HttpUtils.h"
#include "./FileReader.h"

using std::string;

namespace hw4 {

bool FileReader::ReadFile(string* const contents) {
  string full_file = basedir_ + "/" + fname_;

  // Read the file into memory, and store the file contents in the
  // output parameter "str."
  //
  // This function uses malloc to allocate memory, so it needs to use
  // free() to free up that memory after copying to the string output
  // parameter.

  // STEP 1:
  // check if bsae dir contains file's dir
  if (!IsPathSafe(basedir_, full_file)) {
    return false;
  }
  int size;
  char* actcont;
  actcont = ReadFileToString(full_file.c_str(), &size);
  if (actcont == NULL) {
    return false;
  }
  // successful, set the return parameter
  *contents = std::string(actcont, size);
  free(actcont);
  return true;
}

}  // namespace hw4
