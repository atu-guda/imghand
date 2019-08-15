#include "imghand_base.h"
#include "sobeldialog.h"

SobelDialog::SobelDialog( SobelData &sd, QWidget *parent )
  : QDialog( parent ), d( sd )
{
  setupUi();
}

void SobelDialog::setupUi()
{
  setObjectName( QSL("SobelDialog") );
  setWindowTitle( QSL( "Sobel filter parameters" ) );
  // resize(279, 261);
  verticalLayout = new QVBoxLayout( this );
  verticalLayout->setObjectName( QSL("verticalLayout") );
  gridLayout = new QGridLayout();
  gridLayout->setObjectName( QSL("gridLayout") );

  dx_label = new QLabel( QSL("dx"), this );
  dx_label->setObjectName( QSL("dx_label") );
  gridLayout->addWidget( dx_label, 0, 0, 1, 1 );

  dx_cb = new QComboBox( this );
  dx_cb->addItem( QSL("0") );
  dx_cb->addItem( QSL("1") );
  dx_cb->addItem( QSL("2") );
  dx_cb->addItem( QSL("3") );
  dx_cb->setObjectName( QSL("dx_cb") );
  gridLayout->addWidget( dx_cb, 0, 1, 1, 1 );

  dy_label = new QLabel( QSL("dy"), this );
  dy_label->setObjectName( QSL("dy_label") );
  gridLayout->addWidget( dy_label, 1, 0, 1, 1 );

  dy_cb = new QComboBox( this );
  dy_cb->addItem( QSL("0") );
  dy_cb->addItem( QSL("1") );
  dy_cb->addItem( QSL("2") );
  dy_cb->addItem( QSL("3") );
  dy_cb->setObjectName( QSL("dy_cb") );
  gridLayout->addWidget( dy_cb, 1, 1, 1, 1 );

  ksize_label = new QLabel( QSL("ksize"), this );
  ksize_label->setObjectName( QSL("ksize_label") );
  gridLayout->addWidget( ksize_label, 2, 0, 1, 1 );

  ksize_cb = new QComboBox( this );
  ksize_cb->addItem( QSL("3")  );
  ksize_cb->addItem( QSL("5")  );
  ksize_cb->addItem( QSL("7")  );
  ksize_cb->addItem( QSL("1")  );
  ksize_cb->addItem( QSL("-1") );
  ksize_cb->setObjectName( QSL("ksize_cb") );
  gridLayout->addWidget( ksize_cb, 2, 1, 1, 1 );

  scale_label = new QLabel( QSL("scale"), this );
  scale_label->setObjectName( QSL("scale_label") );
  gridLayout->addWidget( scale_label, 3, 0, 1, 1 );

  scale_le = new QLineEdit( this );
  scale_le->setObjectName( QSL("scale_le") );
  gridLayout->addWidget( scale_le, 3, 1, 1, 1 );

  delta_label = new QLabel( QSL("delta"), this );
  delta_label->setObjectName( QSL("delta_label") );
  gridLayout->addWidget( delta_label, 4, 0, 1, 1 );

  delta_le = new QLineEdit( this );
  delta_le->setObjectName( QSL("delta_le") );

  gridLayout->addWidget( delta_le, 4, 1, 1, 1 );

  label = new QLabel( this );
  label->setObjectName( QSL("label") );
  gridLayout->addWidget( label, 5, 0, 1, 1 );

  rescale_cb = new QCheckBox( this );
  rescale_cb->setObjectName( QSL("rescale_cb") );
  gridLayout->addWidget( rescale_cb, 5, 1, 1, 1 );

  verticalLayout->addLayout( gridLayout );

  buttonBox = new QDialogButtonBox( this );
  buttonBox->setObjectName( QSL("buttonBox") );
  buttonBox->setOrientation( Qt::Horizontal );
  buttonBox->setStandardButtons( QDialogButtonBox::Cancel | QDialogButtonBox::Ok );

  verticalLayout->addWidget( buttonBox );

  dx_label->setBuddy( dx_cb );
  dy_label->setBuddy( dy_cb );
  ksize_label->setBuddy( ksize_cb );
  scale_label->setBuddy( scale_le );
  delta_label->setBuddy( delta_le );

  btnRevert  = buttonBox->addButton( QSL("Revert"),   QDialogButtonBox::ActionRole );
  btnDefault = buttonBox->addButton( QSL("Default"),  QDialogButtonBox::ResetRole  );

  revert();

  QObject::connect( buttonBox,  &QDialogButtonBox::accepted, this, &SobelDialog::accept );
  QObject::connect( buttonBox,  &QDialogButtonBox::rejected, this, &SobelDialog::reject );
  QObject::connect( btnRevert,  &QPushButton::pressed,       this, &SobelDialog::revert );
  QObject::connect( btnDefault, &QPushButton::pressed,       this, &SobelDialog::setDefault );

  // QMetaObject::connectSlotsByName(this);
}

void SobelDialog::revert()
{
  if( !dx_cb ) {
    return;
  }
  dx_cb->setCurrentText( QString::number( d.dx ) );
  dy_cb->setCurrentText( QString::number( d.dy ) );
  ksize_cb->setCurrentText( QString::number( d.ksize ) );
  scale_le->setText( QString::number( d.scale ) );
  delta_le->setText( QString::number( d.delta ) );
  rescale_cb->setChecked( d.rescale );
}

void SobelDialog::setDefault()
{
  auto s = d;
  SobelData defdata;
  d = defdata;
  revert();
  d = s;
}

void SobelDialog::accept()
{
  d.dx = dx_cb->currentText().toInt();
  d.dy = dy_cb->currentText().toInt();
  d.ksize = ksize_cb->currentText().toInt();
  d.scale = scale_le->text().toDouble();
  d.delta = delta_le->text().toDouble();
  d.rescale = rescale_cb->isChecked();
  QDialog::accept();
}

