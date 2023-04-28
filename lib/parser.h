#pragma once

#include <filesystem>
#include <istream>


namespace omfl {

class __NameMe__;
__NameMe__ parse(const std::filesystem::path& path);
__NameMe__ parse(const std::string& str);


}// namespace