#ifndef META_PORT_H
#define META_PORT_H

/// PROJECT
#include <csapex/view/widgets/port.h>

namespace csapex
{

class MetaPort : public Port
{
    Q_OBJECT

    Q_PROPERTY(QString class READ cssClass)

public:
    MetaPort(ConnectorType port_type, const AUUID &target, QWidget *parent = nullptr);

    QString cssClass() {
        return QString("MetaPort");
    }

    void showContextMenu(const QPoint &pt);

    void dragEnterEvent(QDragEnterEvent* e);
    void dragMoveEvent(QDragMoveEvent* e);
    void dropEvent(QDropEvent* e);

Q_SIGNALS:
    void createPortRequest(const AUUID& target, ConnectorType output, ConnectionTypeConstPtr, std::string, bool);
    void createPortAndConnectRequest(const AUUID& target, Connectable*, ConnectionTypeConstPtr, std::string, bool);
    void createPortAndMoveRequest(const AUUID& target, Connectable*, ConnectionTypeConstPtr, std::string, bool);

private Q_SLOTS:
    void triggerCreatePort();

private:
    ConnectorType port_type_;
    AUUID target;
};

}

#endif // META_PORT_H
