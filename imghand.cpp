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


#include <cmath>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <chrono>

#include <QtGui>
#include <QtDebug>

#include <QtCharts>

#include <gsl/gsl_statistics.h>
#include <gsl/gsl_fit.h>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
// #include <opencv2/highgui.hpp>


#include "imghand.h"
#include "sobeldialog.h"

using namespace std;
using namespace QtCharts;

using namespace cv;

// number of '1' bits in byte
static const int bit_tab[256] = {
  0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
};

ImgData::ImgData()
{
  histo_r.assign( 256, 0.0 );
  histo_r[0] = 1.0;
  v_lnr.reserve( 32 );
  v_lnN.reserve( 32 );
}

ImgHand::ImgHand()
  : scene( new QGraphicsScene( this ) ),
  view( new QGraphicsView( scene ) )
{

  scene->setSceneRect( 0, 0, 800, 800 ); // safe values
  view->setDragMode( QGraphicsView::RubberBandDrag );

  setCentralWidget( view );

  createActions();
  createMenus();
  createToolBars();
  createStatusBar();

  // readSettings();

  setCurrentFile( "" );
}

// void ImgHand::closeEvent( QCloseEvent *event )
// {
//   event->accept();
//   loaded = calibrated = analyzed = 0;
// }

void ImgHand::contextMenuEvent( QContextMenuEvent *event )
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
  QString fileName = QFileDialog::getOpenFileName( this, "Image file", QString(), "Images (*.png *.tif *.jpg)" );

  if( !fileName.isEmpty() ) {
    loadFile( fileName );
  }
}

void ImgHand::updateSrcItem()
{
  if( pi1 ) {
    scene->removeItem( pi1 );
    delete pi1; pi1 = nullptr;
  }
  pi1 = scene->addPixmap( QPixmap::fromImage( img ) );
}

void ImgHand::updateDstItem()
{
  if( pi2 ) {
    scene->removeItem( pi2 );
    delete pi2; pi2 = nullptr;
  }
  pi2 = scene->addPixmap( QPixmap::fromImage( imgx ) );
}

void ImgHand::loadFile( const QString &fileName )
{
  {
    QImage img0;
    if ( ! img0.load( fileName ) ) {
      QMessageBox::warning( this, tr("ImgHand"),  tr("Cannot read file %1:").arg( fileName ) );
      return;
    }

    QApplication::setOverrideCursor( Qt::WaitCursor );

    img_s  = img0.convertToFormat( QImage::Format_Grayscale8, Qt::ThresholdDither );
  }

  loaded = true;
  setCurrentFile( fileName );
  ida.n_pix = img_s.width() * img_s.height();
  scene->setSceneRect( 0, 0, img_s.width(), img_s.height() );
  QString s = QSL( "File \"" ) % curFile % QSL( "\" " ) %
    QSN( img_s.width() ) % QSL( "x" ) % QSN( img_s.height() ) %
    QSL( " loaded" );
  stat_lbl->setText( s );

  if( global_debug > 0 ) {
    cerr << qPrintable( s ) << endl;
  }
  if( global_debug > 1 ) {
    cerr << "Scene: " << scene->width() << 'x' << scene->height() << endl;
    cerr << "View:  " <<  view->width() << 'x' << view->height()  << endl;
    cerr << "MainW: " <<  this->width() << 'x' << this->height()  << endl;
  }

  restoreImage();

  makeBW( ida.histo_auto ); // pi2 added here

  QApplication::restoreOverrideCursor();
}

void ImgHand::restoreImage()
{
  if( !loaded ) {
    return;
  }
  img = img_s.copy();

  updateSrcItem();
  calcHisto();
  viewZoomFit();
  viewSource();
}

void ImgHand::calcHisto()
{
  calcImgHisto( img, ida );
}

void ImgHand::makeBW( int level )
{
  makeBWImage( img, imgx, level );
  ida.u_level = level;

  updateDstItem();
  QString s = QSL( "File \"" ) % curFile % QSL( "\" " ) %
    QSN( img_s.width() ) % QSL( "x" ) % QSN( img_s.height() ) %
    QSL( " B/W recalculated: level " ) % QSN( level );
  if( global_debug > 0 ) {
    cerr << qPrintable( s ) << endl;
  }
  stat_lbl->setText( s );
}

