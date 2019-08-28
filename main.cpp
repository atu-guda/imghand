/***************************************************************************
 *   Copyright (C) 2018-2019 by Guda Anton, GRoza                          *
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

#include <iostream>

#include <QApplication>
#include <QtDebug>
#include <QCommandLineParser>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "imghand.h"

using namespace std;

using namespace cv;

int global_debug = 0;
int batch_proc = 0;

int batch_act( QCommandLineParser &prs );

int main( int argc, char *argv[] )
{
  setenv( "LC_NUMERIC", "C", 1 );
  Q_INIT_RESOURCE( imghand );
  QApplication app( argc, argv );

  app.setApplicationName( QSL("imghand") );
  app.setApplicationDisplayName( QSL("imghand") );
  app.setApplicationVersion( QSL("0.2") );

  QCommandLineParser prs;
  prs.setApplicationDescription( QSL( "imghand - fractal estimation application" ) );
  prs.addOptions( {
      { QSL("b"), QSL("Batch file procession") },
      { QSL("d"), QSL("debug level"), QSL("N") },
      { QSL("a"), QSL("use adaptive binarization") },
      { QSL("w"), QSL("white level (0=auto)"), QSL("level") },
      { QSL("r"), QSL("Adaptive binarization radius"), QSL("pixels") },
      { QSL("o"), QSL("output image filename"), QSL("file") },
  } );
  prs.addHelpOption();
  prs.addVersionOption();

  prs.process( app );

  global_debug = prs.value( QSL("d") ).toInt();
  if( global_debug > 0 ) {
    qWarning() << "# debug level:" << global_debug;
  }
  if( prs.isSet( QSL("b") ) ) {
    batch_proc = true;
  }

  const QStringList pargs = prs.positionalArguments();

  int rc = 0;

  if( ! batch_proc ) {
    auto *mw = new ImgHand();
    mw->showMaximized();
    mw->show();

    for( auto fn : pargs ) {
      if( global_debug > 0 ) {
        cerr << "Try to open file \"" << qPrintable( fn ) << '"' << endl;
      }
      mw->loadFile( fn );
      break; // TODO: more files?
    }

    rc = app.exec();
  } else { // batch processing
    rc = batch_act( prs );
  }
  return rc;
}

int batch_act( QCommandLineParser &prs )
{
  const QStringList pargs = prs.positionalArguments();
  if( pargs.size() < 1 ) {
    cerr << "Error: need filename for batch process" << endl;
    return 1;
  }
  QString fn = pargs[0];

  int bw_level = 0;
  if( prs.isSet( QSL("w") ) ) {
    bw_level = prs.value( QSL("w") ).toInt();
  }
  int r_adapt = 101;
  if( prs.isSet( QSL("r") ) ) {
    r_adapt = prs.value( QSL("r") ).toInt();
  }
  bool use_adapt = prs.isSet( QSL("a") );
  QString out_fn = prs.value( QSL("o") );

  if( global_debug > 0 ) {
    cerr << "Batch: in_file: \"" << qPrintable( fn ) << "\" adapt: " << use_adapt
         << " bw_level: " << bw_level << " r= " << r_adapt
         << " out_file: \"" << qPrintable( out_fn ) << "\"" << endl;
  }


  QImage img;
  {
    QImage img0;
    if ( ! img0.load( fn ) ) {
      cerr << "Error: fail to load file \"" << qPrintable( fn ) << "\" : " << strerror( errno ) << endl;
      return 2;
    }
    img = img0.convertToFormat( QImage::Format_Grayscale8, Qt::ThresholdDither );
  }
  ImgData ida;
  unsigned i_w = img.width(), i_h = img.height();
  ida.n_pix = i_w * i_h;

  if( global_debug > 0 ) {
    cerr << "Image \"" << qPrintable( fn ) << "\" " << i_w << 'x' << i_h << endl;
  }

  QImage imgx;

  if( use_adapt ) {
    Mat mi, mo;
    img2mat( img, mi );

    //                 src dst max   method                      type           size     C
    adaptiveThreshold( mi, mo, 255, ADAPTIVE_THRESH_GAUSSIAN_C,  THRESH_BINARY, r_adapt, 1 );
    mat2img( mo, img );

    calcImgHisto( img, ida );
    makeBWImage( img, imgx, ida.histo_auto );

    ida.u_level = 0; // flag for output
  } else { // global B/W

    if( bw_level == 0 ) { // use histogramm to find level
      calcImgHisto( img, ida );
      bw_level = ida.histo_auto;
    }

    makeBWImage( img, imgx, bw_level );
    ida.u_level = bw_level;
    r_adapt = 0;
  }

  if( ! out_fn.isEmpty() ) {
    imgx.save( out_fn );
  }

  if( !boxCount0( imgx, ida ) ) {
    return 4;
  }

  cout << "\"" << qPrintable( fn )
       << "\" " << ida.u_level << " " << r_adapt << ' ' << ida.d1
       << ' ' << ida.c0 << ' ' << ida.corr << endl;

  return 0;
}

