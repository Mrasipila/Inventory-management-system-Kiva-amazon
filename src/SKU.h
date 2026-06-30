#pragma once

#include <string>

struct SKU {
    int id{0};
    std::string name;
    float weight{0.0f};
    float volume{0.0f};
    int velocityClass{2};
    float demandWeight{1.0f};
};
