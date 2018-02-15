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


#include <math.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <QtGui>

#include <QApplication>
#include <QImage>
#include <QLabel>
#include <QSplitter>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QTextStream>
#include <QCloseEvent>
#include <QFileDialog>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QMenu>
#include <QStatusBar>
#include <QToolBar>
#include <QMenuBar>
#include <QMessageBox>
#include <QTableWidgetItem>

#include "imghand.h"

using namespace std;


ImgHand::ImgHand()
  :img( new QImage ),
  imgx( new QImage ),
  scene( new QGraphicsScene( this ) ),
  view( new QGraphicsView( scene ) )
{
  scene->setSceneRect(0, 0, 320, 200); // safe values
  view->setDragMode( QGraphicsView::RubberBandDrag );

  pi1 = pi2 = nullptr;  // without pixmap items at start

  setCentralWidget( view );

  createActions();
  createMenus();
  createToolBars();
  createStatusBar();

  // readSettings();

  setCurrentFile( "" );
}

// void ImgHand::closeEvent(QCloseEvent *event)
// {
//   event->accept();
//   loaded = calibrated = analyzed = 0;
// }

void ImgHand::contextMenuEvent(QContextMenuEvent *event)
{
  QMenu menu( this );
  menu.addAction( viewZoomInAct    );
  menu.addAction( viewZoomOutAct   );
  menu.addAction( viewZoomResetAct );
  menu.addAction( viewZoomFitAct   );
  menu.addAction( viewZoomSelAct   );
  menu.addSeparator();
  menu.addAction( viewSourceAct    );
  menu.addAction( viewResultAct    );
  menu.addSeparator();
  menu.addAction( analyzeAct       );
  menu.addAction( showInfoAct      );

  menu.exec( event->globalPos() );
}


void ImgHand::open()
{
  QString fileName = QFileDialog::getOpenFileName( this );

  if( !fileName.isEmpty() ) {
    loadFile(fileName);
  }
}

void ImgHand::loadFile( const QString &fileName )
{
  if ( ! img->load( fileName ) ) {
    QMessageBox::warning(this, tr("ImgHand"),  tr("Cannot read file %1:").arg(fileName) );
    return;
  }

  QApplication::setOverrideCursor( Qt::WaitCursor );

  *imgx = img->copy();

  if( pi1 != 0 ) {
    scene->removeItem( pi1 );
    delete pi1; pi1 = 0;
  }
  pi1 = scene->addPixmap( QPixmap::fromImage( *img ) );
  if( pi2 != 0 ) {
    scene->removeItem( pi2 );
    delete pi2; pi2 = 0;
  }

  setCurrentFile( fileName );

  // iinf.w = img->width();
  // iinf.h = img->height();
  // iinf.bpp = img->depth();
  // iinf.bypp = iinf.bpp/8;
  // iinf.bpl = img->bytesPerLine();
  // iinf.dpy_x = img->logicalDpiX();
  // iinf.dpy_y = img->logicalDpiY();

  loaded = true;
  scene->setSceneRect( 0, 0, img->width(), img->height() );
  viewZoomFit();

  QApplication::restoreOverrideCursor();
  statusBar()->showMessage( tr("File loaded"), 2000 );
}


void ImgHand::saveAs()
{
  // if( ! analyzed ) {
  //   QMessageBox::warning(this, tr("Missed output image"),
  //           tr("There is no output image to save.  Use 'Analyze' to acquire." ));
  //   return;
  // }
  QString fileName = QFileDialog::getSaveFileName( this );

  if( !fileName.isEmpty() ) {
    imgx->save(fileName);
  }
}


void ImgHand::analyze()
{
  if( !loaded  ) {
    return;
  }

  // imgx->fill( QColor(Qt::black).rgb() );

  // uchar *data1 = imgx->bits();
  // uchar *d1;

  // TEST: check average color

  QApplication::restoreOverrideCursor();

  // analyzed = true;
}


void ImgHand::showInfo()
{
  if( !loaded || !img ) {
    return;
  }
  QString s;
  s = curFile + "\n "
        + QString::number( img->width() ) + " x "
        + QString::number( img->height() ) + ";  bpp: "
        + QString::number( img->depth() )+ ";  bpl: "
        + QString::number( img->bytesPerLine() ) + ";  ";
  QMessageBox::about(this, tr("Image informaton"), s );
}


