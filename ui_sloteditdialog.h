/********************************************************************************
** Form generated from reading UI file 'sloteditdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SLOTEDITDIALOG_H
#define UI_SLOTEDITDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_SlotEditDialog
{
public:
    QDialogButtonBox *buttonBox;
    QPushButton *pushButton;
    QLabel *label;
    QLabel *label_2;
    QLineEdit *costumeEdit;
    QLineEdit *codeEdit;
    QLabel *label_3;
    QLineEdit *presetEdit;
    QCheckBox *gk2Check;
    QLabel *label_4;
    QLineEdit *unlockEdit;
    QLabel *label_5;
    QLineEdit *voicesEdit;
    QLabel *label_6;
    QComboBox *dlcComboBox;

    void setupUi(QDialog *SlotEditDialog)
    {
        if (SlotEditDialog->objectName().isEmpty())
            SlotEditDialog->setObjectName(QStringLiteral("SlotEditDialog"));
        SlotEditDialog->resize(342, 362);
        buttonBox = new QDialogButtonBox(SlotEditDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(40, 302, 251, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        buttonBox->setCenterButtons(true);
        pushButton = new QPushButton(SlotEditDialog);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(234, 20, 91, 23));
        label = new QLabel(SlotEditDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(55, 70, 47, 13));
        label_2 = new QLabel(SlotEditDialog);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(9, 100, 81, 16));
        costumeEdit = new QLineEdit(SlotEditDialog);
        costumeEdit->setObjectName(QStringLiteral("costumeEdit"));
        costumeEdit->setGeometry(QRect(95, 98, 113, 20));
        codeEdit = new QLineEdit(SlotEditDialog);
        codeEdit->setObjectName(QStringLiteral("codeEdit"));
        codeEdit->setGeometry(QRect(95, 68, 113, 20));
        label_3 = new QLabel(SlotEditDialog);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(20, 130, 71, 16));
        presetEdit = new QLineEdit(SlotEditDialog);
        presetEdit->setObjectName(QStringLiteral("presetEdit"));
        presetEdit->setGeometry(QRect(95, 128, 113, 20));
        gk2Check = new QCheckBox(SlotEditDialog);
        gk2Check->setObjectName(QStringLiteral("gk2Check"));
        gk2Check->setGeometry(QRect(20, 190, 70, 17));
        label_4 = new QLabel(SlotEditDialog);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(23, 160, 71, 16));
        unlockEdit = new QLineEdit(SlotEditDialog);
        unlockEdit->setObjectName(QStringLiteral("unlockEdit"));
        unlockEdit->setGeometry(QRect(95, 158, 113, 20));
        label_5 = new QLabel(SlotEditDialog);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(24, 220, 71, 16));
        voicesEdit = new QLineEdit(SlotEditDialog);
        voicesEdit->setObjectName(QStringLiteral("voicesEdit"));
        voicesEdit->setGeometry(QRect(95, 218, 113, 20));
        label_6 = new QLabel(SlotEditDialog);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(62, 250, 31, 16));
        dlcComboBox = new QComboBox(SlotEditDialog);
        dlcComboBox->setObjectName(QStringLiteral("dlcComboBox"));
        dlcComboBox->setGeometry(QRect(95, 250, 113, 22));

        retranslateUi(SlotEditDialog);
        QObject::connect(buttonBox, SIGNAL(rejected()), SlotEditDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(SlotEditDialog);
    } // setupUi

    void retranslateUi(QDialog *SlotEditDialog)
    {
        SlotEditDialog->setWindowTitle(QApplication::translate("SlotEditDialog", "Dialog", 0));
        pushButton->setText(QApplication::translate("SlotEditDialog", "Copy from game", 0));
        label->setText(QApplication::translate("SlotEditDialog", "Code:", 0));
        label_2->setText(QApplication::translate("SlotEditDialog", "Costume index:", 0));
        label_3->setText(QApplication::translate("SlotEditDialog", "Model preset:", 0));
        gk2Check->setText(QApplication::translate("SlotEditDialog", "Flag GK2", 0));
        label_4->setText(QApplication::translate("SlotEditDialog", "Unlock index:", 0));
        label_5->setText(QApplication::translate("SlotEditDialog", "Voices id list:", 0));
        label_6->setText(QApplication::translate("SlotEditDialog", "DLC:", 0));
        dlcComboBox->clear();
        dlcComboBox->insertItems(0, QStringList()
         << QApplication::translate("SlotEditDialog", "Dlc_Def", 0)
         << QApplication::translate("SlotEditDialog", "Dlc_Gkb", 0)
         << QApplication::translate("SlotEditDialog", "Dlc_1", 0)
         << QApplication::translate("SlotEditDialog", "Dlc_2", 0)
         << QApplication::translate("SlotEditDialog", "Dlc_3", 0)
         << QApplication::translate("SlotEditDialog", "Dlc_4", 0)
         << QApplication::translate("SlotEditDialog", "Dlc_5", 0)
         << QApplication::translate("SlotEditDialog", "Dlc_6", 0)
         << QApplication::translate("SlotEditDialog", "Dlc_7", 0)
         << QApplication::translate("SlotEditDialog", "Dlc_8", 0)
         << QApplication::translate("SlotEditDialog", "Dlc_9", 0)
         << QApplication::translate("SlotEditDialog", "Dlc_10", 0)
         << QApplication::translate("SlotEditDialog", "Ver_Day1", 0)
         << QApplication::translate("SlotEditDialog", "Ver_TU4", 0)
         << QApplication::translate("SlotEditDialog", "UD7", 0)
         << QApplication::translate("SlotEditDialog", "PRB", 0)
         << QApplication::translate("SlotEditDialog", "EL0", 0)
         << QApplication::translate("SlotEditDialog", "Dlc_12", 0)
         << QApplication::translate("SlotEditDialog", "Dlc_13", 0)
         << QApplication::translate("SlotEditDialog", "Dlc_14", 0)
        );
    } // retranslateUi

};

namespace Ui {
    class SlotEditDialog: public Ui_SlotEditDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SLOTEDITDIALOG_H
