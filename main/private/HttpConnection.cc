/*
 * Copyright ©2022 Dylan Hartono.  All rights reserved.
 */

#include <stdint.h>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <map>
#include <string>
#include <vector>
#include <iostream>

#include "./HttpRequest.h"
#include "./HttpUtils.h"
#include "./HttpConnection.h"

using std::map;
using std::string;
using std::vector;

namespace hw4 {

static const char* kHeaderEnd = "\r\n\r\n";
static const int kHeaderEndLen = 4;

bool HttpConnection::GetNextRequest(HttpRequest* const request) {
  // Use WrappedRead from HttpUtils.cc to read bytes from the files into
  // private buffer_ variable. Keep reading until:
  // 1. The connection drops
  // 2. You see a "\r\n\r\n" indicating the end of the request header.
  //
  // Read in a large amount of bytes each time it calls WrappedRead.
  //
  // After reading complete request header, use ParseRequest() to parse into
  // an HttpRequest and save to the output parameter request.
  //
  // Important note: Clients may send back-to-back requests on the same socket.
  // This means WrappedRead may also end up reading more than one request.
  // Make sure to save anything you read after "\r\n\r\n" in buffer_ for the
  // next time the caller invokes GetNextRequest()!

  // STEP 1:
  // header end in buffer_?
  size_t found = buffer_.find(kHeaderEnd);
  // nope, keep reading
  if (found == string::npos) {
    unsigned char buf[1024];
    int read = -1;
    while (read != 0 && found == string::npos) {
      read = WrappedRead(fd_, buf, 1024);
      if (read == -1) {
        // crashed, exit
        return false;
      } else if (read == 0) {
        // no bytes readd, try again
        continue;
      } else {
        // got something useful, add to buffer_
        buffer_.append(reinterpret_cast<char*>(buf), read);
        // header end in buffer now?
        found = buffer_.find(kHeaderEnd);
      }
    }
  }
  *request = ParseRequest(buffer_.substr(0, found + kHeaderEndLen));
  buffer_.erase(0, found + kHeaderEndLen);
  return true;
}

bool HttpConnection::WriteResponse(const HttpResponse& response) const {
  string str = response.GenerateResponseString();
  int res = WrappedWrite(fd_,
                         reinterpret_cast<const unsigned char*>(str.c_str()),
                         str.length());
  if (res != static_cast<int>(str.length()))
    return false;
  return true;
}

HttpRequest HttpConnection::ParseRequest(const string& request) const {
  HttpRequest req("/");  // by default, get "/".

  // Plan for STEP 2:
  // 1. Split the request into different lines (split on "\r\n").
  // 2. Extract the URI from the first line and store it in req.URI.
  // 3. For the rest of the lines in the request, track the header name and
  //    value and store them in req.headers_ (e.g. HttpRequest::AddHeader).
  //
  // Look up boost functions for:
  // - Splitting a string into lines on a "\r\n" delimiter
  // - Trimming whitespace from the end of a string
  // - Converting a string to lowercase.
  //
  // Note: If a header is malformed, skip that line.

  // STEP 2:
  vector<string> lines;
  // get lines
  boost::split(lines, request, boost::is_any_of("\r\n"),
               boost::token_compress_on);
  for (int i = 0; i < static_cast<int>(lines.size()); i++) {
    boost::trim(lines[i]);
  }
  // the main line (before the headers)
  vector<string> words;
  boost::split(words, lines[0], boost::is_any_of(" "),
               boost::token_compress_on);
  boost::trim(words[1]);
  req.set_uri(words[1]);
  // headers
  for (int i = 1; i < static_cast<int>(lines.size()); i++) {
    boost::split(words, lines[i], boost::is_any_of(": "),
                 boost::token_compress_on);
    if (words.size() == 2) {
    boost::algorithm::to_lower(words[0]);
    boost::algorithm::to_lower(words[1]);
    boost::trim(words[0]);
    boost::trim(words[1]);
    req.AddHeader(words[0], words[1]);
    }
  }
  return req;
}

}  // namespace hw4
