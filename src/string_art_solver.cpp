#include "string_art_solver.h"

StringArtSolver::StringArtSolver(Img &&input_img, std::vector<Color> &&palette, std::string &&output_name, int n_threads)
    : input_img{std::move(input_img)}, palette{std::move(palette)}, output_name{std::move(output_name)}, pool{n_threads}
{
}

void StringArtSolver::solve()
{
}