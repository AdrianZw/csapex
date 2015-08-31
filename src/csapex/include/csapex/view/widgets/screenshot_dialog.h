#ifndef SCREENSHOT_DIALOG_H
#define SCREENSHOT_DIALOG_H

/// COMPONENT
#include <csapex/model/model_fwd.h>

/// SYSTEM
#include <QDialog>

class QGraphicsView;
class QRadioButton;
class QDialogButtonBox;
class QAbstractButton;

namespace csapex
{
class ScreenshotDialog : public QDialog
{
    Q_OBJECT

public:
    ScreenshotDialog(GraphWorkerPtr graph, QWidget *widget, QWidget *parent = 0, Qt::WindowFlags f = 0);

private Q_SLOTS:
    void refreshScreenshot();
    void save();
    void handle(QAbstractButton* button);

private:
    GraphWorkerPtr graph_;
    QWidget* widget_;
    QImage image_;
    QGraphicsView* view_;

    QDialogButtonBox* button_box_;
    QRadioButton* rect_full_;
    QRadioButton* rect_scene_;
};

}
#endif // SCREENSHOT_DIALOG_H

