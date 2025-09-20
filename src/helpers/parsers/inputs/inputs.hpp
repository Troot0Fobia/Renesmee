#pragma once

#include <limits>
#include <string>
#include <utility>
#ifndef INPUT_PARSER_H
#define INPUT_PARSER_H

namespace helpers {

const std::pair<std::string, unsigned short> parseAddr(const std::string& addr);

inline bool isUShort(int v) {
    return std::numeric_limits<unsigned short>::max() >= v && std::numeric_limits<unsigned short>::min() <= v;
}

} // helpers

#endif // INPUT_PARSER_H