void ImgHand::showData()
{
  return;
  // if( vdp.empty() ) {
  //   QMessageBox::warning(this, tr("Missed data"),
	// tr("There is no data to show. "
	//   "Use 'Analyze Dir' to acquire data." ));
  //   return;
  // }
  // QDialog *ddi = new QDialog(this);
  //   ddi->setObjectName(QString::fromUtf8("Data"));
  // ddi->resize(560, 320);
  // QVBoxLayout  *verticalLayout = new QVBoxLayout(ddi);
  //
  // QTableWidget *tableWidget = new QTableWidget(ddi);
  // tableWidget->setColumnCount(opt.objects*2+1);
  // tableWidget->setRowCount(vdp.size());
  //
  // verticalLayout->addWidget(tableWidget);
  //
  // QDialogButtonBox *buttonBox = new QDialogButtonBox(ddi);
  // buttonBox->setOrientation(Qt::Horizontal);
  // buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
  //
  // verticalLayout->addWidget(buttonBox);
  //
  // // retranslateUi(ddi);
  // QObject::connect(buttonBox, SIGNAL(accepted()), ddi, SLOT(accept()));
  // QObject::connect(buttonBox, SIGNAL(rejected()), ddi, SLOT(reject()));
  //
  // for( unsigned i=0; i<vdp.size(); ++i ) {
  //   QTableWidgetItem *it = new QTableWidgetItem( QString(vnm[i].c_str() ) );
  //   tableWidget->setItem(i, 0, it);
  //   dpoints cps = vdp[i];
  //   for( int j=0; j<cps.getN(); ++j ) {
  //     dpoint cp = cps[j];
  //     QTableWidgetItem *x = new QTableWidgetItem( QString::number( cp.x ) );
  //     QTableWidgetItem *y = new QTableWidgetItem( QString::number( cp.y ) );
  //     tableWidget->setItem( i, 1+2*j, x );
  //     tableWidget->setItem( i, 2+2*j, y );
  //   }
  // }
  //
  // ddi->exec();
}


void ImgHand::viewZoomIn()
{
  view->scale( 1.2, 1.2 );
}

void ImgHand::viewZoomOut()
{
  view->scale( 1/1.2, 1/1.2 );
}

void ImgHand::viewZoomReset()
{
  view->resetTransform();
}

void ImgHand::viewZoomFit()
{
  view->fitInView( 0, 0, img->width(), img->height(), Qt::KeepAspectRatio );
}

void ImgHand::viewZoomSel()
{
  // QRectF s = scene->selRect();
  // view->fitInView( s, Qt::KeepAspectRatio );
}

void ImgHand::viewSource()
{
  if( pi1 != 0 ) {
    pi1->setZValue( 2 );
  }
  if( pi2 != 0 ) {
    pi2->setZValue( 1 );
  }
}

void ImgHand::viewResult()
{
  if( pi1 != 0 ) {
    pi1->setZValue( 1 );
  }
  if( pi2 != 0 ) {
    pi2->setZValue( 2 );
  }
}



void ImgHand::about()
{
  QMessageBox::about(this, tr("About ImgHand"),
      tr("The <b>ImgHand</b> is a a test "
         "application to maje something with images " ));
}


