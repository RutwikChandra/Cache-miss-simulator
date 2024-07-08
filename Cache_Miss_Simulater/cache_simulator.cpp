#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cmath>
#include <bitset>
#include <sstream>
#include <vector>
#include <random>

// Define a structure to hold configuration parameters for the cache
struct config {
    int cache_size;
    int block_size;
    int associativity;
    std::string rep_policy;
    std::string write_policy;
};

// Function to read configuration from a file and return a config structure
struct config read_config_file(std::string file_name) {
    struct config config;
    std::ifstream config_file(file_name);
    std::string line;

    int i = 0;
    // Read each line from the configuration file and assign values to the config structure
    while (std::getline(config_file, line)) {
        if (i == 0) {
            config.cache_size = std::stoi(line);
        } else if (i == 1) {
            config.block_size = std::stoi(line);
        } else if (i == 2) {
            config.associativity = std::stoi(line);
        } else if (i == 3) {
            config.rep_policy = line;
        } else if (i == 4) {
            config.write_policy = line;
        }
        i++;
    }
    return config;
}

// Convert binary string to hexadecimal string
std::string bin_to_hex(std::string bin_string) {
    std::bitset<32> bits(bin_string);
    unsigned long decimal_value = bits.to_ulong();
    std::stringstream hex_stream;
    hex_stream << std::hex << decimal_value;

    return "0x" + hex_stream.str();
}

// Convert binary string to decimal integer
int bin_to_decimal(std::string bin_string) {
    std::bitset<32> bits(bin_string);
    unsigned long decimal_value = bits.to_ulong();

    return decimal_value;
}

// Convert hexadecimal string to binary string
std::string hex_to_bin(std::string hex_string) {
    std::string binary_string;
    char hex_char;
    // Iterate through each character in the hexadecimal string
    for (int i = 2; i < hex_string.length(); i++) {
        hex_char = hex_string[i];
        // Convert each hexadecimal character to 4-bit binary representation
        switch (hex_char) {
            case '0': binary_string += "0000"; break;
            case '1': binary_string += "0001"; break;
            case '2': binary_string += "0010"; break;
            case '3': binary_string += "0011"; break;
            case '4': binary_string += "0100"; break;
            case '5': binary_string += "0101"; break;
            case '6': binary_string += "0110"; break;
            case '7': binary_string += "0111"; break;
            case '8': binary_string += "1000"; break;
            case '9': binary_string += "1001"; break;
            case 'A': case 'a': binary_string += "1010"; break;
            case 'B': case 'b': binary_string += "1011"; break;
            case 'C': case 'c': binary_string += "1100"; break;
            case 'D': case 'd': binary_string += "1101"; break;
            case 'E': case 'e': binary_string += "1110"; break;
            case 'F': case 'f': binary_string += "1111"; break;
            default:
                std::cerr << "Invalid hexadecimal character: " << hex_char << std::endl;
                return "";
        }
    }
    // Add zeros at the end to make the binary string 32 bits long
    int end_zeros = 32 - binary_string.length();
    for (int i = 1; i <= end_zeros; i++) {
        binary_string = "0" + binary_string;
    }
    return binary_string;
}

