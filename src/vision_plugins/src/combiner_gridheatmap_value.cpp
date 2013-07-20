#include "combiner_gridheatmap_value.h"

/// SYSTEM
#include <pluginlib/class_list_macros.h>

PLUGINLIB_EXPORT_CLASS(vision_evaluator::GridHeatMapValue, vision_evaluator::BoxedObject)

using namespace vision_evaluator;
using namespace QSignalBridges;
using namespace cv_grid;

GridHeatMapValue::GridHeatMapValue() :
    GridCompareValue(State::Ptr(new State))
{
    private_state_ghv_ = dynamic_cast<State*>(state_.get());
    assert(private_state_ghv_);
}

cv::Mat GridHeatMapValue::combine(const cv::Mat img1, const cv::Mat mask1, const cv::Mat img2, const cv::Mat mask2)
{
    if(!img1.empty() && !img2.empty()) {
        /// PREPARE
        if(img1.channels() != img2.channels())
            throw std::runtime_error("Channel count is not matching!");
        if(img1.channels() > 4)
            throw std::runtime_error("Channel limit 4!");
        if(img1.rows > img2.rows || img1.cols > img2.cols)
            throw std::runtime_error("Image 1 must have smaller or euqal size!");


        if(private_state_gcv_->channel_count != img1.channels()) {
            private_state_gcv_->channel_count = img1.channels();
            Q_EMIT modelChanged();
        }

        updateSliderMaxima(img1.cols, img1.rows, img2.cols, img2.rows);

        /// COMPUTE
        if(eps_sliders_.size() == private_state_gcv_->channel_count) {
            GridScalar g1, g2;
            prepareGrids(g1, g2, img1, img2, mask1, mask2);

            cv::Mat values;
            grid_heatmap(g1, g2, values);

            cv::Mat out;
            render_heatmap(values, cv::Size(10,10), out);
            return out;
        }
    }

    return cv::Mat();

}

void GridHeatMapValue::updateState(int value)
{
    private_state_ghv_->grid_width  = slide_width_->value();
    private_state_ghv_->grid_height = slide_height_->value();
    private_state_ghv_->grid_width_add1  = slide_width_add1_->value();
    private_state_ghv_->grid_height_add1 = slide_height_add1_->value();
}

void GridHeatMapValue::addSliders(QBoxLayout *layout)
{
    slide_width_       = QtHelper::makeSlider(layout, "Grid 1 Width",  64, 1, 640);
    slide_height_      = QtHelper::makeSlider(layout, "Grid 1 Height", 48, 1, 640);
    slide_width_add1_  = QtHelper::makeSlider(layout, "Grid 2 Width",  64, 1, 640);
    slide_height_add1_ = QtHelper::makeSlider(layout, "Grid 2 Height", 48, 1, 640);

}

void GridHeatMapValue::fill(QBoxLayout *layout)
{
    GridCompareValue::fill(layout);
    connect(slide_height_add1_, SIGNAL(valueChanged(int)), this, SLOT(updateState(int)));
    connect(slide_width_add1_, SIGNAL(valueChanged(int)), this, SLOT(updateState(int)));

    limit_sliders_height_.reset(new QAbstractSliderLimiter(slide_height_, slide_height_add1_));
    limit_sliders_width_.reset(new QAbstractSliderLimiter(slide_width_, slide_width_add1_));
}

void GridHeatMapValue::updateSliderMaxima(int width, int height, int width_add1, int height_add1)
{
    GridCompare::updateSliderMaxima(width, height);
    if(private_state_ghv_->grid_height_max_add1 != height_add1) {
        private_state_ghv_->grid_height_max_add1 = height_add1;
        slide_height_add1_->setMaximum(height_add1);
    }
    if(private_state_ghv_->grid_width_max_add1 != width_add1) {
        private_state_ghv_->grid_width_max_add1 = width_add1;
        slide_width_add1_->setMaximum(width_add1);
    }

}

/// MEMENTO ------------------------------------------------------------------------------------
Memento::Ptr GridHeatMapValue::getState() const
{
    State::Ptr memento(new State);
    *memento = *boost::dynamic_pointer_cast<State>(state_);
    return memento;
}

void GridHeatMapValue::setState(Memento::Ptr memento)
{
    state_.reset(new State);
    State::Ptr s = boost::dynamic_pointer_cast<State>(memento);
    assert(s.get());
    *boost::dynamic_pointer_cast<State>(state_) = *s;
    assert(state_.get());
    private_state_gcv_ = boost::dynamic_pointer_cast<GridCompareValue::State>(state_).get();
    assert(private_state_gcv_);
    private_state_ghv_ = boost::dynamic_pointer_cast<State>(state_).get();
    assert(private_state_ghv_);


    slide_height_->setValue(private_state_ghv_->grid_height);
    slide_width_->setValue(private_state_ghv_->grid_width);
    slide_height_add1_->setValue(private_state_ghv_->grid_height_add1);
    slide_width_add1_->setValue(private_state_ghv_->grid_width_add1);

    Q_EMIT modelChanged();
}

GridHeatMapValue::State::State() :
    GridCompareValue::State::State(),
    grid_width_add1(48),
    grid_height_add1(64),
    grid_width_max_add1(640),
    grid_height_max_add1(480)
{
}

void GridHeatMapValue::State::readYaml(const YAML::Node &node)
{
    GridHeatMapValue::State::readYaml(node);
    node["grid_width_add1"] >> grid_width_add1;
    node["grid_height_add1"] >> grid_height_add1;
    node["grid_width_max_add1"] >> grid_width_max_add1;
    node["grid_height_max_add1"] >> grid_height_max_add1;
}

void GridHeatMapValue::State::writeYaml(YAML::Emitter &out) const
{
    GridHeatMapValue::State::writeYaml(out);
    out << YAML::Key << "grid_width_add1" << YAML::Value << grid_width_add1;
    out << YAML::Key << "grid_height_add1" << YAML::Value << grid_height_add1;
    out << YAML::Key << "grid_width_max_add1" << YAML::Value << grid_width_max_add1;
    out << YAML::Key << "grid_height_max_add1" << YAML::Value << grid_height_max_add1;
}