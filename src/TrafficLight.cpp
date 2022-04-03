#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
	std::unique_lock<std::mutex> lck(_mutex);
	_condiMQ.wait(lck, [this]{return !_queue.empty();});
    // to wait for and receive new messages and pull them from the queue using move semantics. 
	T output = std::move(_queue[0]);
	_queue.pop_front();
    // The received object should then be returned by the receive function. 
	return output;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms 
	
	std::lock_guard<std::mutex> lck(_mutex);
	_queue.emplace_back(std::move(msg));
	
	_condiMQ.notify_one();
	
    // FP.4a : as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
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
	while(1){
		_currentPhase = _message.receive();
		if(_currentPhase == TrafficLightPhase::green) break;
	}
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. To do this, use the thread queue in the base class. 
	threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles. 
	
	std::chrono::time_point<std::chrono::system_clock> t_start, t_end;
	t_start = std::chrono::system_clock::now();
	long long_duration;
	long long_randomTp = RandomTimeCalc();
	std::cout<<"long_randomTp: "<<long_randomTp<<std::endl;
	while(1){
		
		std::this_thread::sleep_for(std::chrono::milliseconds(1));	
		
		// time calculate 
		t_end = std::chrono::system_clock::now();
		long_duration = (t_end - t_start).count()*1000;
		std::cout<<"long_duration: "<<long_duration<<std::endl;
		
		// if the time reaches the random time point, between 4ms - 6ms
		if (long_duration >= long_randomTp){
			// random Time, start Time re-calc
			t_start = std::chrono::system_clock::now();
			
			_currentPhase = (_currentPhase == TrafficLightPhase::red)?TrafficLightPhase::green:TrafficLightPhase::red;
			
			// sends an update method to the message queue using move semantics
			_message.send(std::move(_currentPhase));
		}
	}
}

long TrafficLight::RandomTimeCalc(){
	// duration, ms
	int random_max = 6000;
	int random_min = 4000;
	
	std::random_device rd;
	std::mt19937 eng(rd());
	std::uniform_int_distribution<> distr(random_min,random_max);
	return distr(eng);	
}

