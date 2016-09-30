#ifndef DESIGNER_H
#define DESIGNER_H

/// COMPONENT
#include <csapex/view/csapex_qt_export.h>
#include <csapex/core/core_fwd.h>
#include <csapex/command/command_fwd.h>
#include <csapex/view/view_fwd.h>
#include <csapex/model/model_fwd.h>
#include <csapex/utility/uuid.h>
#include <csapex/utility/slim_signal.h>
#include <csapex/view/csapex_view_core.h>
#include <csapex/profiling/profilable.h>

/// SYSTEM
#include <QWidget>
#include <yaml-cpp/yaml.h>
#include <unordered_map>

/// FORWARD DECLARATIONS
namespace Ui
{
class Designer;
}

namespace csapex
{

class NodeFactory;

class CSAPEX_QT_EXPORT Designer : public QWidget, public Profilable
{
    Q_OBJECT

    friend class DesignerIO;
    friend class DesignerOptions;

public:
    Designer(CsApexViewCore& view_core, QWidget* parent = 0);
    virtual ~Designer();

    DesignerOptions *options();
    MinimapWidget* getMinimap();

    void setup();

    void setView(int x, int y);

    void addGraph(GraphFacadePtr graph);
    void removeGraph(GraphFacade *graph);

    GraphView* getVisibleGraphView() const;
    GraphView* getGraphView(const AUUID &uuid) const;

    GraphFacade* getVisibleGraphFacade() const;
    DesignerScene* getVisibleDesignerScene() const;

    NodeAdapterFactory* getNodeAdapterFactory() const;

    bool hasSelection() const;

    void saveSettings(YAML::Node& doc);
    void loadSettings(YAML::Node& doc);

    void saveView(Graph *graph, YAML::Node &e);
    void loadView(Graph* graph, YAML::Node& doc);


    virtual void useProfiler(std::shared_ptr<Profiler> profiler) override;

Q_SIGNALS:
    void selectionChanged();
    void helpRequest(NodeBox*);

public Q_SLOTS:
    void showGraph(UUID uuid);
    void showNodeDialog();
    void showNodeSearchDialog();

    void closeView(int page);

    void addBox(NodeBox* box);
    void removeBox(NodeBox* box);

    void focusOnNode(const AUUID& id);

    void overwriteStyleSheet(QString& stylesheet);

    void updateMinimap();

    void refresh();
    void reset();
    void reinitialize();

    std::vector<NodeBox*> getSelectedBoxes() const;
    void selectAll();
    void clearSelection();
    void deleteSelected();
    void copySelected();
    void paste();
    void groupSelected();
    void ungroupSelected();

private:
    void observe(GraphFacadePtr graph);
    void showGraph(GraphFacadePtr graph);

private:
    Ui::Designer* ui;

    DesignerOptions options_;

    MinimapWidget* minimap_;

    CsApexCore& core_;
    CsApexViewCore& view_core_;

    std::unordered_map<UUID, GraphFacadePtr, UUID::Hasher> graphs_;

    std::set<Graph*> visible_graphs_;
    std::map<Graph*, GraphView*> graph_views_;
    std::map<AUUID, GraphView*> auuid_views_;
    std::map<GraphView*, GraphFacade*> view_graphs_;
    std::map<Graph*, std::vector<slim_signal::ScopedConnection>> graph_connections_;
    std::map<GraphView*, std::vector<slim_signal::ScopedConnection>> view_connections_;

    std::map<UUID, YAML::Node> states_for_invisible_graphs_;

    bool space_;
    bool drag_;
    QPoint drag_start_pos_;

    bool is_init_;

    std::vector<csapex::slim_signal::ScopedConnection> connections_;
};

}
#endif // DESIGNER_H
