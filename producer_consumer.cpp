/*
Producer consumer pattern fun

Revise multithreading cpp with an example of the producer consumer paradigm.

We have multiple producers creating work and multiple consumers doing work on a
common buffer.

Hooray for cpp.
*/

#include <iostream>
#include <thread>
#include <deque>
#include <mutex>
#include <chrono>
#include <condition_variable>

std::mutex mu, cout_mu;
std::condition_variable cond;

class Buffer
{
public:
  // Adds work to the buffer, (work is just an int)
  void add(int num)
  {
    while (true)
    {
      std::unique_lock<std::mutex> locker(mu);
      cond.wait(locker, [this]()
                { return this->_buffer.size() < this->_size; }); // Wait till there is free space in the buffer to aquire the lock.
      this->_buffer.push_back(num);                              // Add the work
      locker.unlock();                                           // Remove the lock
      cond.notify_all();                                         // Notify everyone who is waiting to recheck.
    }
  }

  int take()
  {
    while (true)
    {
      std::unique_lock<std::mutex> locker(mu);
      cond.wait(locker, [this]()
                { return this->_buffer.size() > 0; }); // Wait around if the buffer is empty until it's not empty
      int back = this->_buffer.back();                 // Get the last value to return
      this->_buffer.pop_back();                        // Remove the last value from the buffer
      locker.unlock();
      cond.notify_all();
      return back;
    }
  }

  Buffer() {}

private:
  std::deque<int> _buffer;
  const unsigned int _size = 10; // Max size of our queue
};

int main()
{

  return 0;
};