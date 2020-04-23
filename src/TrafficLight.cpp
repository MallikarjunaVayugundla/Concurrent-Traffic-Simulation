#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */


template <typename T>
TrafficLightPhase MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function.
    std::unique_lock<std::mutex> ulck(_mtx);
    //do stuff

    //wait
    _cvr.wait(ulck );


    //return received TrafficLightPhase
    TrafficLightPhase lastPhase=std::move(_queue.back());
    _queue.pop_back(); // need to remove?
    return lastPhase;

}

template <typename T>
void MessageQueue<T>::send(TrafficLightPhase &&trafficLightPhase)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
    std::lock_guard<std::mutex> lck(_mtx);
    _queue.push_back(trafficLightPhase);
    //send a notification
    _cvr.notify_one();
}


/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
     while(_trafficLightPhaseQueue.receive()==TrafficLightPhase::red) {
         std::this_thread::sleep_for(std::chrono::milliseconds(1));
     }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. To do this, use the thread queue in the base class.
    std::thread trafficLightThread(&TrafficLight::cycleThroughPhases,this);
    threads.push_back(std::move(trafficLightThread));

}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles.
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    while (true)
    {
        if(std::chrono::steady_clock::now() - start > std::chrono::seconds(6))
        {
            std::cout << "toggling current phase!" << std::endl;

            //toggles the current phase of the traffic light between red and green and sends an update method
         //to the message queue using move semantics
            _currentPhase = (_currentPhase == TrafficLightPhase::red) ? TrafficLightPhase::green : TrafficLightPhase::red;
            _trafficLightPhaseQueue.send(std::move(_currentPhase));
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            //reset start time;
            start = std::chrono::steady_clock::now();

        }

    }
}

TrafficLight::~TrafficLight() {

}

