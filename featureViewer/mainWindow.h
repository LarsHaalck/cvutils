#ifndef CVUTILS_MAIN_WINDOW_H
#define CVUTILS_MAIN_WINDOW_H

#include <QWidget>

#include <opencv2/core.hpp>

class QGraphicsScene;
class QGraphicsView;
class QSlider;
class QSpinBox;
class QLabel;
class QPushButton;
class QString;

namespace cvutils
{
namespace misc
{
class Graphics_view_zoom;
}
}

namespace cvutils
{
class MainWindow : public QWidget
{
    Q_OBJECT
public:
    MainWindow(QWidget* parent = 0);

private:
    QGraphicsScene* mImgScene;
    QGraphicsView* mImgView;
    misc::Graphics_view_zoom* mZoom;
    QSlider* mSlider;
    QSpinBox* mSpinBox;
    QLabel* mNumFrames;
    QPushButton* mPrev;
    QPushButton* mNext;
    std::vector<std::string> mImgFiles;
    std::vector<std::vector<cv::KeyPoint>> mFtFiles;
    float mScale;
    size_t mCurrImg;

    void populateScene(const QString& imgDir, const QString& txtFile,
        const QString& ftDir);
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
