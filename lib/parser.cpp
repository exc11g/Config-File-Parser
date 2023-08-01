#pragma once

#include "parser.h"

namespace omfl {

    const std::string& Section::GetSectionName() const {
        return this->_name;
    }

    Section Section::GetSubSection(const Section& section) {
        return section._subSections.begin()->second;
    }

    void Section::AddValue(const std::string &name, Section* section) {
        if (this->ContainsKey(name)) {
            this->validSection = false;
        } else {
            this->_subSections.try_emplace(name, section);
        }
    }

    bool Section::valid() {
        return this->validSection;
    }

    void Section::AddSubSection(const std::string& name) {
        this->_subSections.try_emplace(name, new Section());
    }

    bool Section::ContainsKey(const std::string& key) {
        return this->_subSections.count(key) == 1;
    }

    bool Section::ContainsKey(const std::string_view& key) {
        return this->_subSections.count(key) == 1;
    }

    Section::Section(TypeOfSec typeOfSec) {
        _typeOfSec = typeOfSec;
    }

    Section::Section(int value) {
        variant = value;
    }

    Section::Section(float value) {
        variant = value;
    }

    Section::Section(bool value) {
        variant = value;
    }

    Section::Section(const std::string& value) {
        variant = value;
    }

    Section::Section(const std::vector<Section>& value) {
        variant = value;
    }

    bool Section::IsInt() {
        return std::holds_alternative<int>(this->variant);
    }

    int Section::AsInt() {
        return std::get<int>(this->variant);
    }

    int Section::AsIntOrDefault(int value) {
        if (this->IsInt()) {
            return this->AsInt();
        } else {
            return value;
        }
    }

    bool Section::IsFloat() {
        return std::holds_alternative<float>(this->variant);
    }

    float Section::AsFloat() {
        return std::get<float>(this->variant);
    }

    float Section::AsFloatOrDefault(float value) {
        if (this->IsFloat()) {
            return this->AsFloat();
        } else {
            return value;
        }
    }

    bool Section::IsString() {
        return std::holds_alternative<std::string>(this->variant);
    }

    const std::string& Section::AsString() {
        return std::get<std::string>(this->variant);
    }

    std::string Section::AsStringOrDefault(const std::string& value) {
        if (this->IsString()) {
            return this->AsString();
        } else {
            return value;
        }
    }

    bool Section::IsArray() {
        return std::holds_alternative<std::vector<Section>>(this->variant);
    }

    bool Section::IsBool() {
        return std::holds_alternative<bool>(this->variant);
    }

    bool Section::AsBool() {
        return std::get<bool>(this->variant);
    }

    bool Section::AsBoolOrDefault(bool value) {
        if (this->IsBool()) {
            return this->AsBool();
        } else {
            return value;
        }
    }

    const Section& Section::operator[](uint64_t index) const {
        if (std::get<std::vector<Section>>(this->variant).size() <= index) {
            return EMPTY_SECTION;
        }
        return std::get<std::vector<Section>>(this->variant)[index];
    }


    Section& Section::Get(const std::string_view& name) {
        if (uint64_t index = name.find('.'); index != std::string::npos) {
            return _subSections.find(name.substr(0, index - 1))->second->Get(name.substr(index + 1, name.length() - index));
        }
        return *(this->_subSections.find(name)->second);
    }

    Section* Section::getSection(const std::string& name) {
        return this->_subSections[name];
    }

    Section* Section::getSection(const std::string_view& name) {
        return this->_subSections.find(name)->second;
    }

    Section* UpdateSection(Section* section, const std::string_view& name) {
        uint64_t index = name.find('.');
        if (index != std::string::npos) {
            section = UpdateSection(section, name.substr(index + 1, name.length() - index));
        }
        return section->getSection(name);
    }

    std::vector<Section> ParseArray(const std::string& str) {
        std::vector<Section> res;
        uint64_t currIndex = 0;
        uint64_t tmpIndex = currIndex;
        while (currIndex < str.length()) {
            while (isspace(str[currIndex]) || str[currIndex] == ',' && currIndex < str.length()) {
                currIndex++;
            }
            if (str[currIndex] == '"') {
                tmpIndex = str.find('"', currIndex + 1);
                res.emplace_back(Section(str.substr(currIndex + 1, tmpIndex - (currIndex + 1))));
                currIndex = tmpIndex + 1;
            }
            if (str[currIndex] == '[') {
                uint64_t openStaples = 1;
                uint64_t closedStaples = 0;
                tmpIndex = currIndex;
                ++currIndex;
                while (openStaples != closedStaples && currIndex < str.length()) {
                    if (str[currIndex] == '"') {
                        if (uint64_t findIndex = str.find('"'); findIndex != std::string::npos && findIndex < str.length()) {
                            currIndex = str.find('"', currIndex + 1) + 1;
                        }
                    }
                    if (str[currIndex] == '[') {
                        openStaples++;
                    } else if (str[currIndex] == ']') {
                        closedStaples++;
                    }
                    currIndex++;
                }
                res.emplace_back(Section(ParseArray(str.substr(tmpIndex + 1, currIndex - tmpIndex - 2))));
                currIndex += 2;
            }
            if (str.substr(currIndex, 5) == "false") {
                res.emplace_back(Section(false));
                currIndex += 5;
            } else if (str.substr(currIndex, 4) == "true") {
                res.emplace_back(Section(true));
                currIndex += 4;
            }
            if (isdigit(str[currIndex]) || str[currIndex] == '+' || str[currIndex] == '-') {
                bool isFloat = false;
                tmpIndex = currIndex;
                while (isdigit(str[currIndex]) || str[currIndex] == '.') {
                    if (str[currIndex] == '.') {
                        isFloat = true;
                    }
                    currIndex++;
                }
                if (isFloat) {
                    res.emplace_back(Section(std::stof(str.substr(tmpIndex, currIndex - tmpIndex + 1))));
                } else {
                    res.emplace_back(Section(std::stoi(str.substr(tmpIndex, currIndex - tmpIndex + 1))));
                }
            }
        }

        return res;
    }

