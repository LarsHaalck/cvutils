// This file is part of OpenMVG, an Open Multiple View Geometry C++ library.

// Copyright (c) 2017 <Zillow Inc.> Pierre Moulon

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "pairgraphicsitem.h"

#include <QHBoxLayout>
#include <QSplitter>
#include <QtWidgets>

#include "mainwindow.h"
#include "mainframe.h"

#include "io.h"

namespace cvutils
{
MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    view = new MainFrame("Pairwise matches viewer", doc);
    QMenuBar * menu_bar = new QMenuBar;
    QMenu * fileMenu = new QMenu(tr("&File"));
    menu_bar->addMenu(fileMenu);

    QAction * openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing project"));
    connect(openAct, &QAction::triggered, this, &MainWindow::open);
    fileMenu->addAction(openAct);

    QAction * saveAct = new QAction(tr("&Save..."), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save current matches"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);
    fileMenu->addAction(saveAct);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(menu_bar);
    layout->addWidget(view);
    setLayout(layout);

    setWindowTitle(tr("Pairwise matches viewer"));
}

void MainWindow::open()
{
    const QString imgDir = QFileDialog::getExistingDirectory(
            this, tr("Choose the image directory"),
            QString::null, QFileDialog::ShowDirsOnly
            | QFileDialog::DontResolveSymlinks);
    if (imgDir.isEmpty())
        return;

    // don't return if this string is empty
    const QString txtFile = QFileDialog::getOpenFileName(
        this, tr("Choose a txt file (can be skipped)"),
        QFileInfo(imgDir).path());

    const QString ftDir = QFileDialog::getExistingDirectory(
        this, tr("Choose the feature directory"), QFileInfo(imgDir).path(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (ftDir.isEmpty())
      return;

    bool ok;
    float scale = static_cast<float>(
        QInputDialog::getDouble(this, tr("Scale factor"), tr("Scale factor"), 1.0, 0.0,
            1.0, 2, &ok));
    if (!ok)
        scale = 1.0;

    QStringList items;
    items << "Putative" << "Geometric";
    QString item = QInputDialog::getItem(this, tr("Select match type"), tr("type"),
        items, 0, false, &ok);
    detail::MatchType type = (item == "Putative")
        ? detail::MatchType::Putative
        : detail::MatchType::Geometric;

    populateScene(imgDir.toStdString(), txtFile.toStdString(), ftDir.toStdString(),
        scale, type);
    view->view()->setScene(scene);
    emit view->resetView();
}

void MainWindow::save()
{
    //const QString matches_fileName = QFileDialog::getSaveFileName(
    //        this, tr("Choose a matches.X file"), QString(), tr("matches file (*.bin *.txt)"));
    //if (matches_fileName.isEmpty())
    //    return;

    //if (!Save(doc.matches_provider->pairWise_matches_, matches_fileName.toStdString()))
    //{
    //    std::cerr
    //        << "Cannot save computed matches in: "
    //        << matches_fileName.toStdString();
    //    return;
    //}

}


void MainWindow::populateScene(const std::string& imgDir, const std::string& txtFile,
    const std::string& ftDir, float scale, detail::MatchType type)
{
    scene = new QGraphicsScene(this);

    doc.imgFiles = misc::getImgFiles(imgDir, txtFile);
    doc.keyPoints = misc::getFtVecs(doc.imgFiles, ftDir);
    auto matchPair = misc::getMatches(ftDir, type);
    doc.pairMat = matchPair.first;
    doc.matches = matchPair.second;

    for (int k = 0; k < doc.pairMat.rows; k++)
    {
        size_t i = doc.pairMat.at<int>(k, 0);
        size_t j = doc.pairMat.at<int>(k, 1);

        const QColor color(0, 0, 255, 127);
        QGraphicsItem* item = new PairGraphicsItem(color, i , j, doc.matches[k].size());

        item->setPos(QPointF(
                    i * item->boundingRect().width() * 1.1,
                    j * item->boundingRect().height() * 1.1));
        scene->addItem(item);
    }
}
}
