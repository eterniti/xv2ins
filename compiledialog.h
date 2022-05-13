#ifndef COMPILEDIALOG_H
#define COMPILEDIALOG_H

#include <QDialog>

namespace Ui {
class compiledialog;
}

class CompileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CompileDialog(QWidget *parent = 0);
    ~CompileDialog();

private slots:
    void on_flexButton_clicked();

    void on_buttonBox_accepted();

private:
    Ui::compiledialog *ui;
};

#endif // COMPILEDIALOG_H
