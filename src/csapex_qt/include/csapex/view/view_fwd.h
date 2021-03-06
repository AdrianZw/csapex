#ifndef VIEW_FWD_H
#define VIEW_FWD_H

/// shared_ptr
#include <memory>

#define FWD(name)                                                                                                                                                                                      \
    class name;                                                                                                                                                                                        \
    typedef std::shared_ptr<name> name##Ptr;                                                                                                                                                           \
    typedef std::unique_ptr<name> name##UniquePtr;                                                                                                                                                     \
    typedef std::weak_ptr<name> name##WeakPtr;                                                                                                                                                         \
    typedef std::shared_ptr<const name> name##ConstPtr;

namespace csapex
{
FWD(TracingTimeline)
FWD(TracingLegend)
FWD(NodeBox)
FWD(MessageRenderer)
FWD(NodeAdapter)
FWD(DefaultNodeAdapter)
FWD(NodeAdapterBuilder)
FWD(NodeAdapterFactory)
FWD(Port)
FWD(ParameterAdapter)
FWD(ParameterAdapterBuilder)
FWD(ParameterAdapterFactory)

class CsApexViewCore;

class PortPanel;

class Designer;
class DesignerOptions;
class DesignerScene;
class GraphView;
class MinimapWidget;

class MovableGraphicsProxyWidget;
class ProfilingWidget;
class MessagePreviewWidget;

class DragIO;
FWD(DragIOHandler)
}  // namespace csapex

#undef FWD

#endif  // VIEW_FWD_H