void ImgHand::makeBwSlot()
{
  if( !loaded ) {
    return;
  }
  QString lbl = " p05: "
        % QSN( ida.histo_05p ) + ";  p50: "
        % QSN( ida.histo_50p ) + ";  p95: "
        % QSN( ida.histo_95p ) + ";  p_max: "
        % QSN( ida.histo_max ) ;
  bool ok;
  int level = QInputDialog::getInt( this, "Input white level", lbl, ida.histo_auto, -255, 255, 1, &ok );
  if( !ok ) {
    return;
  }

  makeBW( level );
  viewResult();
}


void ImgHand::makeBwAdaSlot()
{
  if( !loaded ) {
    return;
  }

  bool ok;
  int r = QInputDialog::getInt( this, "Input", "Input neibour size", 101, 3, 2048, 2, &ok );
  if( !ok ) {
    return;
  }
  r |= 1; // 3,5,7...

  Mat mi, mo;
  QImage img_t = img.copy();
  img2mat( img, mi );

  //                 src dst max   method                      type           size C
  adaptiveThreshold( mi, mo, 255, ADAPTIVE_THRESH_GAUSSIAN_C,  THRESH_BINARY, r,   1 );
  mat2img( mo, img );

  calcHisto();
  makeBW( ida.histo_auto ); // pi2 added here

  img = img_t; // restore original
  calcHisto();
  updateSrcItem();

  QString s = QSL( "File\"" ) % curFile % QSL( "\" " ) %
    QSN( img_s.width() ) % QSL( "x" ) % QSN( img_s.width() ) %
    QSL( " B/W adaptive R= " ) % QSN( r );
  stat_lbl->setText( s );
  viewResult();
}

void ImgHand::saveAs()
{
  QString fileName = QFileDialog::getSaveFileName( this );

  if( !fileName.isEmpty() ) {
    imgx.save( fileName );
  }
}


void ImgHand::analyze()
{
  if( !loaded  ) {
    return;
  }

  QApplication::restoreOverrideCursor();

  // analyzed = true;
}


void ImgHand::showInfo()
{
  if( !loaded ) {
    return;
  }
  QString s = curFile               % QSL( "\n " )
        % QSN( img.width() )        % QSL( " x " )
        % QSN( img.height() )       % QSL( ";  bpp: " )
        % QSN( img.depth() )        % QSL( ";  bpl: " )
        % QSN( img.bytesPerLine() ) % QSL( ";  n_pix: " )
        % QSN( ida.n_pix )              % QSL( ";  \n p05: " )
        % QSN( ida.histo_05p )          % QSL( ";  p50: " )
        % QSN( ida.histo_50p )          % QSL( ";  p95: " )
        % QSN( ida.histo_95p )          % QSL( ";  p_max: " )
        % QSN( ida.histo_max )          % QSL( ";  p_auto:" )
        % QSN( ida.histo_auto )         % QSL( ";  u_level: " )
        % QSN( ida.u_level )            % QSL( ";" );

  QDialog *dia = new QDialog( this );
  QVBoxLayout *lay = new QVBoxLayout;

  QLineSeries *ser0 = new QLineSeries;
  for( int i=0; i< 256; ++i ) {
    ser0->append( i, ida.histo_r[i] );
  }
  QAreaSeries *series = new QAreaSeries( ser0 );
  series->setColor( Qt::black );

  QScatterSeries *points = new QScatterSeries();
  points->setMarkerShape( QScatterSeries::MarkerShapeCircle );
  points->setMarkerSize( 5.0 );
  points->setColor( Qt::red );
  points->append( ida.histo_05p, 0 );
  points->append( ida.histo_50p, 0 );
  points->append( ida.histo_95p, 0 );

  QScatterSeries *points1 = new QScatterSeries();
  points1->setMarkerShape( QScatterSeries::MarkerShapeCircle );
  points1->setMarkerSize( 5.0 );
  points1->setColor( Qt::green );
  points1->append( ida.histo_max, 0 );

  QScatterSeries *points2 = new QScatterSeries();
  points2->setMarkerShape( QScatterSeries::MarkerShapeCircle );
  points2->setMarkerSize( 7.0 );
  points2->setColor( Qt::blue );
  points2->append( ida.histo_auto, 0 );

  QChart *chart = new QChart;
  chart->addSeries( series );
  chart->addSeries( points  );
  chart->addSeries( points1 );
  chart->addSeries( points2 );

  // chart->setTitle( "Histogram" );
  chart->createDefaultAxes();
  auto ax_x = chart->axes( Qt::Horizontal ).back();
  ax_x->setRange( 0, 256 );
  ax_x->setLinePenColor( Qt::black );
  auto ax_y = chart->axes( Qt::Vertical ).back();
  ax_y->setLabelsVisible( true );
  chart->legend()->setVisible( false );

  QChartView *chartView = new QChartView( chart );
  chartView->setRenderHint( QPainter::Antialiasing );
  lay->addWidget( chartView );


  QLabel *l1 = new QLabel( s, dia );
  l1->setTextInteractionFlags( Qt::TextSelectableByMouse
      | Qt::LinksAccessibleByKeyboard | Qt::TextSelectableByKeyboard );
  lay->addWidget( l1 );

  QPushButton *done = new QPushButton( "Done", dia );
  done->setDefault( true );
  lay->addWidget( done );
  connect( done, &QPushButton::clicked, dia, &QDialog::accept );

  dia->setLayout( lay );
  dia->resize( 768, 500 );

  dia->exec();

  // TODO: sepatate action
  QPixmap chart_pix( chartView->size() );
  QPainter pa_t( &chart_pix );
  chartView->render( &pa_t );
  QFileInfo fi( curFile );
  QString hi_file = fi.baseName() + QSL("_histo.png");
  cerr << "hi_file: \"" << qPrintable( hi_file) << "\"" << endl;
  chart_pix.save( hi_file, "PNG" );

  delete dia;
}

