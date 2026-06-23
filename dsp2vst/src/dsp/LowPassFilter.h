#pragma once

class LowPassFilter {
public:
    LowPassFilter() = default;

    // Resets the filter's memory
    void reset();

    // Sets the coefficient directly (0.0 to 1.0)
    void setCoefficient(float c);

    // Processes a single sample
    float process(float input);

private:
    float c{1.0f};  // Cutoff coefficient
    float z1{0.0f}; // One-sample memory (z^-1)
};