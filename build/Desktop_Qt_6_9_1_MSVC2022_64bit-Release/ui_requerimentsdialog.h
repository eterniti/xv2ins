/********************************************************************************
** Form generated from reading UI file 'requerimentsdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_REQUERIMENTSDIALOG_H
#define UI_REQUERIMENTSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_RequerimentsDialog
{
public:
    QDialogButtonBox *buttonBox;
    QLabel *label;
    QLineEdit *gameEdit;
    QPushButton *gameButton;

    void setupUi(QDialog *RequerimentsDialog)
    {
        if (RequerimentsDialog->objectName().isEmpty())
            RequerimentsDialog->setObjectName("RequerimentsDialog");
        RequerimentsDialog->resize(400, 178);
        buttonBox = new QDialogButtonBox(RequerimentsDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(120, 105, 161, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        label = new QLabel(RequerimentsDialog);
        label->setObjectName("label");
        label->setGeometry(QRect(30, 30, 81, 16));
        gameEdit = new QLineEdit(RequerimentsDialog);
        gameEdit->setObjectName("gameEdit");
        gameEdit->setGeometry(QRect(30, 55, 311, 20));
        gameButton = new QPushButton(RequerimentsDialog);
        gameButton->setObjectName("gameButton");
        gameButton->setGeometry(QRect(350, 54, 30, 23));

        retranslateUi(RequerimentsDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, RequerimentsDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, RequerimentsDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(RequerimentsDialog);
    } // setupUi

    void retranslateUi(QDialog *RequerimentsDialog)
    {
        RequerimentsDialog->setWindowTitle(QCoreApplication::translate("RequerimentsDialog", "Configure requirements", nullptr));
        label->setText(QCoreApplication::translate("RequerimentsDialog", "Game path:", nullptr));
        gameButton->setText(QCoreApplication::translate("RequerimentsDialog", "...", nullptr));
    } // retranslateUi

};

namespace Ui {
    class RequerimentsDialog: public Ui_RequerimentsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REQUERIMENTSDIALOG_H
