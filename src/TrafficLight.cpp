#include <iostream>
#include <random>
#include "TrafficLight.h"
#include <ctime>
/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 
    std::unique_lock<std::mutex> uLock(_mutex);
    _conditionVariable.wait(uLock, [this] { return !_vehicles.empty(); }); // pass unique lock to condition variable


    // remove last vector element from queue
    T msg = std::move(_messages.back());
    _messages.pop_back();

    return msg;     // will not be copied due to return value optimization (RVO) in C++

}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
	//Task-4
    std::lock_guard<std::mutex> u_lock(_mutex);
	queue_.push_back(std::move(msg));
	_conditionVariable.notify_one();

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
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

//Task-2
void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when
    // the public method „simulate“ is called. To do this, use the thread queue in the base class. 
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this)); 
}

// virtual function which is executed in a thread
//Task-2
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles (done)
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles.  (done)

    auto timeNow = std::chrono::system_clock::now();

    //generating random number from 4 to 6.  ref http://www.cplusplus.com/forum/general/63907/
    static std::mt19937 engine(std::time(nullptr));
	static std::uniform_int_distribution<> distribution(4000, 6000); //4000 & 6000 to generate it in miliseconds instead of seconds.
	auto randNumber =  distribution(engine) ;

    while(1){

        //auto newTime = timeNow - std::chrono::system_clock::now();     //removed for now, will do it that way
        int counter = 0;
        //check if the counter reaches the generated random value in miliseconds
        if(randNumber == counter){

            //toggling the current phase of traffic light
            if (_currentPhase == TrafficLightPhase::red){
                _currentPhase = TrafficLightPhase::green;
            } 
            else{
                _currentPhase = TrafficLightPhase::red;
            }

            _messageQueue.send(std::move(_currentPhase));         //comment to be removed once implementing send function
        }

        //timeNow = std::chrono::system_clock::now();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        counter++;
    }

}

