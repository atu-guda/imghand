#include "imghand.h"

using namespace std;

void GenerData::calc_eff( const FormInfo &f )
{
  eff_scale = scale * f.scale;
}

GenerDialog::GenerDialog( GenerData &sd, const QVector<FormInfo> &a_forms, QWidget *parent )
  : QDialog( parent ), d( sd ), forms( a_forms )
{
  setupUi();
}

QLineEdit* GenerDialog::addSomeEdit( const QString &lbl_str )
{
  auto lbl = new QLabel( lbl_str, this );
  gridLayout->addWidget( lbl, c_row[c_col], c_col*2, 1, 1 );
  auto le = new QLineEdit( this );
  gridLayout->addWidget( le, c_row[c_col], c_col*2+1, 1, 1 );
  lbl->setBuddy( le );
  goNextCell();
  return le;
}


QLineEdit* GenerDialog::addIntEdit( const QString &lbl_str, int vmin, int vmax )
{
  auto le = addSomeEdit( lbl_str );
  le->setValidator( new QIntValidator( vmin, vmax, this ) );
  return le;
}

QLineEdit* GenerDialog::addDoubleEdit( const QString &lbl_str, double vmin, double vmax, int digs )
{
  auto le = addSomeEdit( lbl_str );
  le->setValidator( new QDoubleValidator( vmin, vmax, digs, this ) );
  return le;
}

QComboBox* GenerDialog::addComboBox( const QString &lbl_str, const QStringList &items )
{
  auto lbl = new QLabel( lbl_str, this );
  gridLayout->addWidget( lbl, c_row[c_col], 2*c_col, 1, 1 );
  auto cb = new QComboBox( this );
  cb->addItems( items );
  gridLayout->addWidget( cb, c_row[c_col], 2*c_col+1, 1, 1 );
  lbl->setBuddy( cb );
  goNextCell();
  return cb;
}

void GenerDialog::setupUi()
{
  setObjectName( QSL("GenerDialog") );
  setWindowTitle( QSL( "Parameters for image generation" ) );

  c_col = 0;
  for( auto &x : c_row ) { x = 0; }

  verticalLayout = new QVBoxLayout( this );
  verticalLayout->setObjectName( QSL("verticalLayout") );
  gridLayout = new QGridLayout();
  gridLayout->setObjectName( QSL("gridLayout") );

  w_le    = addIntEdit( QSL("&Width"), 128, 20480 );
  h_le    = addIntEdit( QSL("&Height"), 128, 20480 );
  iter_le = addIntEdit( QSL("&Iterations"), 1, 32 );

  QStringList tp_names;
  for( const auto& f : forms ) {
    tp_names += f.name;
  }
  type_cb = addComboBox( QSL("&Type"), tp_names );

  size0_le  = addDoubleEdit( QSL("Initial &size"), 0.001, 3.0, 3 );
  scale_le  = addDoubleEdit( QSL("Scale mult"),    0.001, 3.0, 3 );

  c_col = 1;

  ss_le    = addDoubleEdit( QSL("s&hift scale"),    0.001,   10.0, 3 );
  as_le    = addDoubleEdit( QSL("Angle scale"),     -10.0,   10.0, 3 );
  aa_le    = addDoubleEdit( QSL("Angle add"),      -180.0,  180.0, 3 );
  a_le     = addDoubleEdit( QSL("a"),             -1000.0, 1000.0, 3 );
  b_le     = addDoubleEdit( QSL("b"),             -1000.0, 1000.0, 3 );
  c_le     = addDoubleEdit( QSL("c"),             -1000.0, 1000.0, 3 );
  d_le     = addDoubleEdit( QSL("d"),             -1000.0, 1000.0, 3 );


  verticalLayout->addLayout( gridLayout );

  buttonBox = new QDialogButtonBox( this );
  buttonBox->setObjectName( QSL("buttonBox") );
  buttonBox->setOrientation( Qt::Horizontal );
  buttonBox->setStandardButtons( QDialogButtonBox::Cancel | QDialogButtonBox::Ok );

  btnRevert  = buttonBox->addButton( QSL("Revert"),   QDialogButtonBox::ActionRole );
  btnDefault = buttonBox->addButton( QSL("Default"),  QDialogButtonBox::ResetRole  );

  verticalLayout->addWidget( buttonBox );

  revert();

  QObject::connect( buttonBox,  &QDialogButtonBox::accepted, this, &GenerDialog::accept );
  QObject::connect( buttonBox,  &QDialogButtonBox::rejected, this, &GenerDialog::reject );
  QObject::connect( btnRevert,  &QPushButton::pressed,       this, &GenerDialog::revert );
  QObject::connect( btnDefault, &QPushButton::pressed,       this, &GenerDialog::setDefault );

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


void GenerDialog::revert()
{
  w_le->setText( QSN( d.w ) );
  h_le->setText( QSN( d.h ) );
  iter_le->setText( QSN( d.iter ) );
  type_cb->setCurrentIndex( d.type );
  size0_le->setText( QSN( d.size0 ) );
  scale_le->setText( QSN( d.scale ) );
  ss_le->setText( QSN( d.ss ) );
  as_le->setText( QSN( d.as ) );
  aa_le->setText( QSN( d.aa ) );
  a_le->setText( QSN( d.a ) );
  b_le->setText( QSN( d.b ) );
  c_le->setText( QSN( d.c ) );
  d_le->setText( QSN( d.d ) );
}

void GenerDialog::setDefault()
{
  auto s = d;
  GenerData defdata;
  d = defdata;
  revert();
  d = s; // to allow revert
}

