/********************************************************************************
** Form generated from reading UI file 'listdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LISTDIALOG_H
#define UI_LISTDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QListWidget>

QT_BEGIN_NAMESPACE

class Ui_ListDialog
{
public:
    QDialogButtonBox *buttonBox;
    QListWidget *listWidget;

    void setupUi(QDialog *ListDialog)
    {
        if (ListDialog->objectName().isEmpty())
            ListDialog->setObjectName(QStringLiteral("ListDialog"));
        ListDialog->resize(352, 371);
        buttonBox = new QDialogButtonBox(ListDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(1, 320, 351, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        buttonBox->setCenterButtons(true);
        listWidget = new QListWidget(ListDialog);
        listWidget->setObjectName(QStringLiteral("listWidget"));
        listWidget->setGeometry(QRect(28, 20, 296, 281));
        listWidget->setSortingEnabled(true);

        retranslateUi(ListDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), ListDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), ListDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(ListDialog);
    } // setupUi

    void retranslateUi(QDialog *ListDialog)
    {
        ListDialog->setWindowTitle(QApplication::translate("ListDialog", "Select entry", 0));
    } // retranslateUi

};

namespace Ui {
    class ListDialog: public Ui_ListDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LISTDIALOG_H
