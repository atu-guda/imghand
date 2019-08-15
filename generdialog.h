#ifndef _GENERDIALOG_H
#define _GENERDIALOG_H

#include <QDialog>
#include <QApplication>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

struct GenerData {
  unsigned w = 1024, h = 768, iter = 5;
  unsigned type = 0; // TODO: enum - list
  double size0 = 100, scale = 0.5;
};


class GenerDialog : public QDialog {
  Q_OBJECT;
  public:
   GenerDialog( GenerData &sd, QWidget *parent );
   void setupUi();
   virtual void accept() override;
  private:
   GenerData &d;
   QVBoxLayout *verticalLayout;
   QGridLayout *gridLayout;
   QLabel *w_lbl;
   QLineEdit *w_le;
   QLabel *h_lbl;
   QLineEdit *h_le;
   QLabel *iter_lbl;
   QLineEdit *iter_le;

   QLabel *type_lbl;
   QComboBox *type_cb;

   QLabel *size0_lbl;
   QLineEdit *size0_le;
   QLabel *scale_lbl;
   QLineEdit *scale_le;

   QDialogButtonBox *buttonBox;
};

#endif
