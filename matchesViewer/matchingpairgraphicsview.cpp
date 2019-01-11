// This file is part of OpenMVG, an Open Multiple View Geometry C++ library.

// Copyright (c) 2017 <Zillow Inc.> Pierre Moulon

// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "matchingpairgraphicsview.h"
#include "pairgraphicsitem.h"
#include "mainframe.h"

#include <QByteArray>
#include <QSvgWidget>
#ifndef QT_NO_WHEELEVENT
#include <QWheelEvent>
#endif

#include "openMVG/matching/svg_matches.hpp"

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
          unsigned int I = pair_item->get_x();
          unsigned int J = pair_item->get_y();
          doc.matches_provider->pairWise_matches_.erase(std::make_pair(I,J));
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

      const unsigned int I = pair_item->get_x();
      const unsigned int J = pair_item->get_y();

      using namespace openMVG::matching;
      const IndMatches & pairwise_matches =
        doc.matches_provider->pairWise_matches_.at(std::make_pair(I,J));

      if (!pairwise_matches.empty())
      {

        using namespace openMVG::sfm;
        using namespace openMVG::features;

        const openMVG::sfm::View * view_I = doc.sfm_data.GetViews().at(I).get();
        const std::string sView_I = stlplus::create_filespec(doc.sfm_data.s_root_path,
         view_I->s_Img_path);
        const openMVG::sfm::View * view_J = doc.sfm_data.GetViews().at(J).get();
        const std::string sView_J = stlplus::create_filespec(doc.sfm_data.s_root_path,
         view_J->s_Img_path);

        // Show pairwise correspondences
        const bool bVertical = false;
        const std::string svg_string =
          Matches2SVGString
          (
            sView_I,
            {view_I->ui_width, view_I->ui_height},
            doc.feats_provider->getFeatures(view_I->id_view),
            sView_J,
            {view_J->ui_width, view_J->ui_height},
            doc.feats_provider->getFeatures(view_J->id_view),
            pairwise_matches,
            bVertical
          );
        QSvgWidget *svg = new QSvgWidget;
        svg->load(QByteArray(svg_string.c_str()));

        std::ostringstream ofs;
        ofs << view_I->s_Img_path << " " << view_J->s_Img_path
          << " #Matches: " << pairwise_matches.size();
        svg->setWindowTitle( QString::fromStdString(ofs.str()));

        svg->show();
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

        unsigned int I = pair_item->get_x();
        unsigned int J = pair_item->get_y();
        doc.matches_provider->pairWise_matches_.erase(std::make_pair(I,J));
        delete pair_item;
      }

      posPress = QPoint();
      QGraphicsView::mouseReleaseEvent(event); // forward
      return;
  }
}
