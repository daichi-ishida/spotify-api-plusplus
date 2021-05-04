// MIT License

// Copyright (c) 2021 Daichi Ishida

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

#include <chrono>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>


struct Timer 
{
private:
    std::mutex _mtx;
    std::condition_variable _cv;
    std::thread _thread;
    bool _is_interrupted = false;

public:
    Timer(){}
    ~Timer(){ interrupt(); }

    void start(size_t time, const std::function<void(void)> &f)
    {
        _thread = std::thread(&Timer::wait_then_call, this, time, f);
    }

    void interrupt()
    {
        {
            std::lock_guard<std::mutex> lck(_mtx);
            _is_interrupted = true;
        }

        _cv.notify_one();
        _thread.join();
    }

    void wait_then_call(size_t time, const std::function<void(void)> &f)
    {
        std::chrono::seconds sec = std::chrono::seconds(time);
        std::unique_lock<std::mutex> lck(_mtx);

        while (!_cv.wait_for(lck, sec, [this] { return _is_interrupted; })) 
        {
            // after {sec} seconds && _is_interrupted == false
            f();
        }
        // execute here when it is interrupted
    }
};