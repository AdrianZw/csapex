/// HEADER
#include <csapex/view/parameter_context_menu.h>

/// COMPONENT
#include <csapex/view/designer_view.h>

/// PROJECT
#include <utils_param/parameter.h>

/// SYSTEM
#include <QApplication>

using namespace csapex;

ParameterContextMenu::ParameterContextMenu(param::ParameterWeakPtr p)
    : param_(p)
{

}

void ParameterContextMenu::doShowContextMenu(const QPoint& pt)
{
    auto param = param_.lock();
    if(!param) {
        return;
    }

    QWidget* w = dynamic_cast<QWidget*>(parent());
    if(!w) {
        return;
    }

    DesignerView* view = QApplication::activeWindow()->findChild<DesignerView*>();

    QWidget* real_parent = w;
    while(real_parent->parentWidget()) {
        real_parent = real_parent->parentWidget();
    }

    QPoint gpt = view->mapToGlobal(view->mapFromScene(w->mapToGlobal(pt)));

    QMenu menu;
    ContextMenuHandler::addHeader(menu, std::string("Parameter: ") + param->name());

    QAction* connectable = new QAction("connectable", &menu);
    connectable->setCheckable(true);
    connectable->setChecked(param->isInteractive());
    connectable->setIcon(QIcon(":/connector.png"));

    connectable->setIconVisibleInMenu(true);
    menu.addAction(connectable);

    QAction* selectedItem = menu.exec(gpt);
    if (selectedItem) {
        if(selectedItem == connectable) {
            param->setInteractive(!param->isInteractive());
        }
    }
}
