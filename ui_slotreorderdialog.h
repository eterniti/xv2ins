/********************************************************************************
** Form generated from reading UI file 'slotreorderdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SLOTREORDERDIALOG_H
#define UI_SLOTREORDERDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>

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
            SlotReorderDialog->setObjectName(QStringLiteral("SlotReorderDialog"));
        SlotReorderDialog->resize(487, 131);
        buttonBox = new QDialogButtonBox(SlotReorderDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(0, 70, 481, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        buttonBox->setCenterButtons(true);
        beforeCheckBox = new QCheckBox(SlotReorderDialog);
        beforeCheckBox->setObjectName(QStringLiteral("beforeCheckBox"));
        beforeCheckBox->setGeometry(QRect(30, 20, 181, 17));
        comboBox = new QComboBox(SlotReorderDialog);
        comboBox->setObjectName(QStringLiteral("comboBox"));
        comboBox->setGeometry(QRect(225, 20, 241, 22));

        retranslateUi(SlotReorderDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), SlotReorderDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), SlotReorderDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(SlotReorderDialog);
    } // setupUi

    void retranslateUi(QDialog *SlotReorderDialog)
    {
        SlotReorderDialog->setWindowTitle(QApplication::translate("SlotReorderDialog", "Move", 0));
        beforeCheckBox->setText(QApplication::translate("SlotReorderDialog", "Move before (after if unchecked)", 0));
    } // retranslateUi

};

namespace Ui {
    class SlotReorderDialog: public Ui_SlotReorderDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SLOTREORDERDIALOG_H
