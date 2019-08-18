#include "imghand_base.h"
#include "generdialog.h"

using namespace std;

GenerDialog::GenerDialog( GenerData &sd, QWidget *parent )
  : QDialog( parent ), d( sd )
{
  setupUi();
}

void GenerDialog::setupUi()
{
  setObjectName( QSL("GenerDialog") );
  setWindowTitle( QSL( "Generate image parameters" ) );

  verticalLayout = new QVBoxLayout( this );
  verticalLayout->setObjectName( QSL("verticalLayout") );
  gridLayout = new QGridLayout();
  gridLayout->setObjectName( QSL("gridLayout") );

  w_lbl = new QLabel( QSL("&Width"), this );
  w_lbl->setObjectName( QSL("w_lbl") );
  gridLayout->addWidget( w_lbl, 0, 0, 1, 1 );
  w_le = new QLineEdit( this );
  w_le->setObjectName( QSL("w_le") );
  w_le->setText( QSN( d.w ) );
  auto *w_v = new QIntValidator( 128, 10240, this );
  w_le->setValidator( w_v );
  gridLayout->addWidget( w_le, 0, 1, 1, 1 );
  w_lbl->setBuddy( w_le );

  h_lbl = new QLabel( QSL("&Height"), this );
  h_lbl->setObjectName( QSL("h_lbl") );
  gridLayout->addWidget( h_lbl, 1, 0, 1, 1 );
  h_le = new QLineEdit( this );
  h_le->setObjectName( QSL("h_le") );
  h_le->setText( QSN( d.h ) );
  auto *h_v = new QIntValidator( 128, 10240, this );
  h_le->setValidator( h_v );
  gridLayout->addWidget( h_le, 1, 1, 1, 1 );
  h_lbl->setBuddy( h_le );

  iter_lbl = new QLabel( QSL("&Iterations"), this );
  iter_lbl->setObjectName( QSL("iter_lbl") );
  gridLayout->addWidget( iter_lbl, 2, 0, 1, 1 );
  iter_le = new QLineEdit( this );
  iter_le->setObjectName( QSL("iter_le") );
  iter_le->setText( QSN( d.iter ) );
  auto *iter_v = new QIntValidator( 1, 32, this );
  iter_le->setValidator( iter_v );
  gridLayout->addWidget( iter_le, 2, 1, 1, 1 );
  iter_lbl->setBuddy( iter_le );

  type_lbl = new QLabel( QSL("&Type"), this );
  type_lbl->setObjectName( QSL("type_lbl") );
  gridLayout->addWidget( type_lbl, 3, 0, 1, 1 );
  type_cb = new QComboBox( this );
  type_cb->addItem( QSL("Star") );
  type_cb->addItem( QSL("Box") );
  type_cb->addItem( QSL("Triangle") );
  type_cb->addItem( QSL("X3") );
  type_cb->setObjectName( QSL("type_cb") );
  gridLayout->addWidget( type_cb, 3, 1, 1, 1 );
  type_lbl->setBuddy( type_cb );


  size0_lbl = new QLabel( QSL("Initial &size"), this );
  size0_lbl->setObjectName( QSL("size0_lbl") );
  gridLayout->addWidget( size0_lbl, 4, 0, 1, 1 );
  size0_le = new QLineEdit( this );
  size0_le->setObjectName( QSL("size0_le") );
  size0_le->setText( QSN( d.size0 ) );
  auto *size0_v = new QDoubleValidator( 1, 10000, 3, this );
  size0_le->setValidator( size0_v );
  gridLayout->addWidget( size0_le, 4, 1, 1, 1 );
  size0_lbl->setBuddy( size0_le );

  scale_lbl = new QLabel( QSL("Scale mult"), this );
  scale_lbl->setObjectName( QSL("scale_lbl") );
  gridLayout->addWidget( scale_lbl, 5, 0, 1, 1 );
  scale_le = new QLineEdit( this );
  scale_le->setObjectName( QSL("scale_le") );
  scale_le->setText( QSN( d.scale ) );
  auto *scale_v = new QDoubleValidator( 0.001, 1.0, 3, this );
  scale_le->setValidator( scale_v );
  gridLayout->addWidget( scale_le, 5, 1, 1, 1 );
  scale_lbl->setBuddy( scale_le );

  ss_lbl = new QLabel( QSL("s&hift scale"), this );
  ss_lbl->setObjectName( QSL("ss_lbl") );
  gridLayout->addWidget( ss_lbl, 0, 2, 1, 1 );
  ss_le = new QLineEdit( this );
  ss_le->setObjectName( QSL("ss_le") );
  ss_le->setText( QSN( d.ss ) );
  auto *ss_v = new QDoubleValidator( 0.001, 10.0, 3, this );
  ss_le->setValidator( ss_v );
  gridLayout->addWidget( ss_le, 0, 3, 1, 1 );
  ss_lbl->setBuddy( ss_le );

  as_lbl = new QLabel( QSL("Angle scale"), this );
  as_lbl->setObjectName( QSL("as_lbl") );
  gridLayout->addWidget( as_lbl, 1, 2, 1, 1 );
  as_le = new QLineEdit( this );
  as_le->setObjectName( QSL("as_le") );
  as_le->setText( QSN( d.as ) );
  auto *as_v = new QDoubleValidator( 0.001, 10.0, 3, this );
  as_le->setValidator( as_v );
  gridLayout->addWidget( as_le, 1, 3, 1, 1 );
  as_lbl->setBuddy( as_le );

  aa_lbl = new QLabel( QSL("Angle add"), this );
  aa_lbl->setObjectName( QSL("aa_lbl") );
  gridLayout->addWidget( aa_lbl, 2, 2, 1, 1 );
  aa_le = new QLineEdit( this );
  aa_le->setObjectName( QSL("aa_le") );
  aa_le->setText( QSN( d.aa ) );
  auto *aa_v = new QDoubleValidator( -180, 180, 3, this );
  aa_le->setValidator( aa_v );
  gridLayout->addWidget( aa_le, 2, 3, 1, 1 );
  aa_lbl->setBuddy( aa_le );

  a_lbl = new QLabel( QSL("a"), this );
  a_lbl->setObjectName( QSL("a_lbl") );
  gridLayout->addWidget( a_lbl, 3, 2, 1, 1 );
  a_le = new QLineEdit( this );
  a_le->setObjectName( QSL("a_le") );
  a_le->setText( QSN( d.a ) );
  auto *a_v = new QDoubleValidator( -1000, 1000.0, 3, this );
  a_le->setValidator( a_v );
  gridLayout->addWidget( a_le, 3, 3, 1, 1 );
  a_lbl->setBuddy( a_le );

  b_lbl = new QLabel( QSL("b"), this );
  b_lbl->setObjectName( QSL("b_lbl") );
  gridLayout->addWidget( b_lbl, 4, 2, 1, 1 );
  b_le = new QLineEdit( this );
  b_le->setObjectName( QSL("b_le") );
  b_le->setText( QSN( d.b ) );
  auto *b_v = new QDoubleValidator( -1000, 1000.0, 3, this );
  b_le->setValidator( b_v );
  gridLayout->addWidget( b_le, 4, 3, 1, 1 );
  b_lbl->setBuddy( b_le );

  c_lbl = new QLabel( QSL("c"), this );
  c_lbl->setObjectName( QSL("c_lbl") );
  gridLayout->addWidget( c_lbl, 5, 2, 1, 1 );
  c_le = new QLineEdit( this );
  c_le->setObjectName( QSL("c_le") );
  c_le->setText( QSN( d.c ) );
  auto *c_v = new QDoubleValidator( -1000, 1000.0, 3, this );
  c_le->setValidator( c_v );
  gridLayout->addWidget( c_le, 5, 3, 1, 1 );
  c_lbl->setBuddy( c_le );

  d_lbl = new QLabel( QSL("d"), this );
  d_lbl->setObjectName( QSL("d_lbl") );
  gridLayout->addWidget( d_lbl, 6, 2, 1, 1 );
  d_le = new QLineEdit( this );
  d_le->setObjectName( QSL("d_le") );
  d_le->setText( QSN( d.d ) );
  auto *d_v = new QDoubleValidator( -1000, 1000.0, 3, this );
  d_le->setValidator( d_v );
  gridLayout->addWidget( d_le, 6, 3, 1, 1 );
  d_lbl->setBuddy( d_le );


  verticalLayout->addLayout( gridLayout );

  buttonBox = new QDialogButtonBox( this );
  buttonBox->setObjectName( QSL("buttonBox") );
  buttonBox->setOrientation( Qt::Horizontal );
  buttonBox->setStandardButtons( QDialogButtonBox::Cancel | QDialogButtonBox::Ok );

  verticalLayout->addWidget( buttonBox );

  QObject::connect( buttonBox,  &QDialogButtonBox::accepted, this, &GenerDialog::accept );
  QObject::connect( buttonBox,  &QDialogButtonBox::rejected, this, &GenerDialog::reject );

}

void GenerDialog::accept()
{
  d.w = ( w_le->text().toInt() + 15 ) & 0x0000FFF0;
  d.h = ( h_le->text().toInt() + 15 ) & 0x0000FFF0;
  d.iter = iter_le->text().toInt();
  d.type = type_cb->currentIndex();
  d.size0 = clamp( size0_le->text().toDouble(),     0.001,    2.0 );
  d.scale = clamp( scale_le->text().toDouble(),     0.001,   10.0 );
  d.ss    = clamp(    ss_le->text().toDouble(),     0.001,   10.0 );
  d.as    = clamp(    as_le->text().toDouble(),     0.001,   10.0 );
  d.aa    = clamp(    aa_le->text().toDouble(),  -180.0,    180.0 );
  d.a     = clamp(     a_le->text().toDouble(), -1000.0,   1000.0 );
  d.b     = clamp(     b_le->text().toDouble(), -1000.0,   1000.0 );
  d.c     = clamp(     c_le->text().toDouble(), -1000.0,   1000.0 );
  d.d     = clamp(     d_le->text().toDouble(), -1000.0,   1000.0 );
  QDialog::accept();
}

