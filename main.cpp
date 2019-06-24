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


#include <QApplication>
#include <QtDebug>
#include <QCommandLineParser>

#include "imghand.h"

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

  auto *mw = new ImgHand();
  mw->show();

  for( auto fn : pargs ) {
    if( global_debug > 0 ) {
      qDebug() << "Try to open file" << fn;
    }
    mw->loadFile( fn );
    break;
  }

  return app.exec();
}

