#include <fstream>
#include <iostream>
#include <iomanip>
#include <unordered_map>
#include <vector>
#include <string>

#include <LIEF/PE.hpp>

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> parts;
    size_t start = 0, end;
    
    while ((end = str.find(delimiter, start)) != std::string::npos) {
        parts.emplace_back(str.substr(start, end - start));
        start = end + 1;
    }
    
    parts.emplace_back(str.substr(start)); // Add the last part
    return parts;
}

uint32_t get_value(const std::string& name, const LIEF::PE::Binary& patch, std::unordered_map<std::string, uint32_t>& symbols_map) {
    size_t pos;
    uint32_t value = 0;
    try {
        value = std::stoul(name, &pos, 16);
        if (pos != name.size()) {
            value = 0;
        }
    } catch (std::exception) {
    }

    if (!value) {
        auto symbol = patch.get_symbol(name);
        if (symbol) {
            value = (uint32_t)(patch.imagebase() + symbol->value());
        } else if (symbols_map.count(name)) {
            value = symbols_map[name];
        }
    }
    
    return value;
}

int main(int argc, char *argv[]) {
    if (argc != 6) {
        std::cerr << "Usage: " << argv[0] << " <original> <patch> <patched> <symbols> <patches>" << std::endl;
        return 1;
    }

    auto original = LIEF::PE::Parser::parse(argv[1]);
    auto patch = LIEF::PE::Parser::parse(argv[2]);
    std::ifstream symbols(argv[4]);
    std::ifstream patches(argv[5]);
    uint32_t base, start;
    symbols >> std::hex >> base >> start;

    auto widberg = patch->get_section(".widberg");
    if (!widberg) {
        std::cerr << "No .widberg section" << std::endl;
        return 1;
    }

    auto filler = patch->get_section(".filler");
    if (!filler) {
        std::cerr << "No .filler section" << std::endl;
        return 1;
    }

    if (filler->virtual_address() >= widberg->virtual_address()) {
        std::cerr << ".filler section is not before .widberg section" << std::endl;
        return 1;
    }
    
    for (auto export_ : patch->exported_functions()) {
        if (export_.address() < widberg->virtual_address() || export_.address() >= widberg->virtual_address() + widberg->size()) {
            std::cerr << export_.name() << " is not in .widberg section" << std::endl;
        }
    }

    if (patch->has_relocations()) {
        std::cerr << "patch has relocations" << std::endl;
        return 1;
    }

    auto new_widberg = original->add_section(*widberg);
    // LIEF doesn't update the size of the section when padding content to file alignment
    if (new_widberg->content().size() > new_widberg->size()) {
        new_widberg->size(new_widberg->content().size());
    }

    if (base != original->imagebase()) {
        std::cerr << "imagebase does not match" << std::endl;
        return 1;
    }

    if (start != original->imagebase() + new_widberg->virtual_address()) {
        std::cerr << ".widberg virtual address does not match" << std::endl;
        return 1;
    }

    std::unordered_map<std::string, uint32_t> symbols_map;
    for (std::string line; std::getline(symbols, line);) {
        auto parts = split(line, ';');
        if (line.empty() || parts.empty()) {
            continue;
        }

        if (parts.size() != 2) {
            std::cerr << "Invalid symbol: " << line << std::endl;
            return 1;
        }

        auto value = get_value(parts[1], *patch, symbols_map);
        if (!value) {
            std::cerr << "Invalid symbol value: " << line << std::endl;
            return 1;
        }
        symbols_map[parts[0]] = value;
    }

    std::vector<uint8_t> data;

    for (std::string line; std::getline(patches, line);) {
        auto parts = split(line, ';');
        if (line.empty() || parts.empty()) {
            continue;
        }

        if (parts.size() != 3) {
            std::cerr << "Invalid patch: " << line << std::endl;
            return 1;
        }

        auto type = parts[0];
        auto address = get_value(parts[1], *patch, symbols_map);
        if (!address) {
            std::cerr << "Invalid address: " << line << std::endl;
            return 1;
        }
        auto value = get_value(parts[2], *patch, symbols_map);
        if (!value) {
            std::cerr << "Invalid value: " << line << std::endl;
            return 1;
        }

        if (type == "NEAR_JUMP") {
            data.resize(5);
            data[0] = 0xE9;
            *(int32_t *)(data.data() + 1) = value - address - 5;
            original->patch_address(address, data, LIEF::Binary::VA_TYPES::VA);
        } else if (type == "SHORT_JUMP") {
            int32_t offset = value - address - 2;

            if (offset < -128 || offset > 127) {
                std::cerr << "Invalid SHORT_JUMP offset" << std::endl;
                return 1;
            }

            data.resize(2);
            data[0] = 0xEB;
            data[1] = (int8_t)(value - address - 2);
            original->patch_address(address, data, LIEF::Binary::VA_TYPES::VA);
        } else if (type == "BYTE") {
            if (value > 0xFF) {
                std::cerr << "Invalid BYTE value" << std::endl;
                return 1;
            }
            
            data.resize(1);
            data[0] = (uint8_t)value;
            original->patch_address(address, data, LIEF::Binary::VA_TYPES::VA);
        } else {
            std::cerr << "Unknown patch type: " << type << std::endl;
            return 1;
        }
    }

    auto config = LIEF::PE::Builder::config_t();
    config.relocations = false;
    original->write(argv[3], config);
}
