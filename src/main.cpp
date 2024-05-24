#include <iostream>
#include <string>
#include "spdlog/spdlog.h"
#include <nlohmann/json.hpp>
int main()
{
    spdlog::info("Welcome to spdlog!\n\n");
    nlohmann::json j = "{ \"happy\": true, \"pi\": 3.141 }"_json;
    std::string s = j.dump();
    std::cout << j.dump(4) << std::endl;
    
    std::cin.get();
    return 0;
}