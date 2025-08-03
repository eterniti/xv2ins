/********************************************************************************
** Form generated from reading UI file 'cmsentrychoosedialog.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CMSENTRYCHOOSEDIALOG_H
#define UI_CMSENTRYCHOOSEDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>

QT_BEGIN_NAMESPACE

class Ui_CmsEntryChooseDialog
{
public:
    QDialogButtonBox *buttonBox;
    QLineEdit *cmsEntryEdit;
    QLabel *label;

    void setupUi(QDialog *CmsEntryChooseDialog)
    {
        if (CmsEntryChooseDialog->objectName().isEmpty())
            CmsEntryChooseDialog->setObjectName("CmsEntryChooseDialog");
        CmsEntryChooseDialog->resize(329, 158);
        buttonBox = new QDialogButtonBox(CmsEntryChooseDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(0, 100, 331, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        buttonBox->setCenterButtons(true);
        cmsEntryEdit = new QLineEdit(CmsEntryChooseDialog);
        cmsEntryEdit->setObjectName("cmsEntryEdit");
        cmsEntryEdit->setGeometry(QRect(30, 60, 81, 20));
        label = new QLabel(CmsEntryChooseDialog);
        label->setObjectName("label");
        label->setGeometry(QRect(30, 30, 271, 16));

        retranslateUi(CmsEntryChooseDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, CmsEntryChooseDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(CmsEntryChooseDialog);
    } // setupUi

    void retranslateUi(QDialog *CmsEntryChooseDialog)
    {
        CmsEntryChooseDialog->setWindowTitle(QCoreApplication::translate("CmsEntryChooseDialog", "Choose new 3-letter code:", nullptr));
        label->setText(QCoreApplication::translate("CmsEntryChooseDialog", "Choose new 3-letter code (leave blank for random):", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CmsEntryChooseDialog: public Ui_CmsEntryChooseDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CMSENTRYCHOOSEDIALOG_H