void ImgHand::boxCount0Slot()
{
  if( !loaded ) {
    return;
  }

  if( !boxCount0( imgx, ida ) ) {
    return;
  }

  cout << "File: \"" << qPrintable( curFile )
       << "\" Corr: " << ida.corr << " d1: " << ida.d1 << " c0: " << ida.c0 << endl;

  QString s = QString( "File: \"%1\" C1: %2, Corr: %3" ).arg( curFile ).arg( ida.d1 ).arg( ida.corr );

  QDialog *dia = new QDialog( this );
  QVBoxLayout *lay = new QVBoxLayout;

  QLineSeries *ser0 = new QLineSeries;
  QLineSeries *ser1 = new QLineSeries;
  for( unsigned i=0; i<ida.v_lnr.size() ; ++i ) {
    double x = ida.v_lnr[i];
    ser0->append( x, ida.v_lnN[i] );
    ser1->append( x, ida.c0 - ida.d1 * x );
  }
  ser0->setPen( QColor( Qt::black ) );
  ser0->setPointsVisible( true );

  QChart *chart = new QChart;
  chart->addSeries( ser0 );
  chart->addSeries( ser1 );
  chart->createDefaultAxes();
  auto ax_x = chart->axes( Qt::Horizontal ).back();
  // ax_x->setRange( 0, 256 );
  ax_x->setLinePenColor( Qt::black );
  chart->legend()->setVisible( false );
  QChartView *chartView = new QChartView( chart );
  chartView->setRenderHint( QPainter::Antialiasing );
  lay->addWidget( chartView );

  QLabel *l1 = new QLabel( s, dia );
  l1->setTextInteractionFlags( Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard );
  lay->addWidget( l1 );

  QPushButton *done = new QPushButton( "Done", dia );
  done->setDefault( true );
  lay->addWidget( done );
  connect( done, &QPushButton::clicked, dia, &QDialog::accept );

  dia->setLayout( lay );
  dia->resize( 768, 500 );

  dia->exec();
  delete dia;

  stat_lbl->setText( curFile + " " + QSN( ida.d1 ) );
}


