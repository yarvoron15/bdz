#include "unixpath.h"

#include <string>
#include <vector>

namespace {

void ProcessPath(std::string_view path, std::vector<std::string>& parts) {
    size_t start = 0;
    while (start <= path.size()) {
        const size_t slash = path.find('/', start);
        const size_t end = slash == std::string_view::npos ? path.size() : slash;
        const std::string_view token = path.substr(start, end - start);

        if (!token.empty() && token != ".") {
            if (token == "..") {
                if (!parts.empty()) {
                    parts.pop_back();
                }
            } else {
                parts.emplace_back(token);
            }
        }

        if (slash == std::string_view::npos) {
            break;
        }
        start = slash + 1;
    }
}

}  // namespace

std::string normalizePath(std::string_view currentWorkingDir,
                          std::string_view path)
{
    std::vector<std::string> parts;
    if (path.empty() || path.front() != '/') {
        ProcessPath(currentWorkingDir, parts);
    }
    ProcessPath(path, parts);

    if (parts.empty()) {
        return "/";
    }

    std::string result;
    for (const std::string& part : parts) {
        result += '/';
        result += part;
    }
    return result;
}
