#include "compiledialog.h"
#include "ui_compiledialog.h"

#include <QFileDialog>
#include "Config.h"
#include "debug.h"

CompileDialog::CompileDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::compiledialog)
{
    ui->setupUi(this);
    ui->flexEdit->setText(config.flex_path);
}

CompileDialog::~CompileDialog()
{
    delete ui;
}

void CompileDialog::on_flexButton_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, "Select mxmlc.exe", "", "mxmlc.exe (mxmlc.exe)");

    if (file.isNull())
        return;

    ui->flexEdit->setText(file);
}

void CompileDialog::on_buttonBox_accepted()
{
    QString compiler_path = ui->flexEdit->text().trimmed();

    if (compiler_path.isEmpty())
    {
        DPRINTF("Compiler field cannot be empty.\n");
        return;
    }

    QFile compiler(compiler_path);

    if (!compiler.exists())
    {
        DPRINTF("The specified file doesn't exist\n");
        return;
    }

    config.flex_path = compiler_path;
    accept();
}
