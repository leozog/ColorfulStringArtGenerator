#pragma once
#include "array2d.h"
#include "img.h"

class StringColorSolver
{
private:
    Array2d<float> img;
    Color color;

public:
    StringColorSolver(const Img &full_img, Color color);
    ~StringColorSolver();
};

StringColorSolver::StringColorSolver(const Img &full_img, Color color) : img(full_img->get_w(), full_img->get_h()), color{color}
{
}

StringColorSolver::~StringColorSolver()
{
}
