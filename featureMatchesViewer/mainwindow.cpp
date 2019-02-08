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
    items << "Putative" << "Geometric" << "Putative filtered" << "Geometric filtered";
    QString item = QInputDialog::getItem(this, tr("Select match type"), tr("type"),
        items, 0, false, &ok);
    MatchType type(MatchType::Putative);
    if (item == items[0])
        type = MatchType::Putative;
    else if (item == items[1])
        type = MatchType::Geometric;
    else if (item == items[2])
        type = MatchType::PutativeFiltered;
    else if (item == items[3])
        type = MatchType::GeometricFiltered;

    populateScene(imgDir.toStdString(), txtFile.toStdString(), ftDir.toStdString(),
        scale, type);
    view->view()->setScene(scene);
    emit view->resetView();
}

void MainWindow::save()
{
    if (doc.type == MatchType::Putative
            || doc.type == MatchType::PutativeFiltered)
    {
        MatchesWriter writer(doc.ftDir, MatchType::PutativeFiltered);
        std::vector<size_t> matchSizes(doc.pairMat.rows);

        for(int k = 0; k < doc.pairMat.rows; k++)
        {
            int idI = doc.pairMat.at<int>(k, 0);
            int idJ = doc.pairMat.at<int>(k, 1);
            writer.writeMatches(idI, idJ, doc.matches[k]);
            matchSizes[k] = doc.matches[k].size();
        }
        writer.writePairMat(doc.pairMat, matchSizes);
    }
    else
    {
        MatchesWriter writer(doc.ftDir, MatchType::GeometricFiltered);
        std::vector<size_t> matchSizes(doc.pairMat.rows);

        for(int k = 0; k < doc.pairMat.rows; k++)
        {
            int idI = doc.pairMat.at<int>(k, 0);
            int idJ = doc.pairMat.at<int>(k, 1);
            writer.writeMatches(idI, idJ, doc.matches[k]);
            matchSizes[k] = doc.matches[k].size();
        }
        writer.writePairMat(doc.pairMat, matchSizes);
    }
}


void MainWindow::populateScene(const std::string& imgDir, const std::string& txtFile,
    const std::string& ftDir, float scale, MatchType type)
{
    scene = new QGraphicsScene(this);

    ImageReader imgReader(imgDir, txtFile);
    FeatureReader ftReader(imgDir, txtFile, ftDir);
    MatchesReader matchReader(ftDir, type);

    doc.imgFiles.clear();
    doc.keyPoints.clear();
    for (size_t i = 0; i < imgReader.numImages(); i++)
    {
        doc.imgFiles.push_back(imgReader.getImagePath(i));
        doc.keyPoints.push_back(ftReader.getFeatures(i));
    }

    doc.pairMat = matchReader.getPairMat();
    doc.scale = scale;
    doc.ftDir = ftDir;
    doc.type = type;

    doc.matches.clear();
    for (int k = 0; k < doc.pairMat.rows; k++)
    {
        size_t i = doc.pairMat.at<int>(k, 0);
        size_t j = doc.pairMat.at<int>(k, 1);
        doc.matches.push_back(matchReader.getMatches(i, j));

        // only for vis purposes
        if (doc.matches[k].empty())
            continue;

        const QColor color(0, 0, 255, 127);
        QGraphicsItem* item = new PairGraphicsItem(color, i , j, doc.matches[k].size());

        item->setPos(QPointF(
                    i * item->boundingRect().width() * 1.1,
                    j * item->boundingRect().height() * 1.1));
        scene->addItem(item);
    }
}
}
