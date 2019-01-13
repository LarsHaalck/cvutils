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

#include "io.h"
#include "qImgCv.h"
#include "qGraphicsZoom.h"


namespace cvutils
{
MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{

    QMenuBar* bar = new QMenuBar(this);
    QMenu* fileMenu = new QMenu(tr("&File"));
    bar->addMenu(fileMenu);

    QAction* openAct = new QAction(tr("&Open..."));
    openAct->setShortcuts(QKeySequence::Open);
    connect(openAct, &QAction::triggered, this, &MainWindow::open);
    fileMenu->addAction(openAct);

    mImgScene = new QGraphicsScene(this);
    mImgView = new QGraphicsView(mImgScene);
    mZoom = new misc::Graphics_view_zoom(mImgView);

    QGroupBox* box = new QGroupBox(tr("Playback Control"), this);
    mSlider = new QSlider(Qt::Horizontal, this);
    mSlider->setSingleStep(1);
    mSlider->setMinimum(1);
    connect(mSlider, SIGNAL(sliderMoved(int)), this, SLOT(sliderMoved(int)));
    mSpinBox = new QSpinBox(this);
    connect(mSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this,
        [=](int i){ spinChanged(i); });

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
    mScale = static_cast<float>(
        QInputDialog::getDouble(this, tr("Scale factor"), tr("Scale factor"), 1.0, 0.0,
            1.0, 2, &ok));
    if (!ok)
        mScale = 1.0;

    populateScene(imgDir, txtFile, ftDir);

}

void MainWindow::populateScene(const QString& imgDir, const QString& txtFile,
    const QString& ftDir)
{
    auto imgFiles = misc::getImgFiles(imgDir.toStdString(), txtFile.toStdString());
    auto ftFiles = misc::getFtVecs(imgFiles, ftDir.toStdString());

    mImgs.reserve(imgFiles.size());
    for (size_t i = 0; i < imgFiles.size(); i++)
    {
        cv::Mat currImg = cv::imread(imgFiles[i], cv::IMREAD_UNCHANGED);
        if (mScale != 1)
        {
            cv::Mat resImg;
            cv::resize(currImg, resImg, cv::Size(0, 0), mScale, mScale);
            currImg = resImg;
        }

        cv::Mat res;
        cv::drawKeypoints(currImg, ftFiles[i], res);
        mImgs.push_back(res);
    }

    mNumFrames->setText(QString::number(mImgs.size()));
    mSpinBox->setRange(1, imgFiles.size());
    mSpinBox->setValue(1);
    mSlider->setMaximum(imgFiles.size());
    mSlider->setValue(1);

    mImgScene->addPixmap(QPixmap::fromImage(QtOcv::mat2Image(mImgs[0])));
    mImgView->fitInView(mImgScene->itemsBoundingRect(), Qt::KeepAspectRatio);
    mCurrImg = 0;

}

void MainWindow::updateScene()
{
    mImgScene->clear();
    mImgScene->addPixmap(QPixmap::fromImage(QtOcv::mat2Image(mImgs[mCurrImg])));
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
    if (mCurrImg + 1 < mImgs.size())
    {
        mCurrImg++;
        updateScene();
    }
}

void MainWindow::sliderMoved(int value)
{
    mCurrImg = value - 1;
    updateScene();
}

void MainWindow::spinChanged(int value)
{
    mCurrImg = value - 1;
    updateScene();
}
}
