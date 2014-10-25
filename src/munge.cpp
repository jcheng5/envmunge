#include <Rcpp.h>
#include <unistd.h>
#include <stdlib.h>
#include <vector>
#include <string>
using namespace Rcpp;

extern char **environ;

// [[Rcpp::export]]
std::vector<std::string> ls_env() {
  char **e = environ;
  std::vector<std::string> result;
  while (*e) {
    result.push_back(*e);
    e++;
  }
  return result;
}

// [[Rcpp::export]]
void replace(int index, std::string name, std::string value) {
  index--; // convert to 0-based
  char *line = (char*)malloc(name.size() + 1 + value.size());
  strcpy(line, (name + "=" + value).c_str());
  environ[index] = line;
}