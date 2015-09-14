/// HEADER
#include <utils_param/parameter.h>

#include <yaml-cpp/yaml.h>
#include <cxxabi.h>

using namespace param;

Parameter::Parameter(const std::string &name, const ParameterDescription &description)
    : name_(name), description_(description), enabled_(true), interactive_(false)
{
}

Parameter::~Parameter()
{
    destroyed(this);
}

void Parameter::setUUID(const std::string& uuid)
{
    uuid_ = uuid;
}

std::string Parameter::UUID() const
{
    return uuid_;
}

void Parameter::setEnabled(bool enabled)
{
    if(enabled != enabled_) {
        enabled_ = enabled;

        parameter_enabled(this, enabled);
    }
}

bool Parameter::isEnabled() const
{
    return enabled_;
}

void Parameter::setInteractive(bool interactive)
{
    if(interactive != interactive_) {
        interactive_ = interactive;

        interactive_changed(this, interactive_);
    }
}

bool Parameter::isInteractive() const
{
    return interactive_;
}

bool Parameter::hasState() const
{
    return true;
}

Parameter::Lock Parameter::lock() const
{
    return Lock(new boost::recursive_mutex::scoped_lock(mutex_));
}

void Parameter::triggerChange()
{
    parameter_changed(this);
}

std::string Parameter::name() const
{
    return name_;
}

const std::type_info& Parameter::type() const
{
    return typeid(void);
}

std::string Parameter::toString() const
{
    return std::string("[") + name() + ": " + toStringImpl() + "]";
}

const ParameterDescription& Parameter::description() const
{
    return description_;
}

std::string Parameter::toStringImpl() const
{
    throw std::logic_error("not implemented");
}

void Parameter::serialize(YAML::Node &n) const
{
    n["type"] = TYPE();
    n["name"] = name();

    if(interactive_) {
        n["interactive"] = interactive_;
    }
    try {
        doSerialize(n);
    } catch(const std::exception& e) {
        std::cerr << "cannot serialize parameter " << name() << ": " << e.what() << std::endl;
        throw e;
    }
}

void Parameter::deserialize(const YAML::Node &n)
{
    if(n["interactive"].IsDefined()) {
        interactive_ = n["interactive"].as<bool>();
    }

    try {
        doDeserialize(n);
    } catch(const std::exception& e) {
        std::cerr << "cannot deserialize parameter " << name() << ": " << e.what() << std::endl;
        throw e;
    }
}

void Parameter::setValueFrom(const Parameter &other)
{
    name_ = other.name_;
    interactive_ = other.interactive_;
    enabled_ = other.enabled_;
    doSetValueFrom(other);
}

void Parameter::clone(const Parameter &other)
{
    name_ = other.name_;
    description_ = other.description_;
    interactive_ = other.interactive_;
    enabled_ = other.enabled_;
    doClone(other);
}

boost::any Parameter::access_unsafe(const Parameter &p) const
{
    return p.get_unsafe();
}

std::string Parameter::type2string(const std::type_info &type)
{
    int status;
    return abi::__cxa_demangle(type.name(), 0, 0, &status);
}

void Parameter::throwTypeError(const std::type_info &a, const std::type_info &b, const std::string& prefix) const
{
    throw std::runtime_error(prefix + std::string("'") + name() + "' is not of type '" + type2string(a) + "' but '" + type2string(b) + "'");
}