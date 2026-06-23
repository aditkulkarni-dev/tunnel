#include "LowPassFilter.h"

void LowPassFilter::reset() {
    z1 = 0.0f;
}

void LowPassFilter::setCoefficient(float newC) {
    // Keep it safely within bounds
    if (newC < 0.0f) newC = 0.0f;
    if (newC > 1.0f) newC = 1.0f;
    c = newC;
}

float LowPassFilter::process(float input) {
    // y[n] = c * x[n] + (1 - c) * y[n-1]
    z1 = (c * input) + ((1.0f - c) * z1);
    return z1;
}