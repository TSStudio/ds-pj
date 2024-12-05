#include "utils.h"
#include <boost/beast/core/detail/base64.hpp>

auto Base64::decode(const std::string& str) -> std::string {
    std::string dest;
    dest.resize(boost::beast::detail::base64::decoded_size(str.size()));
    auto const result = boost::beast::detail::base64::decode(
        &dest[0], str.data(), str.size());
    dest.resize(result.first);
    return dest;
}