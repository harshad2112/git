#ifndef GIT_UTIL_H
#define GIT_UTIL_H

#include <vector>

#include "IndexEntry.hpp"

std::string cleanPath(std::string path);

std::vector<IndexEntry> readGitIndex(const std::string &indexPath);

#endif
