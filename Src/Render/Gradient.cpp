#include "pch.h"
#include "Render/Gradient.hpp"

#include <vector>

Gradient::GradientColor::GradientColor(float _r, float _g, float _b, float _a) :r(_r / 255.0f), g(_g / 255.0f), b(_b / 255.0f), a(_a / 255.0f) {}
Gradient::GradientColor::GradientColor() : r(), g(0), b(0), a(0) {}

const Gradient::GradientColor & Gradient::GradientColor::operator+=(const GradientColor &lhs) {
    r += lhs.r;
    g += lhs.g;
    b += lhs.b;
    a += lhs.a;
    return *this;
}

const Gradient::GradientColor & Gradient::GradientColor::operator-=(const GradientColor &lhs) {
    r -= lhs.r;
    g -= lhs.g;
    b -= lhs.b;
    a -= lhs.a;
    return *this;
}

const Gradient::GradientColor & Gradient::GradientColor::operator*=(const float &lhs) {
    r *= lhs;
    g *= lhs;
    b *= lhs;
    a *= lhs;
    return *this;
}
const Gradient::GradientColor Gradient::GradientColor::operator+(const GradientColor& lhs) const {
    GradientColor res = *this;
    res += lhs;
    return res;
}

const Gradient::GradientColor Gradient::GradientColor::operator-(const GradientColor& lhs) const {
    GradientColor res = *this;
    res -= lhs;
    return res;
}

const Gradient::GradientColor Gradient::GradientColor::operator*(const float& lhs) const {
    GradientColor res = *this;
    res *= lhs;
    return res;
}