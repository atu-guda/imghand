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
  type_cb->addItem( QSL("Box") );
  type_cb->addItem( QSL("X1") );
  type_cb->addItem( QSL("X2") );
  type_cb->addItem( QSL("X3") );
  type_cb->setObjectName( QSL("type_cb") );
  gridLayout->addWidget( type_cb, 3, 1, 1, 2 );
  type_lbl->setBuddy( type_cb );


  size0_lbl = new QLabel( QSL("Element &size"), this );
  size0_lbl->setObjectName( QSL("size0_lbl") );
  gridLayout->addWidget( size0_lbl, 4, 0, 1, 1 );
  size0_le = new QLineEdit( this );
  size0_le->setObjectName( QSL("size0_le") );
  size0_le->setText( QSN( d.size0 ) );
  auto *size0_v = new QDoubleValidator( 1, 10000, 3, this );
  size0_le->setValidator( size0_v );
  gridLayout->addWidget( size0_le, 4, 1, 1, 1 );
  size0_lbl->setBuddy( size0_le );

  scale_lbl = new QLabel( QSL("&Scale"), this );
  scale_lbl->setObjectName( QSL("scale_lbl") );
  gridLayout->addWidget( scale_lbl, 5, 0, 1, 1 );
  scale_le = new QLineEdit( this );
  scale_le->setObjectName( QSL("scale_le") );
  scale_le->setText( QSN( d.scale ) );
  auto *scale_v = new QDoubleValidator( 0.001, 1.0, 3, this );
  scale_le->setValidator( scale_v );
  gridLayout->addWidget( scale_le, 5, 1, 1, 1 );
  scale_lbl->setBuddy( scale_le );


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
  d.type = type_cb->currentText().toInt();
  d.size0 = clamp( size0_le->text().toDouble(), 1.0, 10000.0 );
  d.scale = clamp( scale_le->text().toDouble(), 0.001, 1.0 );
  QDialog::accept();
}

