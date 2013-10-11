/// HEADER
#include "extract_timestamp.h"

/// PROJECT
#include <csapex/model/box.h>
#include <csapex/model/connector_in.h>
#include <csapex/model/connector_out.h>
#include <csapex_transform/time_stamp_message.h>

/// SYSTEM
#include <pluginlib/class_list_macros.h>

PLUGINLIB_EXPORT_CLASS(csapex::ExtractTimeStamp, csapex::BoxedObject)

using namespace csapex;
using namespace csapex::connection_types;

ExtractTimeStamp::ExtractTimeStamp()
{
    addTag(Tag::get("PointCloud"));
    addTag(Tag::get("Time"));
}

void ExtractTimeStamp::fill(QBoxLayout *layout)
{
    box_->setSynchronizedInputs(true);

    input_ = new ConnectorIn(box_, 0);
    input_->setLabel("PointCloud");
    input_->setType(connection_types::PointCloudMessage::make());

    output_ = new ConnectorOut(box_, 0);
    output_->setLabel("Time");
    output_->setType(connection_types::TimeStampMessage::make());

    box_->addInput(input_);
    box_->addOutput(output_);
}

void ExtractTimeStamp::allConnectorsArrived()
{
    PointCloudMessage::Ptr msg(input_->getMessage<PointCloudMessage>());

    boost::apply_visitor (PointCloudMessage::Dispatch<ExtractTimeStamp>(this), msg->value);
}

template <class PointT>
void ExtractTimeStamp::inputCloud(typename pcl::PointCloud<PointT>::Ptr cloud)
{
    connection_types::TimeStampMessage::Ptr time(new connection_types::TimeStampMessage);
    time->value = time->value.fromNSec(cloud->header.stamp * 1000);

    output_->publish(time);
}
