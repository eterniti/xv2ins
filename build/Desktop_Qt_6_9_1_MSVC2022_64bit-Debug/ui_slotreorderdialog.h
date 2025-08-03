/********************************************************************************
** Form generated from reading UI file 'slotreorderdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SLOTREORDERDIALOG_H
#define UI_SLOTREORDERDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>

QT_BEGIN_NAMESPACE

class Ui_SlotReorderDialog
{
public:
    QDialogButtonBox *buttonBox;
    QCheckBox *beforeCheckBox;
    QComboBox *comboBox;

    void setupUi(QDialog *SlotReorderDialog)
    {
        if (SlotReorderDialog->objectName().isEmpty())
            SlotReorderDialog->setObjectName("SlotReorderDialog");
        SlotReorderDialog->resize(487, 131);
        buttonBox = new QDialogButtonBox(SlotReorderDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(0, 70, 481, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        buttonBox->setCenterButtons(true);
        beforeCheckBox = new QCheckBox(SlotReorderDialog);
        beforeCheckBox->setObjectName("beforeCheckBox");
        beforeCheckBox->setGeometry(QRect(30, 20, 181, 17));
        comboBox = new QComboBox(SlotReorderDialog);
        comboBox->setObjectName("comboBox");
        comboBox->setGeometry(QRect(225, 20, 241, 22));

        retranslateUi(SlotReorderDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, SlotReorderDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, SlotReorderDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(SlotReorderDialog);
    } // setupUi

    void retranslateUi(QDialog *SlotReorderDialog)
    {
        SlotReorderDialog->setWindowTitle(QCoreApplication::translate("SlotReorderDialog", "Move", nullptr));
        beforeCheckBox->setText(QCoreApplication::translate("SlotReorderDialog", "Move before (after if unchecked)", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SlotReorderDialog: public Ui_SlotReorderDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SLOTREORDERDIALOG_H
