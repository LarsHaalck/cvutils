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

#include "io/imageReader.h"
#include "io/matchesReader.h"
#include "io/featureReader.h"
#include "io/matchesWriter.h"

#include <iostream>

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

    QAction * changeAct = new QAction(tr("&Change..."), this);
    saveAct->setStatusTip(tr("Change matching file"));
    connect(changeAct, &QAction::triggered, this, &MainWindow::change);
    fileMenu->addAction(changeAct);

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
        QFileInfo(imgDir).path(), tr("Txt file (*.txt)"));

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

    const QString matchFile = QFileDialog::getOpenFileName(
        this, tr("Choose a match file"),
        QFileInfo(imgDir).path(), tr("Matches files (*.bin)"));

    if (matchFile.isEmpty())
      return;

    populateScene(imgDir.toStdString(), txtFile.toStdString(), ftDir.toStdString(),
        scale, matchFile.toStdString());
    view->view()->setScene(scene);
    emit view->resetView();
}

void MainWindow::save()
{
    const QString matchFile = QFileDialog::getSaveFileName(
        this, tr("Choose a txt file (can be skipped)"),
        QFileInfo(QString::fromStdString(doc.ftDir)).path(),
        tr("Matches files (*.bin)"));

    MatchesWriter writer(matchFile.toStdString());
    writer.writePairWiseMatches(doc.pairWiseMatches);
}

void MainWindow::change()
{
    if (doc.ftDir.empty())
        return;
    const QString matchFile = QFileDialog::getOpenFileName(
        this, tr("Choose a match file"),
        QFileInfo(QString::fromStdString(doc.ftDir)).path(),
        tr("Matches files (*.bin)"));

    if (matchFile.isEmpty())
      return;

    populateMatches(matchFile.toStdString());
}

void MainWindow::populateScene(const std::string& imgDir, const std::string& txtFile,
    const std::string& ftDir, float scale, const std::string& matchFile)
{
    scene = new QGraphicsScene(this);

    ImageReader imgReader(imgDir, txtFile);
    FeatureReader ftReader(imgDir, txtFile, ftDir);
    doc.ftDir = ftDir;
    doc.scale = scale;

    doc.imgFiles.clear();
    doc.keyPoints.clear();
    for (size_t i = 0; i < imgReader.numImages(); i++)
    {
        doc.imgFiles.push_back(imgReader.getImagePath(i));
        doc.keyPoints.push_back(ftReader.getFeatures(i));
    }

    populateMatches(matchFile);
}
void MainWindow::populateMatches(const std::string& matchFile)
{
    MatchesReader matchReader(matchFile);
    doc.pairWiseMatches = matchReader.moveMatches();
    doc.matchFile = matchFile;

    scene->clear();
    for (const auto& matches : doc.pairWiseMatches)
    {
        size_t idI = matches.first.first;
        size_t idJ = matches.first.second;

        // only for vis purposes
        if (matches.second.empty())
            continue;

        const QColor color(0, 0, 255, 127);
        QGraphicsItem* item = new PairGraphicsItem(color, idI , idJ, matches.second.size());

        item->setPos(QPointF(
            idI * item->boundingRect().width() * 1.1,
            idJ * item->boundingRect().height() * 1.1));
        scene->addItem(item);
    }
}
}
