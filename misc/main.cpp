#include "io/imageReader.h"

#include <opencv2/highgui.hpp>

int main()
{
    auto imReader = cvutils::io::ImageReader("/home/lars/gitProjects/ant-tracking-data/imgs", ""); 

    for (size_t i = 0; i < imReader.numImages(); i++)
    {
        cv::imshow("test", imReader.getImage(i));
    }


    return 0;
}
