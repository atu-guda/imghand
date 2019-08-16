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
  unsigned w = 1024, h = 1024, iter = 7;
  unsigned type = 0; // TODO: enum - list
  double size0 = 200, scale = 0.5, ss = 1.0, as = 1.0, aa = 0.0;
  // ss = shift_scale, as = alpha_scale, aa = alpha_add
  double a = 1, b = 1, c = 1, d = 1;
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

   QLabel   *w_lbl, *h_lbl, *iter_lbl;
   QLineEdit *w_le,  *h_le,  *iter_le;

   QLabel *type_lbl;
   QComboBox *type_cb;

   QLabel   *size0_lbl, *scale_lbl;
   QLineEdit *size0_le, *scale_le;

   QLabel   *ss_lbl, *as_lbl, *aa_lbl;
   QLineEdit *ss_le, *as_le, *aa_le;
   QLabel    *a_lbl, *b_lbl, *c_lbl, *d_lbl;
   QLineEdit *a_le,   *b_le,  *c_le, *d_le;

   QDialogButtonBox *buttonBox;
};

#endif
