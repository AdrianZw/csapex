#ifndef CSAPEX_WINDOW_H
#define CSAPEX_WINDOW_H

/// COMPONENT
#include <csapex/csapex_fwd.h>
#include <csapex/core/csapex_core.h>

/// SYSTEM
#include <QMainWindow>
#include <QTimer>
#include <QFileSystemWatcher>

namespace Ui
{
class EvaluationWindow;
}

namespace csapex
{

/**
 * @brief The EvaluationWindow class provides the window for the evaluator program
 */
class CsApexWindow : public QMainWindow, public CsApexCore::Listener
{
    Q_OBJECT

public:
    /**
     * @brief EvaluationWindow
     * @param parent
     */
    explicit CsApexWindow(CsApexCore &core, CommandDispatcher *cmd_dispatcher, GraphPtr graph, Designer *designer, QWidget* parent = 0);
    virtual ~CsApexWindow();

    void showMenu();
    void closeEvent(QCloseEvent* event);

    void resetSignal();

private Q_SLOTS:
    void updateMenu();
    void updateTitle();
    void tick();
    void hideLog();
    void scrollDownLog();
    void init();
    void loadStyleSheet(const QString& path);
    void updateDeleteAction();

public Q_SLOTS:
    void save();
    void saveAs();
    void saveAsCopy();
    void load();
    void reload();
    void reset();
    void clear();
    void undo();
    void redo();

    void start();
    void showStatusMessage(const std::string& msg);
    void reloadBoxMenues();

    void saveSettings(YAML::Emitter& e);
    void loadSettings(YAML::Node& doc);

    void nodeAdded(NodePtr node);
    void nodeRemoved(NodePtr node);

    void openSubGraph(Group* grp);

Q_SIGNALS:
    void statusChanged(const QString& status);

private:
    void construct();
    void loadStyleSheet();

private:
    CsApexCore& core_;
    CommandDispatcher* cmd_dispatcher_;

    GraphPtr graph_;

    Ui::EvaluationWindow* ui;

    Designer* designer_;

    QTimer timer;

    bool init_;

    QString style_sheet_;
    QFileSystemWatcher* style_sheet_watcher_;
};

} /// NAMESPACE

#endif // CSAPEX_WINDOW_H
