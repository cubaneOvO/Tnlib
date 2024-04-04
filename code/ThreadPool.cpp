#include "ThreadPool.h"
ThreadPool::ThreadPool(size_t threadNum, const std::string threadtype):stop_(false), threadType_(threadtype)
{
    for(size_t i = 0; i < threadNum; i++){
        threads_.emplace_back(
            [this]{//在这个地方相当于创建了一个线程，并且执行lambda函数中的内容
                //printf("create %s thread(%d)\n", threadType_.c_str(),syscall(SYS_gettid));//显示线程id
                while(stop_ == false){
                    std::function<void()> task;

                    {//锁作用域的开始
                        std::unique_lock<std::mutex> lock(this->mutex_);
                        //等待生产者的条件变量
                        this->condition_.wait(lock, 
                            [this]{//只有该lambda函数返回真时才能获取锁
                                return ((this->stop_ == true) || (this->taskqueue_.empty() == false));
                            }
                        );
                        if((this->stop_ == true) && (this->taskqueue_.empty() == true))//线程要退出并且无任务
                            return;
                        task = std::move(this->taskqueue_.front());
                        this->taskqueue_.pop();
                    }//锁作用域结束

                    //printf("%s thread (%d) execute task\n", threadType_.c_str(),syscall(SYS_gettid));
                    task();
                }
            }
        );
    }
}

ThreadPool::~ThreadPool(){
    stop();
}

//向任务队列中添加任务
void ThreadPool::addtask(std::function<void()> task){
    {
        std::unique_lock<std::mutex> lock(mutex_);
        taskqueue_.push(task);
    }
    condition_.notify_one();//唤醒一个正在等待的线程
}

size_t ThreadPool::size(){
    return threads_.size();
}

void ThreadPool::stop(){
    if(stop_) return;
    stop_ = true;
    condition_.notify_all();//唤醒全部线程
    //等待全部线程执行完任务后析构线程池
    for(std::thread &th:threads_)
        th.join();
}
