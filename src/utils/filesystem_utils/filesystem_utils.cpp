#include "filesystem_utils.hpp"
#include <stdexcept>
#include <format>

namespace utils {

std::filesystem::path resolvePath(const std::string& path_str) {
    if (path_str.empty())
        throw std::invalid_argument("Specified path cannot be empty");

    std::error_code err_code;;
    std::filesystem::path cannonical_path
        = std::filesystem::canonical(std::filesystem::path{path_str}, err_code);
    
    if (err_code)
        throw std::filesystem::filesystem_error(std::format(
            "Provided path {} does not exist", path_str), err_code
        );
        
    return cannonical_path;
}

std::filesystem::path createFolder(const std::string& path_str) {
    if (path_str.empty())
        throw std::runtime_error("Output path cannot be empty");

    std::error_code err_code;
    std::filesystem::path path = std::filesystem::weakly_canonical(
        std::filesystem::path(path_str),
        err_code
    );

    if (err_code)
        throw std::filesystem::filesystem_error(
            std::format("Error occured with path {}: {}", path_str, err_code.message()),
            err_code
        );

    if (std::filesystem::exists(path))
        return path;

    if (!std::filesystem::create_directories(path, err_code))
        throw std::filesystem::filesystem_error(
            std::format(
                "Failed create directories with path {}: {}",
                path.string(),
                err_code.message()
            ),
            err_code
        );

    return path;
}

} // namespace utils