void ImgHand::sobelSlot()
{
  SobelData sd;
  SobelDialog *dia = new SobelDialog( sd, this );
  auto rc = dia->exec();
  delete dia;

  if( rc != QDialog::Accepted ) {
    return;
  }

  Mat mat;
  img2mat( img, mat );

  Mat mat1; // = mat;

  QString estr;
  try {
    Sobel( mat, mat1, mat.depth(), sd.dx, sd.dy, sd.ksize, sd.scale, sd.delta );
  }
  catch( cv::Exception &e ) {
    estr = e.msg.c_str();
    // cerr << "Sobel error: " << e.msg << endl;
    // return;
  }

  if( ! estr.isEmpty() ) {
    QMessageBox::warning( this, QSL("Error"), estr );
    return;
  }

  mat2img( mat1, img );

  updateSrcItem();
  calcHisto();
  makeBW( ida.histo_auto ); // pi2 added here
  viewSource();

  // Mat ker = ( Mat_<char>( 3, 3 ) <<
  //      0, -1,  0,
  //     -1,  4, -1,
  //      0, -1,  0 );
  // Mat mat2;
  // cvtColor( mat1, mat2, CV_BGR2RGB );
  //
  // filter2D( mat, mat1, mat.depth(), ker );
}

bool halfImageBW( const QImage &s, QImage &d )
{
  if( s.format() != QImage::Format_Mono ) {
    cerr << "Error: bad image format" << endl;
    return false;
  }
  const unsigned w = s.width(), h = s.height();
  const unsigned w1 = ( w >> 1 ) & ~0x07,   h1 = h >> 1;
  QImage z( w1, h1, QImage::Format_Mono );
  bool invers = qRed( s.color( 0 )) != 0; // 0 may be white!

  for( unsigned dline=0; dline<h1; ++dline ) {
    uint8_t *d0 = z.scanLine( dline );
    const unsigned sline0 = dline  * 2;
    const unsigned sline1 = sline0 + 1;
    const uint8_t *s0 = s.scanLine( sline0 );
    const uint8_t *s1 = s.scanLine( sline1 );
    for( unsigned dcol = 0; dcol < w1/8; ++dcol ) {

      uint8_t c0 = *s0++, c1 = *s1++;
      uint8_t c = 0x00;

      if( invers ) {
        c0 = ~c0; c1 = ~c1;
      }
      uint8_t cx = ~( c0 & c1 );

      if( cx & 0xC0 )
        c |= 0x80;
      if( cx & 0x30 )
        c |= 0x40;
      if( cx & 0x0C )
        c |= 0x20;
      if( cx & 0x03 )
        c |= 0x10;

      c0 = *s0++; c1 = *s1++;
      if( invers ) {
        c0 = ~c0; c1 = ~c1;
      }
      cx = ~( c0 & c1 );

      if( cx & 0xC0 )
        c |= 0x08;
      if( cx & 0x30 )
        c |= 0x04;
      if( cx & 0x0C )
        c |= 0x02;
      if( cx & 0x03 )
        c |= 0x01;

      c = ~c;
      *d0++ = c;
    }
  }
  d = z;
  return true;
}


void ImgHand::showData()
{
  return;
  // if( vdp.empty() ) {
  //   QMessageBox::warning( this, tr("Missed data"),
  //      tr("There is no data to show. "
  //         "Use 'Analyze Dir' to acquire data." ));
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
  //     QTableWidgetItem *x = new QTableWidgetItem( QSN( cp.x ) );
  //     QTableWidgetItem *y = new QTableWidgetItem( QSN( cp.y ) );
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
  view->fitInView( 0, 0, img.width(), img.height(), Qt::KeepAspectRatio );
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
  QMessageBox::about( this, tr("About ImgHand"),
      tr("The <b>ImgHand</b> is a a test "
         "application to do something with images " ));
}


