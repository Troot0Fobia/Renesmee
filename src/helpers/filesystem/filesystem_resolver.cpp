#include "filesystem_resolver.hpp"
#include <format>
#include <stdexcept>
#include <string>

namespace helpers {

const std::filesystem::path resolvePath(const std::string& path_str) {
    if (path_str.empty())
        throw std::invalid_argument("Specified path cannot be empty");

    std::error_code err_code;
    std::filesystem::path path(path_str);

    if (!std::filesystem::exists(path, err_code))
        throw std::filesystem::filesystem_error(
            std::format("File with path {} does not exist", path_str),
            err_code
        );

    std::filesystem::path cannonical_path = std::filesystem::canonical(path, err_code);
    
    if (err_code)
        throw std::filesystem::filesystem_error(
            "Provided path {} does not exist", err_code
        );
        
    return cannonical_path;
}

} // helpers