#pragma once
#include <vector>
#include <stdexcept>

class CircularBuffer {
public:
    CircularBuffer() = default;
    
    void reset();
    void setSize(int newSize);
    int getSize() const;
    
    // Reads the oldest sample in the buffer (max delay)
    float read() const; 
    
    // Reads a specific amount of samples into the past
    float read(int delay) const; 
    
    // Writes a sample to the current index (does NOT move the index)
    void write(float sample); 
    
    // Moves the index forward and wraps around
    void advance(); 
    
    int getWriteIdx() const;

private:
    std::vector<float> buffer;
    int writeIdx{0};
    int size{0}; // Cached size for faster wrap-around checking
};