void ImgHand::createActions()
{

  //  -------------------------- File ------------------------------------
  openAct = new QAction( QIcon(":/icons/fileopen.png"), tr("&Open..."), this );
  openAct->setShortcut( tr("Ctrl+O") );
  openAct->setToolTip( tr("Open an existing image file (Ctrl+O)") );
  connect( openAct, &QAction::triggered, this, &ImgHand::open );

  saveAsAct = new QAction( QIcon(":/icons/filesaveas.png"), tr("&Save result"), this);
  saveAsAct->setShortcut( tr("Ctrl+S") );
  saveAsAct->setToolTip( tr("Save result image (Ctrl+S)") );
  connect( saveAsAct, &QAction::triggered, this, &ImgHand::saveAs );

  exitAct = new QAction( QIcon(":/icons/exit.png"),tr("E&xit"), this);
  exitAct->setShortcut( tr("Ctrl+Q") );
  exitAct->setToolTip( tr("Exit the application (Ctrl+Q)"));
  connect( exitAct, &QAction::triggered, this, &ImgHand::close );

  // ------------------------------ Image --------------------------------------

  showInfoAct = new QAction( QIcon(":/icons/info.png"), tr("&Image info"), this );
  showInfoAct->setShortcut( tr("Ctrl+I") );
  showInfoAct->setToolTip( tr("Image info (Ctrl+I)") );
  connect( showInfoAct, &QAction::triggered, this, &ImgHand::showInfo );


  restoreImageAct = new QAction( QSL("&Restore Image"), this );
  restoreImageAct->setShortcut( tr("Ctrl+Z") );
  restoreImageAct->setToolTip( tr("Resore image (Ctrl+Z)") );
  connect( restoreImageAct, &QAction::triggered, this, &ImgHand::restoreImage );

  makeBwAct = new QAction( QIcon(":/icons/makebw.png"), tr("make &B/W"), this );
  makeBwAct->setShortcut( tr("F5") );
  makeBwAct->setToolTip( tr("Make black/white image with level (F5)") );
  connect( makeBwAct, &QAction::triggered, this, &ImgHand::makeBwSlot );

  makeBwAdaAct = new QAction( QIcon(":/icons/makebw_a.png"),  tr("&Adaptive B/W"), this );
  makeBwAdaAct->setShortcut( tr("F6") );
  makeBwAdaAct->setToolTip( tr("Make black/white by adaptive algorithm (F6)") );
  connect( makeBwAdaAct, &QAction::triggered, this, &ImgHand::makeBwAdaSlot );

  boxCount0Act = new QAction( QIcon(":/icons/boxcount0.png"), tr("Box&Count 0"), this );
  boxCount0Act->setShortcut( tr("F9") );
  boxCount0Act->setToolTip( tr("Make boxcount analysis type 0 (F9)") );
  connect( boxCount0Act, &QAction::triggered, this, &ImgHand::boxCount0Slot );

  analyzeAct = new QAction( QIcon(":/icons/launch.png"), tr("&Analyze"), this );
  analyzeAct->setToolTip( tr("Analyze image (nothing for now)") );
  connect( analyzeAct, &QAction::triggered, this, &ImgHand::analyze );

  sobelAct = new QAction( QSL("&Sobel"), this );
  sobelAct->setToolTip( tr("Apply Sobel transformation") );
  connect( sobelAct, &QAction::triggered, this, &ImgHand::sobelSlot );

  aboutAct = new QAction( tr("&About"), this );
  aboutAct->setToolTip( tr("Show the application's About box") );
  connect( aboutAct, &QAction::triggered, this, &ImgHand::about );

  aboutQtAct = new QAction( tr("About &Qt"), this );
  aboutQtAct->setToolTip( tr("Show the Qt library's About box") );
  connect( aboutQtAct, &QAction::triggered, qApp, &QApplication::aboutQt );

  // TODO: tips, shortcuts, icons
  // ---------------------------- View ---------------------
  viewZoomInAct = new QAction( QIcon(":/icons/view_zoomin.png"), tr("Zoom &In"), this );
  connect( viewZoomInAct, &QAction::triggered, this, &ImgHand::viewZoomIn );
  viewZoomInAct->setShortcut( QKeySequence::ZoomIn );
  viewZoomInAct->setToolTip( tr("Zoom In image (Ctrl++)") );

  viewZoomOutAct = new QAction( QIcon(":/icons/view_zoomout.png"), tr("Zoom &Out"), this );
  connect( viewZoomOutAct, &QAction::triggered, this, &ImgHand::viewZoomOut );
  viewZoomOutAct->setShortcut( QKeySequence::ZoomOut );
  viewZoomOutAct->setToolTip( tr("Zoom Out image (Ctrl+-)") );

  viewZoomResetAct = new QAction( QIcon(":/icons/view_zoomreset.png"), tr("&1:1"), this );
  connect( viewZoomResetAct, &QAction::triggered, this, &ImgHand::viewZoomReset );
  viewZoomResetAct->setShortcut( tr("Ctrl+1") );
  viewZoomResetAct->setToolTip( tr("Set zoom to 1:1 (Ctrl+1)") );

  viewZoomFitAct = new QAction( QIcon(":/icons/view_zoomfit.png"), tr("&Fit"), this );
  connect( viewZoomFitAct, &QAction::triggered, this, &ImgHand::viewZoomFit );
  viewZoomFitAct->setShortcut( tr("Ctrl+0") );
  viewZoomFitAct->setToolTip( tr("Fit image to window (Ctrl+0)") );

  viewZoomSelAct = new QAction( QIcon(":/icons/view_zoomsel.png"), tr("&Selection"), this );
  connect( viewZoomSelAct, &QAction::triggered, this, &ImgHand::viewZoomSel );
  viewZoomSelAct->setShortcut( tr("Ctrl+5") );
  viewZoomSelAct->setToolTip( tr("Scale to selection (Ctrl+5)" ));

  viewSourceAct = new QAction( QIcon(":/icons/view_source.png"), tr("So&urce"), this );
  connect( viewSourceAct, &QAction::triggered, this, &ImgHand::viewSource );
  viewSourceAct->setShortcut( tr("Ctrl+2") );
  viewSourceAct->setToolTip( tr("View source image (Ctrl+2)") );

  viewResultAct = new QAction( QIcon(":/icons/view_result.png"), tr("&Result"), this );
  connect( viewResultAct, &QAction::triggered, this, &ImgHand::viewResult );
  viewResultAct->setShortcut( tr("Ctrl+3") );
  viewResultAct->setToolTip( tr("View Result Image (Ctrl+3)") );

}