void ImgHand::createActions()
{

  //  -------------------------- File ------------------------------------
  openAct = new QAction( QIcon(":/icons/fileopen.png"), tr("&Open..."), this );
  openAct->setShortcut( tr("Ctrl+O") );
  openAct->setStatusTip( tr("Open an existing image file") );
  connect( openAct, SIGNAL(triggered()), this, SLOT(open()) );

  saveAsAct = new QAction(QIcon(":/icons/filesaveas.png"), tr("&Save result"), this);
  saveAsAct->setShortcut( tr("Ctrl+S") );
  saveAsAct->setStatusTip( tr("Save result image") );
  connect( saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()) );

  exitAct = new QAction(QIcon(":/icons/exit.png"),tr("E&xit"), this);
  exitAct->setShortcut(tr("Ctrl+Q"));
  exitAct->setStatusTip(tr("Exit the application"));
  connect( exitAct, SIGNAL(triggered()), this, SLOT(close()) );

  // ------------------------------ Image --------------------------------------

  showInfoAct = new QAction( QIcon(":/icons/info.png"), tr("&Image info"), this );
  showInfoAct->setShortcut( tr("Ctrl+I") );
  showInfoAct->setStatusTip( tr("Image info") );
  connect( showInfoAct, SIGNAL(triggered()), this, SLOT(showInfo()) );


  analyzeAct = new QAction( QIcon(":/icons/launch.png"), tr("&Analyze"), this );
  analyzeAct->setShortcut( tr("F9") );
  analyzeAct->setStatusTip( tr("Analyze image") );
  connect( analyzeAct, SIGNAL(triggered()), this, SLOT(analyze()) );

  aboutAct = new QAction(tr("&About"), this );
  aboutAct->setStatusTip(tr("Show the application's About box") );
  connect( aboutAct, SIGNAL(triggered()), this, SLOT(about()) );

  aboutQtAct = new QAction( tr("About &Qt"), this );
  aboutQtAct->setStatusTip( tr("Show the Qt library's About box") );
  connect( aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()) );

  // TODO: tips, shortcuts, icons
  // ---------------------------- View ---------------------
  viewZoomInAct = new QAction(QIcon(":/icons/view_zoomin.png"), tr("Zoom &In"), this );
  connect( viewZoomInAct, SIGNAL(triggered()), this, SLOT(viewZoomIn()) );
  viewZoomInAct->setShortcut( QKeySequence::ZoomIn );
  viewZoomInAct->setStatusTip( tr("Zoom In image") );

  viewZoomOutAct = new QAction( QIcon(":/icons/view_zoomout.png"), tr("Zoom &Out"), this );
  connect( viewZoomOutAct, SIGNAL(triggered()), this, SLOT(viewZoomOut()) );
  viewZoomOutAct->setShortcut( QKeySequence::ZoomOut );
  viewZoomOutAct->setStatusTip( tr("Zoom Out image") );

  viewZoomResetAct = new QAction( QIcon(":/icons/view_zoomreset.png"), tr("&1:1"), this );
  connect( viewZoomResetAct, SIGNAL(triggered()), this, SLOT(viewZoomReset()) );
  viewZoomResetAct->setShortcut( tr("Ctrl+1") );
  viewZoomResetAct->setStatusTip( tr("Set zoom to 1:1") );

  viewZoomFitAct = new QAction( QIcon(":/icons/view_zoomfit.png"), tr("&Fit"), this );
  connect (viewZoomFitAct, SIGNAL(triggered()), this, SLOT(viewZoomFit()) );
  viewZoomFitAct->setShortcut( tr("Ctrl+0") );
  viewZoomFitAct->setStatusTip( tr("Fit image to window") );

  viewZoomSelAct = new QAction( QIcon(":/icons/view_zoomsel.png"),tr("&Selection"), this );
  connect( viewZoomSelAct, SIGNAL(triggered()), this, SLOT(viewZoomSel()) );
  viewZoomSelAct->setShortcut( tr("Ctrl+5") );
  viewZoomSelAct->setStatusTip( tr("Scale to selection" ));

  viewSourceAct = new QAction( QIcon(":/icons/view_source.png"),tr("So&urce"), this );
  connect( viewSourceAct, SIGNAL(triggered()), this, SLOT(viewSource()) );
  viewSourceAct->setShortcut( tr("Ctrl+2") );
  viewSourceAct->setStatusTip( tr("View source image") );

  viewResultAct = new QAction(QIcon(":/icons/view_result.png"),tr("&Result"), this );
  connect(viewResultAct, SIGNAL(triggered()), this, SLOT(viewResult()));
  viewResultAct->setShortcut( tr("Ctrl+3") );
  viewResultAct->setStatusTip( tr("View Result Image") );

}

