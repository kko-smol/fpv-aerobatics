#ifndef PROCESSTHREAD_H
#define PROCESSTHREAD_H

#include <boost/signals2.hpp>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>

typedef void DataContainer;
typedef std::shared_ptr<DataContainer> DataContainerPtr;

class Processor{
public:
    virtual DataContainerPtr process(DataContainerPtr) = 0;
};

class ProcessorCommand {
public:
    ProcessorCommand();
    ProcessorCommand(DataContainerPtr task);

    bool isCmd() const;
    bool isTask() const;

    virtual DataContainerPtr run(Processor*);
private:
    bool _isCmd;
    DataContainerPtr _task;
};

class ProcessorTaskCmd : public ProcessorCommand{

};


class ProcessThread
{
public:
    ProcessThread();

    ProcessThread(std::shared_ptr<Processor> proc);

    std::shared_ptr<Processor> processor(){
        return _proc;
    }

    typedef boost::signals2::signal<void (DataContainerPtr)> OutputFrame;

    void start();

    boost::signals2::connection connectOutput(const OutputFrame::slot_type& slot);

    void enqueue(DataContainerPtr input);
    void enqueueCmd(std::shared_ptr<ProcessorCommand> cmd);

    std::shared_ptr<Processor> _proc;

    bool _stopFlag;
    std::mutex _lock;
    std::condition_variable _condvar;
    std::queue<std::shared_ptr<ProcessorCommand>> _queue;

    std::thread _thread;
    OutputFrame sig_frame_;    

};

#endif // PROCESSTHREAD_H