void ImgHand::createMenus()
{
  fileMenu = menuBar()->addMenu( tr("&File") );
  fileMenu->addAction( openAct );
  fileMenu->addSeparator();
  fileMenu->addAction( saveAsAct );
  fileMenu->addSeparator();
  fileMenu->addAction( exitAct );

  imageMenu = menuBar()->addMenu( tr("&Image") );
  imageMenu->addAction( showInfoAct );
  imageMenu->addAction( restoreImageAct );
  imageMenu->addSeparator();
  imageMenu->addAction( makeBwAct );
  imageMenu->addAction( makeBwAdaAct );
  imageMenu->addAction( boxCount0Act );
  imageMenu->addSeparator();
  imageMenu->addAction( sobelAct );
  imageMenu->addSeparator();
  imageMenu->addAction( analyzeAct );

  viewMenu = menuBar()->addMenu( tr("&View") );
  viewMenu->addAction( viewZoomInAct );
  viewMenu->addAction( viewZoomOutAct );
  viewMenu->addAction( viewZoomResetAct );
  viewMenu->addAction( viewZoomFitAct );
  viewMenu->addAction( viewZoomSelAct );
  viewMenu->addSeparator();
  viewMenu->addAction( viewSourceAct );
  viewMenu->addAction( viewResultAct );

  menuBar()->addSeparator();

  helpMenu = menuBar()->addMenu( tr("&Help") );
  helpMenu->addAction( aboutAct );
  helpMenu->addAction( aboutQtAct );
}

void ImgHand::createToolBars()
{
  fileToolBar = addToolBar( tr("Main") );
  fileToolBar->addAction( openAct );
  fileToolBar->addSeparator();

  fileToolBar->addAction( saveAsAct );
  fileToolBar->addSeparator();

  fileToolBar->addAction( viewZoomInAct );
  fileToolBar->addAction( viewZoomOutAct );
  fileToolBar->addAction( viewZoomResetAct );
  fileToolBar->addAction( viewZoomFitAct );
  fileToolBar->addAction( viewZoomSelAct );
  fileToolBar->addSeparator();

  fileToolBar->addAction( viewSourceAct );
  fileToolBar->addAction( viewResultAct );
  fileToolBar->addSeparator();

  fileToolBar->addAction( showInfoAct );
  fileToolBar->addAction( makeBwAct );
  fileToolBar->addAction( makeBwAdaAct );
  fileToolBar->addAction( boxCount0Act );
  fileToolBar->addAction( analyzeAct );
  fileToolBar->addSeparator();

  fileToolBar->addAction( exitAct );
}

