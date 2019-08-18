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
#include <QCheckBox>
#include <QVBoxLayout>

struct FormInfo; // from imghand.h

struct GenerData {
  unsigned w = 1024, h = 1024, iter = 7;
  unsigned type = 0; // TODO: enum - list
  double size0 = 1, scale = 1;
  int ovr_scale = 0; // false
  double ss = 1.0, as = 1.0, aa = 0.0;
  // ss = shift_scale, as = alpha_scale, aa = alpha_add
  double a = 1, b = 1, c = 1, d = 1;
  // effective values
  double eff_scale = 1.0;
  void calc_eff( const FormInfo &f );
};


class GenerDialog : public QDialog {
  Q_OBJECT;
  public:
   GenerDialog( GenerData &sd, const QVector<FormInfo> &a_forms, QWidget *parent );
   void setupUi();
   virtual void accept() override;
  public slots:
   void revert();
   void setDefault();
  private:
   void goNextCell() { ++c_row[c_col];};
   QLineEdit* addSomeEdit( const QString &lbl_str );
   QLineEdit* addIntEdit( const QString &lbl_str, int vmin, int vmax );
   QLineEdit* addDoubleEdit( const QString &lbl_str, double vmin, double vmax, int digs );
   QComboBox* addComboBox( const QString &lbl_str, const QStringList &items );
   QCheckBox* addCheckBox( const QString &lbl_str );

   GenerData &d;
   const QVector<FormInfo> &forms;

   static const unsigned max_n_col = 5; // really * 2
   unsigned c_row[max_n_col] = {}, c_col = 0;
   QVBoxLayout *verticalLayout;
   QGridLayout *gridLayout;

   QLineEdit *w_le,  *h_le,  *iter_le;

   QComboBox *type_cb;

   QLineEdit *size0_le, *scale_le;
   QCheckBox *ovr_scale_cb;

   QLineEdit *ss_le, *as_le, *aa_le;
   QLineEdit *a_le,   *b_le,  *c_le, *d_le;

   QPushButton *btnRevert;
   QPushButton *btnDefault;

   QDialogButtonBox *buttonBox;
};

#endif
