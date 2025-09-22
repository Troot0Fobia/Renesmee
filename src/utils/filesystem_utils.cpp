#include "filesystem_utils.hpp"
#include <stdexcept>

namespace utils {

const std::filesystem::path resolvePath(const std::string& path_str) {
    if (path_str.empty())
        throw std::invalid_argument("Specified path cannot be empty");

    std::error_code err_code;;
    std::filesystem::path cannonical_path
        = std::filesystem::canonical(std::filesystem::path{path_str}, err_code);
    
    if (err_code)
        throw std::filesystem::filesystem_error(
            "Provided path {} does not exist", err_code
        );
        
    return cannonical_path;
}

} // namespace utils
