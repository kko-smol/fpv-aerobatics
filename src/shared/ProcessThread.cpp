#include "ProcessThread.h"

ProcessThread::ProcessThread(){

}

ProcessThread::ProcessThread(std::shared_ptr<Processor> proc):_proc(proc){

}

void ProcessThread::start() {
    _thread = std::thread([=](){
        _stopFlag = false;
        std::shared_ptr<ProcessorCommand> in;
        while(!_stopFlag){
            {
                {
                    std::unique_lock<std::mutex> lock(_lock);
                    while (_queue.size()==0) {  // loop to avoid spurious wakeups
                        _condvar.wait(lock);
                    }
                    in = _queue.front();
                    _queue.pop();
                }

                DataContainerPtr out = in->run(processor().get());

                if (out.get()!=nullptr){
                    sig_frame_(out);
                }

            }
        }
    });
}

boost::signals2::connection ProcessThread::connectOutput(const OutputFrame::slot_type &slot){
    return sig_frame_.connect(slot);
}

void ProcessThread::enqueue(DataContainerPtr input){
    {
        std::unique_lock<std::mutex> lock(_lock);
        _queue.push(std::make_shared<ProcessorCommand>(input));
    }
    _condvar.notify_one();
}

void ProcessThread::enqueueCmd(std::shared_ptr<ProcessorCommand> cmd)
{
    {
        std::unique_lock<std::mutex> lock(_lock);
        _queue.push(cmd);
    }
    _condvar.notify_one();
}

ProcessorCommand::ProcessorCommand():_isCmd(true)
{

}

ProcessorCommand::ProcessorCommand(DataContainerPtr task):_isCmd(false),_task(task)
{

}

bool ProcessorCommand::isCmd() const
{
    return _isCmd;
}

bool ProcessorCommand::isTask() const
{
    return !isCmd();
}

DataContainerPtr ProcessorCommand::run(Processor * p)
{
    return p->process(_task);
}
