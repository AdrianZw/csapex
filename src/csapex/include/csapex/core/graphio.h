#ifndef GRAPHIO_H
#define GRAPHIO_H

/// COMPONENT
#include <csapex/model/graph.h>
#include <csapex/csapex_fwd.h>

/// SYSTEM
#include <map>
#include <string>
#include <yaml-cpp/yaml.h>
#include <QList>
#include <QPoint>
#include <QSize>

/// FORWARD DECLARATION

namespace csapex
{

class GraphIO : public QObject
{
    Q_OBJECT

public:
    GraphIO(Graph::Ptr graph);

public:
    void saveSettings(YAML::Emitter &yaml);
    void loadSettings(YAML::Node& doc);

    void saveBoxes(YAML::Emitter &yaml);
    void loadBoxes(YAML::Parser &doc);

    void saveConnections(YAML::Emitter& yaml);
    void loadConnections(YAML::Node& doc);

private:
    Graph::Ptr graph_;
};

}

#endif // GRAPHIO_H