void ImgHand::createStatusBar()
{
  statBar = statusBar();
  statBar->addWidget( (stat_lbl =  new QLabel( QSL("Start")) ) );
  stat_lbl->setTextInteractionFlags( Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard );
  // statBar->addWidget( (labelSizeU =  new QLabel("(0x0).(0x0)")) );
  // statBar->showMessage(tr("Ready"));
  //statusBar()->
}

// void ImgHand::readSettings()
// {
//   QSettings settings( "atu", "ImgHand" );
//   QPoint pos = settings.value( "pos", QPoint(100, 100) ).toPoint();
//   QSize size = settings.value( "size", QSize(800, 600) ).toSize();
//   resize(size);
//   move(pos);
// }

// void ImgHand::writeSettings()
// {
//   QSettings settings( "atu", "ImgHand" );
//   settings.setValue( "pos", pos() );
//   settings.setValue( "size", size() );
// }


void ImgHand::setCurrentFile(const QString &fileName)
{
  curFile = fileName;
  setWindowModified(false);

  QString shownName;

  if( curFile.isEmpty() ) {
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
}

// --------------------- bits and image calculation functions

// works only if width is a multiple of 8
uint64_t count_bits( const QImage &img, bool count0 )
{
  if( img.format() != QImage::Format_Mono ) {
    return 0;
  }
  unsigned w = img.width(), h = img.height();
  unsigned wb = w / 8; // in bytes
  uint64_t nbits = 0, totalbits = (uint64_t) wb * 8 * h;

  auto t_st = chrono::high_resolution_clock::now();
  for( unsigned row = 0; row < h; ++row ) {
    const uint8_t *s = img.scanLine( row );
    for( unsigned i=0; i<wb; ++i ) {
      nbits += bit_tab[s[i]];
    }
  }
  auto t_end = chrono::high_resolution_clock::now();

  QRgb co0 = img.color( 0 ); // 0 may be white!
  if( qRed(co0) != 0 ) {
    count0 = !count0;
  }

  if( count0 ) {
    nbits = totalbits - nbits;
  }
  if( global_debug > 0 ) {
    cerr << "# count_bits bits: " << totalbits
         << " time: " << chrono::duration_cast<chrono::microseconds>( t_end - t_st ).count() << " us" << endl;
  }

  return nbits;
}

void img2mat( const QImage &img, Mat &m )
{
  auto h = img.height();
  auto w = img.width();
  auto bpl = img.bytesPerLine();
  Mat mat( h, w, CV_8UC1 );
  for( int i=0; i<h; ++i ) {
    memcpy( mat.ptr( i ), img.scanLine( i ), bpl );
  }
  m = mat;
  // namedWindow( "img2mat", WINDOW_AUTOSIZE );
  // imshow( "img2mat", m );
}


void mat2img( const Mat &m, QImage &img )
{
  // namedWindow( "mat2img", WINDOW_AUTOSIZE );
  // imshow( "mat2img", m );

  img = QImage( m.cols, m.rows, QImage::Format_Grayscale8 );
  auto bpl = img.bytesPerLine();
  for( int i=0; i<m.rows; ++i ) {
    memcpy( img.scanLine( i ), m.ptr( i ), bpl );
  }
}

void makeBWImage( const QImage &img, QImage &dst, int level )
{
  QImage i0 = img.copy();
  uint8_t lev = abs( level );
  bool invers = level < 0;
  uint8_t black_lev = 0, white_lev = 255;
  if( invers ) {
    black_lev = 255; white_lev = 0;
  }

  unsigned n_pix = img.width() * img.height(); // Format_Grayscale8 = 1 byte / pixel
  uint8_t *d = i0.bits();
  for( unsigned i=0; i<n_pix; ++i, ++d ) { // TODO: check scanlines
    *d = ( *d > lev ) ? white_lev : black_lev;
  }

  dst = i0.convertToFormat( QImage::Format_Mono,       Qt::ThresholdDither );
}

bool calcImgHisto( const QImage &img, ImgData &ida )
{
  if( img.format() != QImage::Format_Grayscale8 ) {
    cerr << "Error: bad image format, need Format_Grayscale8" << endl;
    return false;
  }

  ida.n_pix = img.width() * img.height(); // Format_Grayscale8 = 1 byte / pixel
  const uint8_t *d = img.bits();
  vector<int>  histo_0( 256, 0 );

  for( unsigned i=0; i<ida.n_pix; ++i, ++d ) { // count raw histogramm data
    ++histo_0[*d];
  }

  vector<double> histo_c( 256, 0.0 );
  double c = 0;
  ida.histo_05p = ida.histo_50p = ida.histo_95p = -1; // flag: not set
  ida.histo_max = 0;
  double v_max = -1;
  for( int i=0; i<256; ++i ) {
    double v   = (double)histo_0[i] / ida.n_pix;
    ida.histo_r[i] = v;
    c += v;
    histo_c[i] = c;
    if( ida.histo_05p < 0 && c >= 0.05 ) {
      ida.histo_05p = i;
    }
    if( ida.histo_50p < 0 && c >= 0.5  ) {
      ida.histo_50p = i;
    }
    if( ida.histo_95p < 0 && c >= 0.95  ) {
      ida.histo_95p = i;
    }
    if( v > v_max ) {
      v_max = v; ida.histo_max = i;
    }
  }

  ida.histo_auto = ida.histo_50p;
  if( ( ida.histo_95p - ida.histo_50p ) < 5 || ( ida.histo_50p - ida.histo_05p ) < 5 ) {
    if( ( ida.histo_95p - ida.histo_05p ) > 4 ) {
      ida.histo_auto = ( ida.histo_95p + ida.histo_05p ) / 2;
    } else {
      ida.histo_auto = 127;
    }
  }
  return true;
}

bool boxCount0( const QImage &imgx, ImgData &ida )
{
  if( imgx.width() < 32 || imgx.height() < 32 || imgx.format() != QImage::Format_Mono ) {
    cerr << "Error: boxCount0: bad input image" << endl;
    return false;
  }

  uint64_t cnbp;
  unsigned box_sz = 1, box_scale = 1;

  double lnr, lnN;
  ida.v_lnr.clear(); ida.v_lnN.clear();

  QImage xi = imgx.copy();
  unsigned pic_w = xi.width(), pic_h = xi.height();

  cout << endl;
  while( pic_w >= 32 ) {
    pic_w = xi.width(); pic_h = xi.height();

    if( global_debug > 5 ) {
      QString cfn = QString("tmp_%1.png").arg( pic_w, 6, 10, QChar('0') );
      xi.save( cfn, "PNG" );
    }

    cnbp = count_bits( xi, true );
    double b_r = (double)cnbp / ( (double) pic_w * pic_h ) ;

    lnr = log2( (double)(box_sz) );
    lnN = (cnbp>0) ? log2( (double)(cnbp) ) : 0;
    ida.v_lnr.push_back( lnr ); ida.v_lnN.push_back( lnN );
    if( global_debug > 0 ) {
      cout << "# " << lnr << ' ' << lnN << ' ' << pic_w << ' ' << pic_h
           << ' ' << cnbp << ' ' << box_scale << ' ' << b_r << endl;
    }
    // cout << "# size: " << pic_w << " x " << pic_h << " black: " << cnbp << " BpL: " << xi.bytesPerLine()
    //      << " box_scale: " << box_scale << " TB: " << (cnbp*box_scale)
    //      << " ln_r: " << lnr << " lnN: " << lnN << " b_r:" << b_r << endl;

    QImage zi;
    halfImageBW( xi, zi );
    box_sz <<= 1 ; box_scale <<= 2;

    xi = zi.copy();
  }


  double corr = gsl_stats_correlation( ida.v_lnr.data(), 1, ida.v_lnN.data(), 1, ida.v_lnr.size() );
  double c0, c1, cov00, cov01, cov11, sumq;
  gsl_fit_linear(  ida.v_lnr.data(), 1, ida.v_lnN.data(), 1, ida.v_lnr.size(),
                   &c0, &c1, &cov00, &cov01, &cov11, &sumq );
  ida.d1 = -c1;
  ida.c0 = c0;
  ida.corr = corr;

  return true;
}
