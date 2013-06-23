#ifndef IMAGE_PROVIDER_BAG_H
#define IMAGE_PROVIDER_BAG_H

/// COMPONENT
#include "image_provider_set.h"

namespace vision_evaluator
{

class ImageProviderMov : public ImageProviderSet
{
    Q_OBJECT

protected:
    ImageProviderMov(const std::string& movie_file);
    ~ImageProviderMov();

public:
    static ImageProvider* createInstance(const std::string& path);

    virtual bool hasNext();
    virtual void reallyNext(cv::Mat& img, cv::Mat& mask);

private:
    void display();

private:
    cv::VideoCapture capture_;
};

} /// NAMESPACE

#endif // IMAGE_PROVIDER_BAG_H
