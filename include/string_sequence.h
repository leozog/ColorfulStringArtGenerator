#pragma once
#include "color.h"
#include "string_line.h"

#include <memory>
#include <string>
#include <vector>

class StringSequence
{
public:
    using sequence_t = std::vector<std::pair<Color, std::unique_ptr<std::vector<StringLine>>>>;

private:
    sequence_t sequence;

public:
    void add(const Color& color, std::unique_ptr<std::vector<StringLine>>&& lines);
    [[nodiscard]] const sequence_t& get_sequence() const;
    [[nodiscard]] std::string get_str() const;
};