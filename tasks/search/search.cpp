#include "search.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace {

std::string ToLowerWord(std::string_view word) {
    std::string result;
    result.reserve(word.size());
    for (unsigned char ch : word) {
        result.push_back(static_cast<char>(std::tolower(ch)));
    }
    return result;
}

std::vector<std::string> SplitWords(std::string_view text) {
    std::vector<std::string> words;
    size_t i = 0;
    while (i < text.size()) {
        while (i < text.size() && std::isalpha(static_cast<unsigned char>(text[i])) == 0) {
            ++i;
        }
        const size_t start = i;
        while (i < text.size() && std::isalpha(static_cast<unsigned char>(text[i])) != 0) {
            ++i;
        }
        if (start != i) {
            words.push_back(ToLowerWord(text.substr(start, i - start)));
        }
    }
    return words;
}

struct LineData {
    std::string_view line;
    double score = 0;
    size_t index = 0;
};

}  // namespace

std::vector<std::string_view> searchQryInText(std::string_view text,
                                              std::string_view query,
                                              size_t resultsCount)
{
    const std::vector<std::string> query_words_vec = SplitWords(query);
    const std::unordered_set<std::string> query_words(query_words_vec.begin(), query_words_vec.end());
    if (query_words.empty() || resultsCount == 0) {
        return {};
    }

    std::vector<LineData> lines;
    std::unordered_map<std::string, size_t> document_frequency;

    size_t start = 0;
    size_t line_index = 0;
    while (start <= text.size()) {
        const size_t end = text.find('\n', start);
        const size_t line_end = end == std::string_view::npos ? text.size() : end;
        std::string_view line = text.substr(start, line_end - start);
        const std::vector<std::string> words = SplitWords(line);

        if (!words.empty()) {
            lines.push_back({line, 0.0, line_index});
            std::unordered_set<std::string> unique_words(words.begin(), words.end());
            for (const std::string& word : unique_words) {
                if (query_words.count(word) != 0) {
                    ++document_frequency[word];
                }
            }
        }

        ++line_index;
        if (end == std::string_view::npos) {
            break;
        }
        start = end + 1;
    }

    const double documents_count = static_cast<double>(lines.size());
    std::vector<LineData> relevant;
    relevant.reserve(lines.size());

    for (const LineData& line_data : lines) {
        const std::vector<std::string> words = SplitWords(line_data.line);
        std::unordered_map<std::string, size_t> freq;
        for (const std::string& word : words) {
            if (query_words.count(word) != 0) {
                ++freq[word];
            }
        }

        double score = 0.0;
        for (const auto& [word, count] : freq) {
            score += (static_cast<double>(count) / words.size()) *
                     std::log(documents_count / document_frequency[word]);
        }

        if (score > 0.0) {
            relevant.push_back({line_data.line, score, line_data.index});
        }
    }

    std::stable_sort(relevant.begin(), relevant.end(), [](const LineData& lhs, const LineData& rhs) {
        return lhs.score > rhs.score;
    });

    if (relevant.size() > resultsCount) {
        relevant.resize(resultsCount);
    }

    std::vector<std::string_view> result;
    result.reserve(relevant.size());
    for (const LineData& line : relevant) {
        result.push_back(line.line);
    }
    return result;
}
