#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>
#include <istream>
#include <string>
#include <map>
#include <variant>
#include <cstring>


namespace omfl {

    struct Comparator {
        using hash_type = std::hash<std::string_view>;
        using is_transparent = void;

        std::size_t operator()(std::string_view str) const   { return hash_type{}(str); }
    };

class Section {

public:

    enum TypeOfSec {
        EMPTY,
        SECTION,
        ARRAY_VALUE,
        VALUE
    };

private:

    std::variant<int, std::string, bool, float, std::vector<Section>> variant;
    TypeOfSec _typeOfSec;
    bool validSection = true;
    std::string _name;
    std::unordered_map<std::string, Section*, Comparator, std::equal_to<>> _subSections;

public:

    const std::string& GetSectionName() const;

    Section GetSubSection(const Section& section);

    void AddValue(const std::string& name, Section* section);

    bool valid();

    void AddSubSection(const std::string& name);

    bool ContainsKey(const std::string& key);

    bool ContainsKey(const std::string_view& key);


    Section(TypeOfSec typeOfSec);

    Section(int value);

    Section(bool value);

    Section(float value);

    Section(const std::string& value);

    Section(const std::vector<Section>& value);

    Section() = default;

    bool IsInt();

    int AsInt();

    int AsIntOrDefault(int value);

    bool IsFloat();

    float AsFloat();

    float AsFloatOrDefault(float value);

    bool IsString();

    const std::string& AsString();

    std::string AsStringOrDefault(const std::string& value);

    bool IsArray();

    bool IsBool();

    bool AsBool();

    bool AsBoolOrDefault(bool value);

    const Section& operator[](uint64_t index) const;

    Section& Get(const std::string_view& name);

    Section* getSection(const std::string& name);

    Section* getSection(const std::string_view& name);

};
    Section* UpdateSection(Section* section, const std::string_view& name);

    std::vector<Section> ParseArray(const std::string& str);

    Section parse(const std::filesystem::path& path);

    Section parse(const std::string& str);

}// namespace

const omfl::Section EMPTY_SECTION = omfl::Section(omfl::Section::EMPTY);

