#include "CircularBuffer.h"

void CircularBuffer::reset() {
    std::fill(buffer.begin(), buffer.end(), 0.0f);
    writeIdx = 0;
}

void CircularBuffer::setSize(int newSize) {
    if (newSize <= 0) {
        throw std::invalid_argument("Buffer size must be greater than 0");
    }
    
    size = newSize;
    buffer.resize(size);
    
    // reset() handles clearing the memory to 0.0f and resetting the index
    reset(); 
}

int CircularBuffer::getSize() const {
    return size;
}

int CircularBuffer::getWriteIdx() const {
    return writeIdx;
}

void CircularBuffer::write(float sample) {
    // Just overwrite the current index. Advance is handled separately.
    buffer[writeIdx] = sample;
}

void CircularBuffer::advance() {
    writeIdx++;
    
    // Use an 'if' branch instead of '%' for massive DSP performance gains
    if (writeIdx >= size) {
        writeIdx = 0;
    }
}

float CircularBuffer::read() const {
    // Reading without a delay parameter automatically grabs the oldest sample
    return buffer[writeIdx];
}

float CircularBuffer::read(int delay) const {
    // Allow delay to be 0 (reading the current writeIdx right before writing)
    if (delay < 0 || delay > size) {
        throw std::invalid_argument("Invalid delay: must be between 0 and buffer size");
    }

    int idxToRead = writeIdx - delay;
    
    // Wrap around backwards if we drop below 0
    if (idxToRead < 0) {
        idxToRead += size; 
    }
    
    return buffer[idxToRead];
}