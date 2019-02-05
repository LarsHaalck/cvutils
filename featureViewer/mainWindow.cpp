#include "mainWindow.h"

#include <iostream>

#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QVBoxLayout>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGroupBox>
#include <QSlider>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QSpacerItem>
#include <QFileDialog>
#include <QString>
#include <QInputDialog>


#include <opencv2/imgcodecs.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/imgproc.hpp>
//#include <opencv2/xfeatures2d.hpp>

#include "io/imageReader.h"
#include "io/featureReader.h"
#include "qimgcv/qImgCv.h"
#include "zoom/qGraphicsZoom.h"


namespace cvutils
{
MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    QMenuBar* bar = new QMenuBar(this);
    QMenu* fileMenu = new QMenu(tr("&File"));
    bar->addMenu(fileMenu);

    QAction* openAct = new QAction(tr("&Open..."), nullptr);
    openAct->setShortcuts(QKeySequence::Open);
    connect(openAct, &QAction::triggered, this, &MainWindow::open);
    fileMenu->addAction(openAct);

    mImgScene = new QGraphicsScene(this);
    mImgView = new QGraphicsView(mImgScene);
    mZoom = new Graphics_view_zoom(mImgView);

    QGroupBox* box = new QGroupBox(tr("Playback Control"), this);
    mSlider = new QSlider(Qt::Horizontal, this);
    mSlider->setSingleStep(1);
    mSlider->setMinimum(1);
    connect(mSlider, SIGNAL(sliderMoved(int)), this, SLOT(sliderMoved(int)));
    mSpinBox = new QSpinBox(this);
    connect(mSpinBox, SIGNAL(valueChanged(int)), this,
        SLOT(spinChanged(int)));

    QLabel* slashLabel = new QLabel("/", this);
    mNumFrames = new QLabel("0", this);
    QHBoxLayout* sliderLayout = new QHBoxLayout();
    sliderLayout->addWidget(mSlider);
    sliderLayout->addWidget(mSpinBox);
    sliderLayout->addWidget(slashLabel);
    sliderLayout->addWidget(mNumFrames);

    mPrev = new QPushButton("<<", this);
    mNext = new QPushButton(">>", this);
    connect(mPrev, SIGNAL(clicked()), this, SLOT(prevClicked()));
    connect(mNext, SIGNAL(clicked()), this, SLOT(nextClicked()));
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(mPrev);
    buttonLayout->addWidget(mNext);
    buttonLayout->addStretch();

    QVBoxLayout* boxLayout = new QVBoxLayout();
    boxLayout->addLayout(sliderLayout);
    boxLayout->addLayout(buttonLayout);
    box->setLayout(boxLayout);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(bar);
    layout->addWidget(mImgView);
    layout->addWidget(box);
    setLayout(layout);

    setWindowTitle(tr("Feature Viewer"));
    setGeometry(0, 0, 1000, 800);
}

void MainWindow::open()
{
    const QString imgDir = QFileDialog::getExistingDirectory(
      this, tr("Choose the image directory"), QString::null, QFileDialog::ShowDirsOnly
        | QFileDialog::DontResolveSymlinks);

    if (imgDir.isEmpty())
      return;

    // don't return if this file is empty
    const QString txtFile = QFileDialog::getOpenFileName(
      this, tr("Choose a txt file (can be skipped)"),
      QFileInfo(imgDir).path());

    const QString ftDir = QFileDialog::getExistingDirectory(
      this, tr("Choose the feature directory"), QFileInfo(imgDir).path(),
      QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (ftDir.isEmpty())
      return;

    bool ok;
    auto scale = static_cast<float>(
        QInputDialog::getDouble(this, tr("Scale factor"), tr("Scale factor"), 1.0, 0.0,
            1.0, 2, &ok));
    if (!ok)
        scale = 1.0;

    populateScene(imgDir.toStdString(), txtFile.toStdString(), ftDir.toStdString(),
        scale);

}

void MainWindow::populateScene(const std::string& imgDir, const std::string& txtFile,
    const std::string& ftDir, float scale)
{
    mImgReader = std::make_unique<cvutils::io::ImageReader>(imgDir, txtFile, scale);
    mFtReader = std::make_unique<cvutils::io::FeatureReader>(imgDir, txtFile, ftDir);
    
    //mFtFiles = ftFiles;
    mNumImages = mImgReader->numImages();
    mNumFrames->setText(QString::number(mNumImages));
    mSpinBox->setRange(1, mNumImages);
    mSpinBox->setValue(1);
    mSlider->setMaximum(mNumImages);
    mSlider->setValue(1);

    mImgScene->addPixmap(QPixmap::fromImage(QtOcv::mat2Image(getImg(0))));
    mImgView->fitInView(mImgScene->itemsBoundingRect(), Qt::KeepAspectRatio);
    mCurrImg = 0;

}

cv::Mat MainWindow::getImg(size_t idx)
{
    auto currImg = mImgReader->getImage(idx);
    auto currFts = mFtReader->getFeatures(idx);

    cv::Mat res;
    cv::drawKeypoints(currImg, currFts, res);
    return res;
}



void MainWindow::updateScene()
{
    mImgScene->clear();
    mImgScene->addPixmap(QPixmap::fromImage(QtOcv::mat2Image(getImg(mCurrImg))));
    mImgView->fitInView(mImgScene->itemsBoundingRect(), Qt::KeepAspectRatio);
    mSpinBox->setValue(mCurrImg + 1);
    mSlider->setValue(mCurrImg + 1);

}

void MainWindow::prevClicked()
{
    if (mCurrImg >= 1)
    {
        mCurrImg--;
        updateScene();
    }

}

void MainWindow::nextClicked()
{
    if (mCurrImg + 1 < mNumImages)
    {
        mCurrImg++;
        updateScene();
    }
}

void MainWindow::sliderMoved(int value)
{
    if (mNumImages)
    {
        mCurrImg = value - 1;
        updateScene();
    }
}

void MainWindow::spinChanged(int value)
{
    if (mNumImages)
    {
        mCurrImg = value - 1;
        updateScene();
    }
}
}
