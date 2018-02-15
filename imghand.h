/***************************************************************************
 *   Copyright (C) 2018 by Anton Guda                                      *
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
#include <QCloseEvent>
#include <QString>
#include <QGraphicsScene>
#include <QGraphicsView>


class QAction;
class QMenu;
class QImage;
class QLabel;
class QScrollArea;


class ImgHand : public QMainWindow
{
      Q_OBJECT

public:
      ImgHand();
      ~ImgHand();

protected:
      // void closeEvent(QCloseEvent *event);
      void contextMenuEvent(QContextMenuEvent *event);

private slots:
      void open();
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

      void about();

private:
      void createActions();
      void createMenus();
      void createToolBars();
      void createStatusBar();
      void loadFile( const QString &fileName );
      void analyze();
      void setCurrentFile( const QString &fileName );
      QString strippedName( const QString &fullFileName );

      QImage *img;  //* source image
      QImage *imgx; // image after filters

      // new Scene approach
      QGraphicsScene *scene;
      QGraphicsView *view;
      QGraphicsPixmapItem *pi1, *pi2;

      QString curFile;
      bool loaded = false;

      // =================== User Interface =================
      QStatusBar *statBar;
      QMenu *fileMenu;
      // QMenu *editMenu;
      QMenu *imageMenu;
      QMenu *viewMenu;
      // QMenu *selectionMenu;
      QMenu *helpMenu;
      QToolBar *fileToolBar;
      // QToolBar *editToolBar;
      // QAction *newAct;
      QAction *openAct;
      // QAction *setOptionsAct;
      QAction *analyzeAct;
      QAction *showInfoAct;

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

