#include <vector>

class Queue{
    public:
    Queue()=default;

    void setSize(int size);
    void pushSample(float sample);
    void addSamples(const std::vector<float>& src, int numSamples);
    void advance(int numSamples);
    void advanceWrite(int hopSize);
    void popSamples(std::vector<float>& dest, int numSamples);

    int getAvailableSamples() const;
    void reset();
    void peekSamples(std::vector<float>& dest, int numSamples) const;

    private:
    std::vector<float> m_queue;
    int readIdx;
    int writeIdx;
    int m_availableSamples;


};