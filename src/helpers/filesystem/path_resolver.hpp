#pragma once

#include <filesystem>
#include <string>
#ifndef PATH_RESOLVER_H
#define PATH_RESOLVER_H

namespace helpers {

const std::filesystem::path resolvePath(const std::string& path);

} // helpers

#endif // PATH_RESOLVER_H