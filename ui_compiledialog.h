/********************************************************************************
** Form generated from reading UI file 'compiledialog.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COMPILEDIALOG_H
#define UI_COMPILEDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>
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
            compiledialog->setObjectName(QStringLiteral("compiledialog"));
        compiledialog->resize(400, 178);
        buttonBox = new QDialogButtonBox(compiledialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(120, 105, 161, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        label = new QLabel(compiledialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(30, 30, 101, 16));
        flexEdit = new QLineEdit(compiledialog);
        flexEdit->setObjectName(QStringLiteral("flexEdit"));
        flexEdit->setGeometry(QRect(30, 55, 311, 20));
        flexButton = new QPushButton(compiledialog);
        flexButton->setObjectName(QStringLiteral("flexButton"));
        flexButton->setGeometry(QRect(350, 54, 30, 23));

        retranslateUi(compiledialog);
        QObject::connect(buttonBox, SIGNAL(rejected()), compiledialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(compiledialog);
    } // setupUi

    void retranslateUi(QDialog *compiledialog)
    {
        compiledialog->setWindowTitle(QApplication::translate("compiledialog", "Dialog", 0));
        label->setText(QApplication::translate("compiledialog", "Flex SDK compiler:", 0));
        flexButton->setText(QApplication::translate("compiledialog", "...", 0));
    } // retranslateUi

};

namespace Ui {
    class compiledialog: public Ui_compiledialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COMPILEDIALOG_H