int main() {
    // Read configuration from the "cache.config" file
    struct config config = read_config_file("cache.config");

    // Calculate cache parameters based on the configuration
    int sets = (config.cache_size) / ((config.associativity) * (config.block_size));
    int index_bits = log2(sets);
    int offset_bits = log2(config.block_size);
    int tag_bits = 32 - index_bits - offset_bits;

    // Initialize total cache hits and cache misses
    int total_hits = 0;
    int total_misses = 0;

    std::string mode, hex_address, bin_address;
    std::string bin_index, hex_index, bin_offset, hex_tag, bin_tag;
    int index;

    // Create a 2D vector to represent the cache sets and blocks
    std::vector<std::vector<std::string>> set(sets, std::vector<std::string>(config.associativity, "-1"));

    // Read 32bit addresses from the "cache.access" file
    std::ifstream input_file("cache.access");

    std::string line;
    // Process each line in the input file
    while (std::getline(input_file, line)) {
        // Extract mode and address from the input line
        mode = line.substr(0, 1);
        hex_address = line.substr(3, line.length() - 3);
        bin_address = hex_to_bin(hex_address);

        // Extract tag, index, and offset
        bin_tag = bin_address.substr(0, tag_bits);
        hex_tag = bin_to_hex(bin_tag);
        bin_index = bin_address.substr(tag_bits, index_bits);
        hex_index = bin_to_hex(bin_index);
        index = bin_to_decimal(bin_index);
        bin_offset = bin_address.substr(tag_bits + index_bits, offset_bits);

        // Cache replacement policies: FIFO, LRU, RANDOM
        if (config.rep_policy == "FIFO") {
            // Implement FIFO replacement policy
            for (int i = 0; i < config.associativity; i++) {
                if (set[index][i] == hex_tag) {
                    std::cout << "Address: " << hex_address << "\tSet: " << hex_index << "\tHit\tTag: " << hex_tag << std::endl;
                    total_hits++;
                    break;
                } else if (i == config.associativity - 1) {
                    if (mode == "W" && config.write_policy == "WT") {
                        std::cout << "Address: " << hex_address << "\tSet: " << hex_index << "\tMiss\tTag: " << hex_tag << std::endl;
                    } else {
                        set[index].erase(set[index].begin() + 0);
                        set[index].push_back(hex_tag);
                        std::cout << "Address: " << hex_address << "\tSet: " << hex_index << "\tMiss\tTag: " << hex_tag << std::endl;
                    }
                    total_misses++;
                }
            }
        } else if (config.rep_policy == "LRU") {
            // Implement LRU replacement policy
            for (int i = 0; i < config.associativity; i++) {
                if (set[index][i] == hex_tag) {
                    set[index].erase(set[index].begin() + i);
                    set[index].push_back(hex_tag);
                    std::cout << "Address: " << hex_address << "\tSet: " << hex_index << "\tHit\tTag: " << hex_tag << std::endl;
                    total_hits++;
                    break;
                } else if (i == config.associativity - 1) {
                    if (mode == "W" && config.write_policy == "WT") {
                        std::cout << "Address: " << hex_address << "\tSet: " << hex_index << "\tMiss\tTag: " << hex_tag << std::endl;
                    } else {
                        set[index].erase(set[index].begin() + 0);
                        set[index].push_back(hex_tag);
                        std::cout << "Address: " << hex_address << "\tSet: " << hex_index << "\tMiss\tTag: " << hex_tag << std::endl;
                    }
                    total_misses++;
                }
            }
        } else if (config.rep_policy == "RANDOM") {
            // Implement RANDOM replacement policy
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> distribution(0, config.associativity - 1);
            int random_number = distribution(gen);
            for (int i = 0; i < config.associativity; i++) {
                if (set[index][i] == hex_tag) {
                    std::cout << "Address: " << hex_address << "\tSet: " << hex_index << "\tHit\tTag: " << hex_tag << std::endl;
                    total_hits++;
                    break;
                } else if (i == config.associativity - 1) {
                    if (mode == "W" && config.write_policy == "WT") {
                        std::cout << "Address: " << hex_address << "\tSet: " << hex_index << "\tMiss\tTag: " << hex_tag << std::endl;
                    } else {
                        set[index][random_number] = hex_tag;
                        std::cout << "Address: " << hex_address << "\tSet: " << hex_index << "\tMiss\tTag: " << hex_tag << std::endl;
                    }
                    total_misses++;
                }
            }
        }
    }
    std::cout<<"Total Hits : "<<total_hits<<"\tTotal Misses : "<<total_misses<<"\t"<<std::endl;
}