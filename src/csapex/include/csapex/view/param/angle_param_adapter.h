#ifndef ANGLE_PARAM_ADAPTER_H
#define ANGLE_PARAM_ADAPTER_H

/// COMPONENT
#include <csapex/view/param/param_adapter.h>
#include <csapex/param/angle_parameter.h>

class QHBoxLayout;

namespace csapex
{
class ParameterContextMenu;

class AngleParameterAdapter : public ParameterAdapter
{
public:
    AngleParameterAdapter(param::AngleParameter::Ptr p);

    virtual void setup(QBoxLayout* layout, const std::string& display_name) override;

private:
    void set(double angle);

private:
    param::AngleParameterPtr angle_p_;
};


}

#endif // ANGLE_PARAM_ADAPTER_H