#include <Rcpp.h>
#include <unistd.h>
#include <stdlib.h>
#include <vector>
#include <string>
using namespace Rcpp;

#ifdef __APPLE__
#include <crt_externs.h>
#define environ (*_NSGetEnviron())
#else
extern char **environ;
#endif
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
  char *line = (char*)malloc(name.size() + 1 + value.size() + 1);
  strcpy(line, (name + "=" + value).c_str());
  environ[index] = line;
}

// [[Rcpp::export]]
void unset(std::vector<bool> indices) {
  char **e = environ;
  for (size_t i = 0; i < indices.size(); i++) {
    if (indices[i]) {
      // it's a dupe
    } else {
      *e = environ[i];
      e++;
    }
  }
  *e = NULL;
}

std::string varname(const char* line) {
  size_t nameLen = strcspn(line, "=");
  if (nameLen == strlen(line)) {
    return std::string();
  } else {
    return std::string(line, nameLen);
  }
}

// [[Rcpp::export]]
void fix_yosemite_bug() {
  std::set<std::string> seen;
  char **read = environ, **write = environ;
  
  for (; *read; read++) {
    std::string name = varname(*read);
    if (seen.find(name) == seen.end()) {
      // Not a dupe
      *write = *read;
      write++;
    }
    seen.insert(name);
  }
  *write = NULL;
}

// [[Rcpp::export]]
void do_unset(const std::string& name) {
  unsetenv(name.c_str());
}

// This leaks memory. It's not safe to do realloc here as it's possible
// that environ is a static. Needless to say, don't use this or anything
// like it in production code.
// [[Rcpp::export]]
void append_env(const std::string& name, const std::string& value) {
  size_t count = 0;
  for (char** e = environ; *e; e++) {
    count++;
  }
  count++; // count the null terminator
  
  char** newEnv = (char**)malloc(sizeof(char*) * (count + 1));
  for (size_t i = 0; i < count; i++) {
    newEnv[i] = environ[i];
  }
  
  char *line = (char*)malloc(name.size() + 1 + value.size() + 1);
  strcpy(line, (name + "=" + value).c_str());
  newEnv[count - 1] = line;
  newEnv[count] = NULL;
  
  environ = newEnv;
}