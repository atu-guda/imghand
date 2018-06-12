#ifndef _SOBELDIALOG_H
#define _SOBELDIALOG_H

#include <QDialog>
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

struct SobelData {
  int dx = 1, dy = 1,
      ksize =  3;
  double scale = 1.0, delta = 0.0;
  bool rescale = false;
};


class SobelDialog : public QDialog {
  Q_OBJECT;
  public:
   SobelDialog( SobelData &sd, QWidget *parent );
   void setupUi();
   virtual void accept() override;
  private:
   SobelData &d;
   QVBoxLayout *verticalLayout;
   QGridLayout *gridLayout;
   QLabel *dx_label;
   QComboBox *dx_cb;
   QLabel *dy_label;
   QComboBox *dy_cb;
   QLabel *ksize_label;
   QComboBox *ksize_cb;
   QLabel *scale_label;
   QLineEdit *scale_le;
   QLabel *delta_label;
   QLineEdit *delta_le;
   QLabel *label;
   QCheckBox *rescale_cb;
   QDialogButtonBox *buttonBox;
};

#endif
