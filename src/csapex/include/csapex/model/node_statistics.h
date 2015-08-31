#ifndef NODE_STATISTICS_H
#define NODE_STATISTICS_H

/// PROJECT
#include <csapex/model/model_fwd.h>
#include <csapex/factory/factory_fwd.h>

/// SYSTEM
#include <QTreeWidgetItem>

namespace csapex
{
class NodeStatistics
{
public:
    NodeStatistics(NodeWorker *node);
    QTreeWidgetItem* createDebugInformation(NodeFactory *node_factory) const;

private:
    QTreeWidgetItem * createDebugInformationConnector(Connectable *connector) const;

private:
    NodeWorker* node_worker_;
};
}

#endif // NODE_STATISTICS_H
