#ifndef NODE_WORKER_H
#define NODE_WORKER_H

/// PROJECT
#include <csapex/model/model_fwd.h>
#include <csapex/utility/utility_fwd.h>
#include <csapex/msg/msg_fwd.h>
#include <csapex/signal/signal_fwd.h>
#include <csapex/param/parameter.h>
#include <csapex/model/error_state.h>
#include <csapex/utility/uuid.h>

/// SYSTEM
#include <map>
#include <functional>
#include <condition_variable>
#include <mutex>
#include <vector>
#include <atomic>
#include <boost/signals2/signal.hpp>

namespace csapex {

class NodeWorker : public ErrorState
{
    friend class Node;
    friend class NodeBox;

public:
    enum ActivityType {
        TICK,
        PROCESS,
        OTHER
    };

public:
    typedef std::shared_ptr<NodeWorker> Ptr;

    enum class State {
        IDLE,
        ENABLED,
        FIRED,
        PROCESSING
    };

public:
    NodeWorker(NodeHandlePtr node_handle);
    ~NodeWorker();

    NodeHandlePtr getNodeHandle();
    NodePtr getNode() const;
    UUID getUUID() const;

    void stop();
    void reset();

    void triggerCheckTransitions();
    void triggerPanic();

    void setState(State state);
    State getState() const;

    bool isEnabled() const;
    bool isIdle() const;
    bool isProcessing() const;
    bool isFired() const;


    bool isProcessingEnabled() const;
    void setProcessingEnabled(bool e);

    void setProfiling(bool profiling);
    bool isProfiling() const;

    void setIOError(bool error);

    /* REMOVE => UI*/ void setMinimized(bool min);

    bool isWaitingForTrigger() const;
    bool canProcess() const;
    bool canReceive() const;
    bool canSend() const;
    bool areAllInputsAvailable() const;

    bool isSource() const;
    void setIsSource(bool source);

    bool isSink() const;
    void setIsSink(bool sink);

    int getLevel() const;
    void setLevel(int level);

    std::vector<TimerPtr> extractLatestTimers();

public:
    bool tick();

    void startProcessingMessages();
    void finishProcessingMessages(bool was_executed);

    void checkTransitions();

    void checkParameters();    
    void checkIO();

    void enableIO(bool enable);
    void enableInput(bool enable);
    void enableOutput(bool enable);
    void enableSlots(bool enable);
    void enableTriggers(bool enable);

    void triggerError(bool e, const std::string& what);

    void killExecution();

    void sendMessages();
    void notifyMessagesProcessed();

public:
    boost::signals2::signal<void()> panic;

    boost::signals2::signal<void()> ticked;
    boost::signals2::signal<void(bool)> enabled;

    boost::signals2::signal<void(NodeWorker* worker, int type, long stamp)> timerStarted;
    boost::signals2::signal<void(NodeWorker* worker, long stamp)> timerStopped;

    boost::signals2::signal<void(NodeWorker* worker)> startProfiling;
    boost::signals2::signal<void(NodeWorker* worker)> stopProfiling;

    boost::signals2::signal<void()> threadChanged;
    boost::signals2::signal<void(bool)> errorHappened;

    boost::signals2::signal<void()> messages_processed;
    boost::signals2::signal<void()> processRequested;
    boost::signals2::signal<void()> checkTransitionsRequested;

private:
    void publishParameters();
    void publishParameter(csapex::param::Parameter *p);
    void publishParameterOn(const csapex::param::Parameter &p, Output *out);

    void finishTimer(TimerPtr t);

    void updateTransitionConnections();

    void errorEvent(bool error, const std::string &msg, ErrorLevel level) override;


    void connectConnector(Connectable *c);

    void checkTransitionsImpl(bool try_fire);

private:
    mutable std::recursive_mutex sync;

    NodeHandlePtr node_handle_;
    NodeModifierPtr modifier_;

    bool is_setup_;
    State state_;

    Trigger* trigger_tick_done_;
    Trigger* trigger_process_done_;


    std::vector<boost::signals2::connection> connections;

    int ticks_;

    bool source_;
    bool sink_;
    int level_;

    mutable std::recursive_mutex state_mutex_;

    std::recursive_mutex timer_mutex_;
    std::vector<TimerPtr> timer_history_;

    std::atomic<bool> profiling_;
    TimerPtr current_process_timer_;
};

}

#endif // NODE_WORKER_H