    Section parse(const std::filesystem::path& path) {
        std::ifstream file(path, std::ios_base::in);
        Section mainSection = Section(Section::SECTION);
        Section* currSection;
        std::string stringData;
        while (file >> stringData) {
            currSection = new Section(parse(stringData));
            mainSection.AddValue(currSection->GetSectionName(), currSection);
        }

        return mainSection;
    }

    Section parse(const std::string& str) {
        Section mainSection = Section(Section::SECTION);
        Section* currSection = &mainSection;
        uint64_t currIndex = 0;
        uint64_t tmpIndex = 0;
        std::string nameOfValue;
        std::string stringValue;
        std::string_view stringView = str;
        std::vector<Section> arrayValue;
        while (currIndex < str.length()) {
            while (isspace(str[currIndex])) {
                ++currIndex;
            }
            if (str[currIndex] == '#') {
                currIndex = str.find('\n') + 1;
            } else if (str[currIndex] == '[') {
                tmpIndex = str.find(']', currIndex);
                std::string nameOfNewSection = str.substr(currIndex + 1, tmpIndex - currIndex);
                currSection = UpdateSection(currSection, nameOfNewSection);
                currIndex = tmpIndex + 1;
            } else if (std::isalnum(str[currIndex])) {
                tmpIndex = currIndex;
                while (std::isalnum(str[currIndex])) {
                    currIndex++;
                }
                nameOfValue = str.substr(tmpIndex, currIndex - tmpIndex + 1);
                currIndex = str.find('=', currIndex) + 1;

                while (isspace(str[currIndex])) {
                    currIndex++;
                }
                if (str[currIndex] == '"') {
                    tmpIndex = str.find('"', currIndex + 1);
                    stringValue = str.substr(currIndex + 1, tmpIndex - (currIndex + 1));
                    currSection->AddValue(nameOfValue, new Section(stringValue));
                    currIndex = tmpIndex + 1;
                } else if (str[currIndex] == '[') {
                    uint64_t openStaples = 1;
                    uint64_t closedStaples = 0;
                    tmpIndex = currIndex;
                    ++currIndex;
                    while (openStaples != closedStaples && currIndex < str.length()) {
                        if (str[currIndex] == '"') {
                            if (uint64_t findIndex = str.find('"'); findIndex != std::string::npos && findIndex < str.length()) {
                                currIndex = str.find('"', currIndex + 1) + 1;
                            }
                        }
                        if (str[currIndex] == '[') {
                            openStaples++;
                        } else if (str[currIndex] == ']') {
                            closedStaples++;
                        }
                        currIndex++;
                    }
                    arrayValue = ParseArray(str.substr(tmpIndex + 1, currIndex - tmpIndex - 2));
                    currSection->AddValue(nameOfValue, new Section(arrayValue));
                    currIndex += 2;
                } else if (stringView.substr(currIndex, 5) == "false") {
                    currSection->AddValue(nameOfValue, new Section(false));
                    currIndex += 5;
                } else if (stringView.substr(currIndex, 4) == "true") {
                    currSection->AddValue(nameOfValue, new Section(true));
                    currIndex += 4;
                } else if (isdigit(str[currIndex]) || str[currIndex] == '+' || str[currIndex] == '-') {
                    bool isFloat = false;
                    tmpIndex = currIndex;
                    while (isdigit(str[currIndex]) || str[currIndex] == '.') {
                        if (str[currIndex] == '.') {
                            isFloat = true;
                        }
                        currIndex++;
                    }
                    if (isFloat) {
                        currSection->AddValue(nameOfValue, new Section(std::stof(str.substr(tmpIndex, currIndex - tmpIndex + 1))));
                    } else {
                        currSection->AddValue(nameOfValue, new Section(std::stoi(str.substr(tmpIndex, currIndex - tmpIndex + 1))));
                    }
                }
            }
        }
        return mainSection;
    }

}// namespace

