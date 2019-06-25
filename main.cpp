/***************************************************************************
 *   Copyright (C) 2018 by Guda Anton   *
 *   atu@nmetau.edu.ua   *
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

#include "imghand.h"

using namespace std;

int global_debug = 0;
int batch_proc = 0;

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

  QStringList pargs = prs.positionalArguments();

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
    if( pargs.size() < 1 ) {
      cerr << "Error: need filename for batch process" << endl;
      return 1;
    }
  }
  return rc;
}

