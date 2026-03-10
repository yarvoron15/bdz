#ifndef SEARCH_H_
#define SEARCH_H_


#include <string_view>
#include <vector>

std::vector<std::string_view> searchQryInText(std::string_view text,
                                              std::string_view query,
                                              size_t resultsCount);


#endif // SEARCH_H_
