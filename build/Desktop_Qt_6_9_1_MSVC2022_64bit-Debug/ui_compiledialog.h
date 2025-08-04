/********************************************************************************
** Form generated from reading UI file 'compiledialog.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COMPILEDIALOG_H
#define UI_COMPILEDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_compiledialog
{
public:
    QDialogButtonBox *buttonBox;
    QLabel *label;
    QLineEdit *flexEdit;
    QPushButton *flexButton;

    void setupUi(QDialog *compiledialog)
    {
        if (compiledialog->objectName().isEmpty())
            compiledialog->setObjectName("compiledialog");
        compiledialog->resize(400, 178);
        buttonBox = new QDialogButtonBox(compiledialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(120, 105, 161, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        label = new QLabel(compiledialog);
        label->setObjectName("label");
        label->setGeometry(QRect(30, 30, 101, 16));
        flexEdit = new QLineEdit(compiledialog);
        flexEdit->setObjectName("flexEdit");
        flexEdit->setGeometry(QRect(30, 55, 311, 20));
        flexButton = new QPushButton(compiledialog);
        flexButton->setObjectName("flexButton");
        flexButton->setGeometry(QRect(350, 54, 30, 23));

        retranslateUi(compiledialog);
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, compiledialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(compiledialog);
    } // setupUi

    void retranslateUi(QDialog *compiledialog)
    {
        compiledialog->setWindowTitle(QCoreApplication::translate("compiledialog", "Dialog", nullptr));
        label->setText(QCoreApplication::translate("compiledialog", "Flex SDK compiler:", nullptr));
        flexButton->setText(QCoreApplication::translate("compiledialog", "...", nullptr));
    } // retranslateUi

};

namespace Ui {
    class compiledialog: public Ui_compiledialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COMPILEDIALOG_H
