#ifndef PARAMETER_MAP_H
#define PARAMETER_MAP_H

/// COMPONENT
#include "parameter.h"

/// SYSTEM
#include <map>
#include <boost/serialization/map.hpp>
#include <boost/serialization/utility.hpp>

namespace param
{

class ParameterMap
{
public:
    ParameterMap();

    Parameter& operator[] (const std::string& name);
    const Parameter::Ptr at(const std::string& name) const;
    Parameter::Ptr at(const std::string& name);

    template<class Archive>
    void serialize(Archive& ar, const unsigned int /*file_version*/) {
        ar & BOOST_SERIALIZATION_NVP(map_);
    }

private:
    std::map<std::string, Parameter::Ptr> map_;
};

}

#endif // PARAMETER_MAP_H
