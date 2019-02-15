// This file is part of OpenMVG, an Open Multiple View Geometry C++ library.

// Copyright (c) 2017 <Zillow Inc.> Pierre Moulon

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "matchingpairgraphicsview.h"
#include "pairgraphicsitem.h"
#include "mainframe.h"

#include <QByteArray>
#ifndef QT_NO_WHEELEVENT
#include <QWheelEvent>
#endif

#include <QGraphicsView>
#include <QGraphicsScene>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/highgui.hpp>

#include "qimgcv/qImgCv.h"
#include "zoom/qGraphicsZoom.h"

MatchingPairGraphicsView::MatchingPairGraphicsView
(
 MainFrame *v,
 Document & doc
 )
    : QGraphicsView(), main_frame(v), doc(doc), eraseMode(false), posPress()
{
}

#ifndef QT_NO_WHEELEVENT
void MatchingPairGraphicsView::wheelEvent(QWheelEvent *e)
{
    if (e->modifiers() & Qt::ControlModifier) {
        if (e->delta() > 0)
            main_frame->zoomIn(6);
        else
            main_frame->zoomOut(6);
        e->accept();
    } else {
        QGraphicsView::wheelEvent(e);
    }
}
#endif

void MatchingPairGraphicsView::mousePressEvent(QMouseEvent *event)
{
    posPress = QPoint();
    if (eraseMode)
    {
        posPress = event->pos();
        QGraphicsItem *item = itemAt(posPress);
        if (item)
        {
            PairGraphicsItem* pair_item = dynamic_cast<PairGraphicsItem*>(item);
            if (pair_item)
            {
                unsigned int idI = pair_item->get_x();
                unsigned int idJ = pair_item->get_y();
                doc.pairWiseMatches[std::make_pair(idI, idJ)].clear();
                delete pair_item;
            }
        }
        QGraphicsView::mousePressEvent(event);
        return;
    }
    const QGraphicsItem *item = itemAt(event->pos());
    if (item)
    {
        const PairGraphicsItem * pair_item = dynamic_cast<const PairGraphicsItem*>(item);
        if (pair_item)
        {
            // Launch here a viewer of the pair matches
            unsigned int idI = pair_item->get_x();
            unsigned int idJ = pair_item->get_y();
            auto pair = std::make_pair(idI, idJ);


            if (!doc.pairWiseMatches[pair].empty())
            {

                cv::Mat imgI = cv::imread(doc.imgFiles[idI], cv::IMREAD_UNCHANGED);
                cv::Mat imgJ = cv::imread(doc.imgFiles[idJ], cv::IMREAD_UNCHANGED);
                if (doc.scale != 1)
                {
                    cv::Mat resImgI, resImgJ;
                    cv::resize(imgI, resImgI, cv::Size(0, 0), doc.scale, doc.scale);
                    cv::resize(imgJ, resImgJ, cv::Size(0, 0), doc.scale, doc.scale);
                    imgI = resImgI;
                    imgJ = resImgJ;
                }

                cv::Mat matchesImg;
                cv::drawMatches(imgI, doc.keyPoints[idI], imgJ, doc.keyPoints[idJ],
                    doc.pairWiseMatches[pair], matchesImg);

                std::stringstream title;
                title << doc.imgFiles[idI] << " " << doc.imgFiles[idJ]
                    << " #Matches: " << doc.pairWiseMatches[pair].size();

                QGraphicsScene* scene = new QGraphicsScene(this);
                QGraphicsView* view = new QGraphicsView(scene);
                new Graphics_view_zoom(view);

                scene->addPixmap(QPixmap::fromImage(QtOcv::mat2Image(matchesImg)));
                view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
                view->setWindowTitle( QString::fromStdString(title.str()));
                view->show();
            }
        }
    }

    QGraphicsView::mousePressEvent(event); // this forwards the event to the item
}

void MatchingPairGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    if (eraseMode && posPress != QPoint())
    {
        QPoint posRelease = event->pos();
        auto topLeft = QPoint(qMin(posPress.x(), posRelease.x()),
                qMin(posPress.y(), posRelease.y()));
        auto bottomRight = QPoint(qMax(posPress.x(), posRelease.x()),
                qMax(posPress.y(), posRelease.y()));
        QList<QGraphicsItem*> itemList = items(QRect(topLeft, bottomRight));
        for (int i = 0; i < itemList.size(); i++)
        {
            if (!itemList[i])
                continue;

            PairGraphicsItem* pair_item = dynamic_cast<PairGraphicsItem*>(itemList[i]);
            if (!pair_item)
                return;

            unsigned int idI = pair_item->get_x();
            unsigned int idJ = pair_item->get_y();
            doc.pairWiseMatches[std::make_pair(idI, idJ)].clear();
            delete pair_item;
        }

        posPress = QPoint();
        QGraphicsView::mouseReleaseEvent(event); // forward
        return;
    }
}
