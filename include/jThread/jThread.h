#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <assert.h> 

/*

      jThread a quick and dirty header only multi-threading library 

*/

// MIT License

// Copyright (c) 2024 Jerboa

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

/*

    stack overflows 
        https://stackoverflow.com/questions/26516683/reusing-thread-in-loop-c
        https://stackoverflow.com/questions/15752659/thread-pooling-in-c11
        https://stackoverflow.com/questions/3929774/how-much-overhead-is-there-when-creating-a-thread
        https://stackoverflow.com/questions/17348228/code-runs-6-times-slower-with-2-threads-than-with-1

    ThreadPool pool(n) - spawn n threads (beware hardware concurrency limits: https://en.cppreference.com/w/cpp/thread/thread/hardware_concurrency)

    pool.queueJob(std::bind(work,std::ref(a),std::ref(b),std::ref(c2))); - enqueue the function, work, with 3 arguments (as references)

    pool.wait() - waits until all jobs are done
    pool.stop() - stops (joins) threads (will interrupt threads if the have not already consumed a job on the queue)

*/
namespace jThread
{

  class ThreadPool 
  {

  public:

      ThreadPool(size_t n)
      : nThreads(n), terminate(false), working(0)
      {
        threads.resize(n);
        for (unsigned i = 0; i < n; i++)
        {
          threads[i] = std::thread(&ThreadPool::main,this);
        }
      }

      void queueJob(const std::function<void(void)> & job)
      {
        {
          std::unique_lock<std::mutex> lock(queueLock);
          jobs.emplace(std::move(job));
          // increment work to do/work inprogress counter
          working++;
        } // release mutex
        queueCondition.notify_one();
      }

      bool busy()
      {
        bool b = false;
        {
          std::unique_lock<std::mutex> lock(queueLock);
          // if work is still to do or still in progress pool is busy
          b = working > 0;
        } // release mutex
        return b;
      }

      void wait()
      {
        while (busy())
        {
          //void
        }
      }

      void stop()
      {
        {
          std::unique_lock<std::mutex> lock(queueLock);
          terminate = true;
        } // release mutex
        queueCondition.notify_all();
        for (std::thread & t : threads)
        {
          t.join();
        }
        threads.clear();
      }

      ~ThreadPool()
      {
          stop();
      }

      void joinThread()
      {
        size_t n = size();
        if (n > 0)
        {

          stop();
          terminate = false;
          threads.resize(n-1);

          for (unsigned i = 0; i < n-1; i++)
          {
            threads[i] = std::thread(&ThreadPool::main,this);
          }
        }
      }

      void joinAll()
      {
        while (size() > 0)
        {
          joinThread();
        }
      }

      void createThread()
      {
        size_t n = size();
        if (n < nThreads)
        {
          stop();
          terminate = false;
          threads.resize(n+1);
          for (unsigned i = 0; i < n+1; i++)
          {
            threads[i] = std::thread(&ThreadPool::main,this);
          }
        }
      }

      size_t size(){return threads.size();}

  private:

    void main()
    {
      while (true)
      {
        std::function<void(void)> job;
        {
          std::unique_lock<std::mutex> lock(queueLock);

          queueCondition.wait(
            lock, [this] {return !jobs.empty() || terminate;}
          );

          if (terminate)
          {
            return;
          }

          job = std::move(jobs.front());
          jobs.pop();
        } // release mutex

        job();

        {
          std::unique_lock<std::mutex> lock(queueLock);
          // decrement work being done/to do
          working--;
        }
      }
    }

    const size_t nThreads;

    bool terminate;

    std::queue<std::function<void(void)>> jobs;
    std::vector<std::thread> threads;

    size_t working;

    std::mutex queueLock;
    std::condition_variable queueCondition;
    
  };
}
#endif /* THREADPOOL_H */
