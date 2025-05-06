#include "string_line.h"
#include "string_sequence.h"
#include <format>
#include <memory>

void StringSequence::add(const Color& color, std::unique_ptr<std::vector<StringLine>>&& lines)
{
    sequence.emplace_back(color, std::move(lines));
}

const StringSequence::sequence_t& StringSequence::get_sequence() const
{
    return sequence;
}

std::string StringSequence::get_str() const
{
    std::string result;
    for (const auto& [color, lines] : sequence) {
        if (!lines->empty()) {
            result += std::format("C {:.0f} {:.0f} {:.0f}\n", 255 * color.r(), 255 * color.g(), 255 * color.b());
            result += std::format("L {} {}\n",
                                  (*lines)[0].get_start_nail_id(),
                                  (*lines)[0].get_start_wrap() == StringLine::Wrap::CLOKWISE ? "c" : "ac");
            for (const auto& line : *lines) {
                result += std::format("L {} {}\n",
                                      line.get_end_nail_id(),
                                      line.get_end_wrap() == StringLine::Wrap::CLOKWISE ? "c" : "ac");
            }
        }
    }
    return result;
}
