/*
 * Copyright ©2022 Dylan Hartono.  All rights reserved.
 */

#include <boost/algorithm/string.hpp>
#include <iostream>
#include <map>
#include <memory>
#include <vector>
#include <string>
#include <sstream>

#include "./FileReader.h"
#include "./HttpConnection.h"
#include "./HttpRequest.h"
#include "./HttpUtils.h"
#include "./HttpServer.h"
#include "libpart3/QueryProcessor.h"

using std::cerr;
using std::cout;
using std::endl;
using std::list;
using std::map;
using std::string;
using std::stringstream;
using std::unique_ptr;

namespace hw4 {
///////////////////////////////////////////////////////////////////////////////
// Constants, internal helper functions
///////////////////////////////////////////////////////////////////////////////
static const char* kThreegleStr =
  "<html><head><title>333gle</title></head>\n"
  "<body>\n"
  "<center style=\"font-size:500%;\">\n"
  "<span style=\"position:relative;bottom:-0.33em;color:orange;\">3</span>"
    "<span style=\"color:red;\">3</span>"
    "<span style=\"color:gold;\">3</span>"
    "<span style=\"color:blue;\">g</span>"
    "<span style=\"color:green;\">l</span>"
    "<span style=\"color:red;\">e</span>\n"
  "</center>\n"
  "<p>\n"
  "<div style=\"height:20px;\"></div>\n"
  "<center>\n"
  "<form action=\"/query\" method=\"get\">\n"
  "<input type=\"text\" size=30 name=\"terms\" />\n"
  "<input type=\"submit\" value=\"Search\" />\n"
  "</form>\n"
  "</center><p>\n";

static const map<string, string> kSuffixMap = {
  {".htm",  "text/html"},
  {".html", "text/html"},
  {".jpg",  "image/jpeg"},
  {".jpeg", "image/jpeg"},
  {".png",  "image/png"},
  {".txt",  "text/plain"},
  {".js",   "text/javascript"},
  {".css",  "text/css"},
  {".xml",  "text/xml"},
  {".gif",  "image/gif"}
};

// static
const int HttpServer::kNumThreads = 100;

// This is the function that threads are dispatched into
// in order to process new client connections.
static void HttpServer_ThrFn(ThreadPool::Task* t);

// Given a request, produce a response.
static HttpResponse ProcessRequest(const HttpRequest& req,
                            const string& base_dir,
                            const list<string>& indices);

// Process a file request.
static HttpResponse ProcessFileRequest(const string& uri,
                                const string& base_dir);

// Process a query request.
static HttpResponse ProcessQueryRequest(const string& uri,
                                 const list<string>& indices);


///////////////////////////////////////////////////////////////////////////////
// HttpServer
///////////////////////////////////////////////////////////////////////////////
bool HttpServer::Run(void) {
  // Create the server listening socket.
  int listen_fd;
  cout << "  creating and binding the listening socket..." << endl;
  if (!socket_.BindAndListen(AF_INET6, &listen_fd)) {
    cerr << endl << "Couldn't bind to the listening socket." << endl;
    return false;
  }

  // Spin, accepting connections and dispatching them.  Use a
  // threadpool to dispatch connections into their own thread.
  cout << "  accepting connections..." << endl << endl;
  ThreadPool tp(kNumThreads);
  while (1) {
    HttpServerTask* hst = new HttpServerTask(HttpServer_ThrFn);
    hst->base_dir = static_file_dir_path_;
    hst->indices = &indices_;
    if (!socket_.Accept(&hst->client_fd,
                    &hst->c_addr,
                    &hst->c_port,
                    &hst->c_dns,
                    &hst->s_addr,
                    &hst->s_dns)) {
      // The accept failed for some reason, so quit out of the server.
      // (Will happen when kill command is used to shut down the server.)
      break;
    }
    // The accept succeeded; dispatch it.
    tp.Dispatch(hst);
  }
  return true;
}

static void HttpServer_ThrFn(ThreadPool::Task* t) {
  // Cast back our HttpServerTask structure with all of our new
  // client's information in it.
  unique_ptr<HttpServerTask> hst(static_cast<HttpServerTask*>(t));
  cout << "  client " << hst->c_dns << ":" << hst->c_port << " "
       << "(IP address " << hst->c_addr << ")" << " connected." << endl;

  // Read in the next request, process it, and write the response.

  // Use the HttpConnection class to read and process the next
  // request from our current client, then write out our response.  If
  // the client sends a "Connection: close\r\n" header, then shut down
  // the connection -- we're done.
  //
  // Note: the client can make multiple requests on our single connection,
  // so we should keep the connection open between requests rather than
  // creating/destroying the same connection repeatedly.

  // STEP 1:
  bool done = false;
  HttpConnection hc(hst->client_fd);
  while (!done) {
    HttpRequest req;
    // get next request
    if (!hc.GetNextRequest(&req)) {
      close(hst->client_fd);
      done = true;
    } else if (req.GetHeaderValue("connection") == "close\r\n") {
      // connection header says close
      close(hst->client_fd);
      done = true;
    }
    // process the request
    HttpResponse res = ProcessRequest(req, hst->base_dir, *hst->indices);
    if (hc.WriteResponse(res)) {
      close(hst->client_fd);
      done = true;
    }
  }
}

static HttpResponse ProcessRequest(const HttpRequest& req,
                            const string& base_dir,
                            const list<string>& indices) {
  // Is the user asking for a static file?
  if (req.uri().substr(0, 8) == "/static/") {
    return ProcessFileRequest(req.uri(), base_dir);
  }

  // The user must be asking for a query.
  return ProcessQueryRequest(req.uri(), indices);
}

static HttpResponse ProcessFileRequest(const string& uri,
                                const string& base_dir) {
  // The response we'll build up.
  HttpResponse ret;

  // Steps to follow:
  // 1. Use the URLParser class to figure out what file name
  //    the user is asking for. Note that we identify a request
  //    as a file request if the URI starts with '/static/'
  //
  // 2. Use the FileReader class to read the file into memory
  //
  // 3. Copy the file content into the ret.body
  //
  // 4. Depending on the file name suffix, set the response
  //    Content-type header as appropriate. View `kSuffixMap`
  //    for help on this.
  //
  // be sure to set the response code, protocol, and message
  // in the HttpResponse as well.
  string file_name = "";

  // STEP 2:
  URLParser parse;
  parse.Parse(uri);
  file_name = parse.path();
  file_name = file_name.substr(7);
  FileReader fr(base_dir, file_name);
  string content;

  if (fr.ReadFile(&content)) {
    size_t found = file_name.find(".");
    std::string suffix = file_name.substr(found, file_name.length() - 1);
    auto it = kSuffixMap.find(suffix);
    ret.set_content_type(it->second);
    ret.set_protocol("HTTP/1.1");
    ret.set_response_code(200);
    ret.set_message("OK");
    ret.AppendToBody(content);
    return ret;
  }

  // If you couldn't find the file, return an HTTP 404 error.
  ret.set_protocol("HTTP/1.1");
  ret.set_response_code(404);
  ret.set_message("Not Found");
  ret.AppendToBody("<html><body>Couldn't find file \""
                   + EscapeHtml(file_name)
                   + "\"</body></html>\n");
  return ret;
}

static HttpResponse ProcessQueryRequest(const string& uri,
                                 const list<string>& indices) {
  // The response we're building up.
  HttpResponse ret;

  // Notes:
  //
  // 1. The 333gle logo and search box/button should be present on the site.
  //
  // 2. If the user had previously typed in a search query, also need
  //    to display the search results.
  //
  // 3. use the URLParser to parse the uri and extract
  //    search terms from a typed-in search query.  convert them
  //    to lower case.
  //
  // 4. Initialize and use hw3::QueryProcessor to process queries with the
  //    search indices.
  //
  // 5. How to hyperlink results to file contents. (Note: Look into HTML
  //    tags!)

  // STEP 3:
  ret.set_protocol("HTTP/1.1");
  ret.set_response_code(200);
  ret.set_message("OK");

  // logog and search bar
  ret.AppendToBody(kThreegleStr);

  // input text
  URLParser parse;
  parse.Parse(uri);
  std::string input = parse.args()["terms"];
  // format the input text
  boost::trim(input);
  boost::to_lower(input);

  // make sure we have input
  if (uri.find("terms=") != std::string::npos) {
    // get individual words from the input text
    std::vector<std::string> words;
    boost::split(words, input, boost::is_any_of(" "), boost::token_compress_on);
    // set up and use the query processor
    hw3::QueryProcessor process(indices, false);
    std::vector<hw3::QueryProcessor::QueryResult> qres =
      process.ProcessQuery(words);

    if (qres.size() == 0) {
      // no result found
      ret.AppendToBody("<p><br> No results found for <b>");
      ret.AppendToBody(EscapeHtml(input));
      ret.AppendToBody("</b></p>");
    } else {
      // result(s) found
      ret.AppendToBody("<p><br>");
      ret.AppendToBody(std::__cxx11::to_string(qres.size()));
      ret.AppendToBody((qres.size() == 1) ? " result " : " results ");
      ret.AppendToBody("found for <b>");
      ret.AppendToBody(EscapeHtml(input));
      ret.AppendToBody("</b></p>");
      // unordered list for the results
      ret.AppendToBody("<ul>");
      for (uint32_t i = 0; i < qres.size(); i++) {
        ret.AppendToBody("<li>");
        ret.AppendToBody("<a href=\"");
        if (qres[i].document_name.find("http") == string::npos) {
          ret.AppendToBody("/static/");
        }
        ret.AppendToBody(qres[i].document_name);
        ret.AppendToBody("\">");
        ret.AppendToBody(EscapeHtml(qres[i].document_name));
        ret.AppendToBody("</a>");
        ret.AppendToBody(" [");
        ret.AppendToBody(std::__cxx11::to_string(qres[i].rank));
        ret.AppendToBody("]<br>\n");
        ret.AppendToBody("</li>");
      }
      ret.AppendToBody("</ul>");
    }
  }
  ret.AppendToBody("</body>\r\n");
  ret.AppendToBody("</html>\r\n");
  return ret;
}

}  // namespace hw4
