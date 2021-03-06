#ifndef DISPATCHER_PROXY_H
#define DISPATCHER_PROXY_H

/// COMPONENT
#include <csapex/command/command.h>
#include <csapex/command/command_fwd.h>
#include <csapex/command/command_executor.h>
#include <csapex/io/io_fwd.h>
#include <csapex/io/proxy.h>

/// SYSTEM
#include <deque>
#include <csapex/utility/slim_signal.h>

namespace csapex
{
class CSAPEX_COMMAND_EXPORT CommandDispatcherProxy : public CommandExecutor, public Proxy
{
public:
    typedef std::shared_ptr<CommandDispatcherProxy> Ptr;

public:
    CommandDispatcherProxy(const SessionPtr& session);

    bool execute(const CommandPtr& command);
    void executeLater(const CommandPtr& command);
    void executeLater();

    bool isDirty() const;

    bool canUndo() const;
    bool canRedo() const;

    void undo();
    void redo();

private:
    void handleBroadcast(const BroadcastMessageConstPtr& message) override;

private:
    mutable bool init_;
    mutable bool dirty_;
};

}  // namespace csapex

#endif  // DISPATCHER_PROXY_H
