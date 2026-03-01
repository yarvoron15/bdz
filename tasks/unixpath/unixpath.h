#ifndef UNIX_PATH_H_
#define UNIX_PATH_H_


#include <string>
#include <string_view>

std::string normalizePath(std::string_view currentWorkingDir,
                          std::string_view path);


#endif // UNIX_PATH_H_
