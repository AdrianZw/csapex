/// HEADER
#include <csapex/command/create_thread.h>

/// COMPONENT
#include <csapex/command/command.h>
#include <csapex/model/graph.h>
#include <csapex/model/graph_facade.h>
#include <csapex/model/node_handle.h>
#include <csapex/model/node_state.h>
#include <csapex/scheduling/thread_pool.h>
#include <csapex/scheduling/thread_group.h>
#include <csapex/model/node_runner.h>

/// SYSTEM
#include <sstream>

/// COMPONENT
#include <csapex/utility/assert.h>

using namespace csapex;
using namespace csapex::command;


CreateThread::CreateThread(const AUUID& parent_uuid, const UUID &node, const std::string& name)
    : Command(parent_uuid), uuid(node), name(name), old_id(-1), new_id(-1)
{
}

std::string CreateThread::getType() const
{
    return "CreateThread";
}

std::string CreateThread::getDescription() const
{
    std::stringstream ss;
    if(uuid.empty()) {
        ss << "created thread with name " << name;

    } else {
        ss << "created thread for " << uuid << " with name " << name;
    }
    return ss.str();
}

bool CreateThread::doExecute()
{
    if(uuid.empty()) {
        ThreadPool* thread_pool = getRootThreadPool();

        new_id = thread_pool->createGroup(name)->id();

    } else {
        TaskGenerator* tg = getGraphFacade()->getTaskGenerator(uuid);

        ThreadPool* thread_pool = getRootThreadPool();

        auto group = thread_pool->getGroupFor(tg);

        old_id = group->id();
        new_id = thread_pool->createNewGroupFor(tg, name);

    }

    return true;
}

bool CreateThread::doUndo()
{
    if(uuid.empty()) {
        getRootThreadPool()->removeGroup(new_id);

    } else {
        TaskGenerator* tg = getRoot()->getTaskGenerator(uuid);

        getRootThreadPool()->addToGroup(tg, old_id);
    }

    return true;
}

bool CreateThread::doRedo()
{
    if(uuid.empty()) {
        ThreadPool* thread_pool = getRootThreadPool();

        thread_pool->createGroup(name, new_id);
        return true;

    } else {
        return doExecute();
    }
}