void ImgHand::createMenus()
{
  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction( openAct );
  fileMenu->addSeparator();
  fileMenu->addAction( saveAsAct );
  fileMenu->addSeparator();
  fileMenu->addAction( exitAct );

  imageMenu = menuBar()->addMenu(tr("&Image"));
  imageMenu->addAction( showInfoAct );
  imageMenu->addSeparator();
  imageMenu->addAction( analyzeAct );

  viewMenu = menuBar()->addMenu(tr("&View"));
  viewMenu->addAction( viewZoomInAct );
  viewMenu->addAction( viewZoomOutAct );
  viewMenu->addAction( viewZoomResetAct );
  viewMenu->addAction( viewZoomFitAct );
  viewMenu->addAction( viewZoomSelAct );
  viewMenu->addSeparator();
  viewMenu->addAction( viewSourceAct );
  viewMenu->addAction( viewResultAct );

  menuBar()->addSeparator();

  helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction( aboutAct );
  helpMenu->addAction( aboutQtAct );
}

void ImgHand::createToolBars()
{
  fileToolBar = addToolBar(tr("Main"));
  fileToolBar->addAction(openAct);
  fileToolBar->addSeparator();

  fileToolBar->addAction(saveAsAct);
  fileToolBar->addSeparator();

  fileToolBar->addAction(viewZoomInAct);
  fileToolBar->addAction(viewZoomOutAct);
  fileToolBar->addAction(viewZoomResetAct);
  fileToolBar->addAction(viewZoomFitAct);
  fileToolBar->addAction(viewZoomSelAct);
  fileToolBar->addSeparator();

  fileToolBar->addAction(viewSourceAct);
  fileToolBar->addAction(viewResultAct);
  fileToolBar->addSeparator();

  fileToolBar->addAction(showInfoAct);
  fileToolBar->addAction(analyzeAct);
  fileToolBar->addSeparator();

  fileToolBar->addAction(exitAct);
}

void ImgHand::createStatusBar()
{
  statBar = statusBar();
  // statBar->addWidget( (labelSizePix =  new QLabel("(XxY).(WxH)")) );
  // statBar->addWidget( (labelSizeU =  new QLabel("(0x0).(0x0)")) );
  // statBar->showMessage(tr("Ready"));
  //statusBar()->
}

// void ImgHand::readSettings()
// {
//   QSettings settings("Trolltech", "ImgHand");
//   QPoint pos = settings.value("pos", QPoint(100, 100)).toPoint();
//   QSize size = settings.value("size", QSize(800, 600)).toSize();
//   resize(size);
//   move(pos);
// }

// void ImgHand::writeSettings()
// {
//   QSettings settings("Trolltech", "ImgHand");
//   settings.setValue("pos", pos());
//   settings.setValue("size", size());
// }


void ImgHand::setCurrentFile(const QString &fileName)
{
  curFile = fileName;
  setWindowModified(false);

  QString shownName;

  if (curFile.isEmpty()) {
    shownName = "empty";
  }  else {
    shownName = strippedName( curFile );
  }

  setWindowTitle( tr("%1[*] - %2").arg(shownName).arg(tr("ImgHand")) );
}

QString ImgHand::strippedName( const QString &fullFileName )
{
  return QFileInfo(fullFileName).fileName();
}

ImgHand::~ImgHand()
{
  delete img;  img  = nullptr;
  delete imgx; imgx = nullptr;

}

// --------------------- calculation functions

// void fillGoodColor( const uchar *src, uchar *dst, const ImageInfo *inf )
// {
//   long long d, sum;
//
//   uchar *d0;
//   const uchar *s0;
//
//   for( int i=inf->wr_s_y; i<inf->wr_e_y; ++i ) {
//     s0 = src + inf->bpl*i;
//     d0 = dst + inf->bpl*i;
//     for( int j=inf->wr_s_b; j<inf->wr_e_b; j+=inf->bypp ) {
//       sum = 0;
//       d = s0[j]   - inf->sp_b; d *= d; sum += d;
//       d = s0[j+1] - inf->sp_g; d *= d; sum += d;
//       d = s0[j+2] - inf->sp_r; d *= d; sum += d;
//       if( sum < inf->tolerance ) {
// 	d0[j] = 255; d0[j+1] = 0; d0[j+2] = 0;
//       };
//     };
//   };
//
// }

