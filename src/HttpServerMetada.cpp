#include "HttpServerMetada.h"

struct MappingEndpoint {
  RequestVerb httpRequestVerb;
  std::string path;

  MappingEndpoint(std::string path, RequestVerb verb) {
    this->httpRequestVerb = verb;
    this->path = path;
  }

  static MappingEndpoint defaultGet(std::string endpoint) {
    return {endpoint, RequestVerb::Get};
  }

  static MappingEndpoint PostMapping(std::string endpoint) {
    return {endpoint, RequestVerb::Post};
  }
};

std::map<std::string, ParserEndpoint> registered_endpoint;
