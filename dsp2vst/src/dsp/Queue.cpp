#include "Queue.h"

void Queue::setSize(int size)
{
    m_queue.resize(size, 0.0f);
}

void Queue::pushSample(float sample)
{
    if(m_availableSamples >= m_queue.size())
    {
        return;
    }
    m_queue[writeIdx] = sample;

    writeIdx =(writeIdx + 1) % m_queue.size();
    m_availableSamples++;
}

void Queue::addSamples(const std::vector<float>& src, int numSamples) {
    // We use a temporary write index because we are mixing a full FFT frame, 
    // but we aren't ready to officially commit them all to m_availableSamples yet.
    int tempWriteIdx = writeIdx;

    for (int i = 0; i < numSamples; ++i) {
        m_queue[tempWriteIdx] += src[i]; // OVERLAP ADD!
        tempWriteIdx = (tempWriteIdx + 1) % m_queue.size();
    }
}

void Queue::advanceWrite(int hopSize) {
    // Once the FFT frame is added, we advance the actual write head by the hop size.
    // This officially marks these samples as "finished" and ready for the DAW to pop.
    writeIdx = (writeIdx + hopSize) % m_queue.size();
    m_availableSamples += hopSize;
}

void Queue::popSamples(std::vector<float>& dest, int numSamples) {
    if (numSamples > m_availableSamples) return;

    for (int i = 0; i < numSamples; ++i) {
        dest[i] = m_queue[readIdx];
        
        m_queue[readIdx] = 0.0f; 
        
        readIdx = (readIdx + 1) % m_queue.size();
    }
    m_availableSamples -= numSamples;
}


void Queue::advance(int numSamples)
{   
    numSamples =std::min(numSamples, m_availableSamples);
    readIdx = (readIdx + numSamples)% m_queue.size();
    m_availableSamples-=numSamples;
}

int Queue::getAvailableSamples() const
{   
    return m_availableSamples;
}

void Queue::reset()
{
    std::fill(m_queue.begin(), m_queue.end(), 0.0f);
    writeIdx = 0;
    readIdx = 0;
    m_availableSamples = 0;
}

void Queue::peekSamples(std::vector<float> &dest, int numSamples) const
{   
    dest.resize(numSamples);
    for(int i{0}; i<(numSamples); ++i){
        int idx = (readIdx + i) % m_queue.size();
        
        dest[i] = m_queue[idx]; 
    }

}

