/***************************************************************************
 *   Copyright (C) 2018-2019 by Anton Guda                                 *
 *   atu@nmetau.edu.ua                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#ifndef IMGHAND_H
#define IMGHAND_H

#include <iosfwd>
#include <vector>

#include <QMainWindow>
#include <QString>

#include "imghand_base.h"


class QAction;
class QMenu;
class QImage;
class QLabel;
class QScrollArea;
class QGraphicsView;
class QGraphicsScene;
class QGraphicsPixmapItem;

struct GenerData;

namespace cv {
 class Mat;
};

// misc image and bits related funcs

struct ImgData {
  ImgData();
  unsigned n_pix = 256;      //* fake
  int histo_05p = 13, histo_50p = 128, histo_95p = 243, histo_max = 0, histo_auto = 128, u_level = 128;
  double d1 = 0;   //* calculated fractal dimension
  double c0 = 0;   //* shift in fractal dimension
  double corr = 0; //* correcation coeff
  std::vector<double> histo_r; //* relative counts
  std::vector<double> v_lnr, v_lnN;
};

bool calcImgHisto( const QImage &img, ImgData &ida );
void makeBWImage( const QImage &img, QImage &dst, int level );
bool halfImageBW( const QImage &s, QImage &d );
uint64_t count_bits( const QImage &img, bool count0 = false );
bool boxCount0( const QImage &imgx, ImgData &ida );
void img2mat( const QImage &img, cv::Mat &m );
void mat2img( const cv::Mat &m, QImage &img );

struct FormInfo {
  QPolygonF form;
  QVector<QPointF> subs;
  QVector<double> angs;
};

void paintForm( QPainter &p, const GenerData &gdat, const FormInfo &f, int depth );

class ImgHand : public QMainWindow
{
      Q_OBJECT

public:
      ImgHand();
      ~ImgHand();
      void loadFile( const QString &fileName );

protected:
      // void closeEvent(QCloseEvent *event);
      void contextMenuEvent(QContextMenuEvent *event);

private slots:
      void open();
      void gener();
      void saveAs();

      void viewZoomIn();
      void viewZoomOut();
      void viewZoomReset();
      void viewZoomFit();
      void viewZoomSel();
      void viewSource();
      void viewResult();
      void showInfo();
      void showData();

      void restoreImage();
      void analyze();
      void makeBwSlot();
      void makeBwAdaSlot();
      void boxCount0Slot();

      void sobelSlot();

      void about();


private:
      void createActions();
      void createMenus();
      void createToolBars();
      void createStatusBar();
      void calcHisto();
      void makeBW( int level = 128 );
      void setCurrentFile( const QString &fileName );
      QString strippedName( const QString &fullFileName );
      void updateSrcItem();
      void updateDstItem();

      QImage img;    //* source image
      QImage img_s;  //* saved image
      QImage imgx;   //* image after filters

      // new Scene approach
      QGraphicsScene *scene;
      QGraphicsView *view;
      QGraphicsPixmapItem *pi1 = nullptr, *pi2 = nullptr;

      QString curFile;
      bool loaded = false;
      ImgData ida;

      QVector<FormInfo> forms;

      // =================== User Interface =================
      QStatusBar *statBar;
      QLabel *stat_lbl;
      QMenu *fileMenu;
      // QMenu *editMenu;
      QMenu *imageMenu;
      QMenu *viewMenu;
      // QMenu *selectionMenu;
      QMenu *helpMenu;
      QToolBar *fileToolBar;
      // QToolBar *editToolBar;

      // QAction *newAct;
      QAction *generAct;
      QAction *openAct;
      // QAction *setOptionsAct;
      QAction *restoreImageAct;
      QAction *analyzeAct;
      QAction *showInfoAct;
      QAction *makeBwAct;
      QAction *makeBwAdaAct;
      QAction *boxCount0Act;
      QAction *sobelAct;

      QAction *viewZoomInAct;
      QAction *viewZoomOutAct;
      QAction *viewZoomResetAct;
      QAction *viewZoomFitAct;
      QAction *viewZoomSelAct;
      QAction *viewSourceAct;
      QAction *viewResultAct;


      // QAction *saveAct;
      QAction *saveAsAct;
      QAction *exitAct;
      // QAction *cutAct;
      // QAction *copyAct;
      // QAction *pasteAct;
      QAction *aboutAct;
      QAction *aboutQtAct;
};


#endif

