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
                { return this->buffer_.size() < this->size_; }); // Wait till there is free space in the buffer to aquire the lock.
      this->buffer_.push_back(num);                              // Add the work
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
                { return this->buffer_.size() > 0; }); // Wait around if the buffer is empty until it's not empty
      int back = this->buffer_.back();                 // Get the last value to return
      this->buffer_.pop_back();                        // Remove the last value from the buffer
      locker.unlock();
      cond.notify_all();
      return back;
    }
  }

  Buffer() {}

private:
  std::deque<int> buffer_;
  const unsigned int size_ = 10; // Max size of our queue
};

class Producer
{
public:
  Producer(Buffer *buffer, std::string name)
  {
    this->_buffer = buffer;
    this->_name = name;
  }

  void start()
  {
    while (true)
    {
      // Create a random work (int) to add in to the queue.
      int num = std::rand() % 100;
      this->_buffer->add(num);
      int sleep_time = rand() % 100; // Sleep time to sleep before producer creates more work

      cout_mu.lock(); // Aquire cout mutex so multiple producers are not writing to cout at the same time.
      std::cout << "Producer: " << this->_name << " added work: " << num << " Sleeping for: " << sleep_time << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
      cout_mu.unlock();
    }
  }

private:
  Buffer *_buffer;
  std::string _name;
};

class Consumer
{

public:
  Consumer(Buffer *buffer, std::string name)
  {
    this->_buffer = buffer;
    this->_name = name;
  }

  void start()
  {
    while (true)
    {
      // Takle work (int) off the buffer and just print.
      int num = this->_buffer->take();
      int sleep_time = std::rand() % 100;

      cout_mu.lock();
      std::cout << "Consumer: " << this->_name << " took work: " << num << " sleeping for: " << sleep_time << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
      cout_mu.unlock();
    }
  }

private:
  Buffer *_buffer;
  std::string _name;
};

int main()
{

  Buffer b;

  Producer p1(&b, "producer1");
  Producer p2(&b, "producer2");

  Consumer c1(&b, "consumer1");
  Consumer c2(&b, "consumer2");

  // Start producers and consumers on their own threads.
  std::thread producer1_thread(&Producer::start, &p1);
  std::thread producer2_thread(&Producer::start, &p2);

  std::thread consumer1_thread(&Consumer::start, &c1);
  std::thread consumer2_thread(&Consumer::start, &c2);

  producer1_thread.join();
  producer2_thread.join();

  consumer1_thread.join();
  consumer2_thread.join();

  std::getchar();
  return 0;
};