#include "./Common/Utilities/DoubleBuffer/DoubleBuffer.h"
#include "./Common/Utilities/ThreadWrapper/ThreadWrapper.h"
#include "TestBed.h"
#include <iostream>
#include <thread>


class Producer;
class Consumer;
class Consumer {
    DoubleBuffer<double>& m_buffer;

public:
    Consumer(DoubleBuffer<double>& buffer) : m_buffer(buffer) {}

    void run() {
        while (true) {
          //  std::shared_ptr<std::vector<double>> temp = m_buffer.getActiveBuffer();

            //if (temp->empty()) {
           //     break;
            //}

           // for (size_t i = 0; i < temp->size(); i++) {
           //     std::cout << temp->at(i) << std::endl;
           // }

            std::this_thread::sleep_for(std::chrono::microseconds(15));
        }
    }
};

class Producer {
    DoubleBuffer<double>& m_buffer;

public:
    Producer(DoubleBuffer<double>& buffer) : m_buffer(buffer) {}

    int index = 0;

    void run() {
        while (true) {
            //std::shared_ptr<std::vector<double>> temp = m_buffer.getInactiveBuffer();
            //temp->clear();

            //for (int i = index; i < (index + 15); i++) {
            //    temp->push_back(i);
            //}

           // index = index + 15;

           // m_buffer.swapBuffers();
           // std::this_thread::sleep_for(std::chrono::microseconds(15));
        }
    }
};

void runTestBed() {
    DoubleBuffer<double> buffer(15);
    ThreadWrapper producerThread;
    ThreadWrapper consumerThread;

    Producer prod(buffer);
    Consumer cons(buffer);

    producerThread.start(&Producer::run, &prod);
    consumerThread.start(&Consumer::run, &cons);

    std::this_thread::sleep_for(std::chrono::seconds(10));

    producerThread.stop();
    consumerThread.stop();
}



