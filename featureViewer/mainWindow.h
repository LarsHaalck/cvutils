#ifndef CVUTILS_MAIN_WINDOW_H
#define CVUTILS_MAIN_WINDOW_H

#include <string>
#include <memory>

#include <QWidget>

#include <opencv2/core.hpp>

#include "io/imageReader.h"
#include "io/featureReader.h"

class QGraphicsScene;
class QGraphicsView;
class QSlider;
class QSpinBox;
class QLabel;
class QPushButton;
class QString;

class Graphics_view_zoom;

namespace cvutils
{
class MainWindow : public QWidget
{
    Q_OBJECT
private:
    QGraphicsScene* mImgScene;
    QGraphicsView* mImgView;
    Graphics_view_zoom* mZoom;
    QSlider* mSlider;
    QSpinBox* mSpinBox;
    QLabel* mNumFrames;
    QPushButton* mPrev;
    QPushButton* mNext;
    size_t mCurrImg;
    size_t mNumImages;
    std::unique_ptr<ImageReader> mImgReader;
    std::unique_ptr<FeatureReader> mFtReader;

public:
    MainWindow(QWidget* parent = 0);

private:
    void populateScene(const std::string& imgDir, const std::string& txtFile,
        const std::string& ftDir, float scale);
    cv::Mat getImg(size_t idx);
    void updateScene();


private slots:
    void open();
    void prevClicked();
    void nextClicked();
    void sliderMoved(int value);
    void spinChanged(int value);
};
}

#endif // CVUTILS_MAIN_WINDOW_H
