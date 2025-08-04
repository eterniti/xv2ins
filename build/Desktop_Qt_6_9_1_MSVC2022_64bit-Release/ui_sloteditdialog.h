/********************************************************************************
** Form generated from reading UI file 'sloteditdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SLOTEDITDIALOG_H
#define UI_SLOTEDITDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
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
    QCheckBox *cgk2Check;

    void setupUi(QDialog *SlotEditDialog)
    {
        if (SlotEditDialog->objectName().isEmpty())
            SlotEditDialog->setObjectName("SlotEditDialog");
        SlotEditDialog->resize(342, 362);
        buttonBox = new QDialogButtonBox(SlotEditDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(40, 302, 251, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        buttonBox->setCenterButtons(true);
        pushButton = new QPushButton(SlotEditDialog);
        pushButton->setObjectName("pushButton");
        pushButton->setGeometry(QRect(234, 20, 91, 23));
        label = new QLabel(SlotEditDialog);
        label->setObjectName("label");
        label->setGeometry(QRect(55, 70, 47, 13));
        label_2 = new QLabel(SlotEditDialog);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(9, 100, 81, 16));
        costumeEdit = new QLineEdit(SlotEditDialog);
        costumeEdit->setObjectName("costumeEdit");
        costumeEdit->setGeometry(QRect(95, 98, 113, 20));
        codeEdit = new QLineEdit(SlotEditDialog);
        codeEdit->setObjectName("codeEdit");
        codeEdit->setGeometry(QRect(95, 68, 113, 20));
        label_3 = new QLabel(SlotEditDialog);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(20, 130, 71, 16));
        presetEdit = new QLineEdit(SlotEditDialog);
        presetEdit->setObjectName("presetEdit");
        presetEdit->setGeometry(QRect(95, 128, 113, 20));
        gk2Check = new QCheckBox(SlotEditDialog);
        gk2Check->setObjectName("gk2Check");
        gk2Check->setGeometry(QRect(20, 190, 70, 17));
        label_4 = new QLabel(SlotEditDialog);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(23, 160, 71, 16));
        unlockEdit = new QLineEdit(SlotEditDialog);
        unlockEdit->setObjectName("unlockEdit");
        unlockEdit->setGeometry(QRect(95, 158, 113, 20));
        label_5 = new QLabel(SlotEditDialog);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(24, 220, 71, 16));
        voicesEdit = new QLineEdit(SlotEditDialog);
        voicesEdit->setObjectName("voicesEdit");
        voicesEdit->setGeometry(QRect(95, 218, 113, 20));
        label_6 = new QLabel(SlotEditDialog);
        label_6->setObjectName("label_6");
        label_6->setGeometry(QRect(62, 250, 31, 16));
        dlcComboBox = new QComboBox(SlotEditDialog);
        dlcComboBox->addItem(QString());
        dlcComboBox->addItem(QString());
        dlcComboBox->addItem(QString());
        dlcComboBox->addItem(QString());
        dlcComboBox->addItem(QString());
        dlcComboBox->addItem(QString());
        dlcComboBox->addItem(QString());
        dlcComboBox->addItem(QString());
        dlcComboBox->addItem(QString());
        dlcComboBox->addItem(QString());
        dlcComboBox->addItem(QString());
        dlcComboBox->addItem(QString());
        dlcComboBox->addItem(QString());
        dlcComboBox->addItem(QString());
        dlcComboBox->addItem(QString());
        dlcComboBox->addItem(QString());
        dlcComboBox->addItem(QString());
        dlcComboBox->addItem(QString());
        dlcComboBox->addItem(QString());
        dlcComboBox->addItem(QString());
        dlcComboBox->addItem(QString());
        dlcComboBox->addItem(QString());
        dlcComboBox->addItem(QString());
        dlcComboBox->addItem(QString());
        dlcComboBox->setObjectName("dlcComboBox");
        dlcComboBox->setGeometry(QRect(95, 250, 113, 22));
        cgk2Check = new QCheckBox(SlotEditDialog);
        cgk2Check->setObjectName("cgk2Check");
        cgk2Check->setGeometry(QRect(120, 190, 91, 17));

        retranslateUi(SlotEditDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, SlotEditDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(SlotEditDialog);
    } // setupUi

    void retranslateUi(QDialog *SlotEditDialog)
    {
        SlotEditDialog->setWindowTitle(QCoreApplication::translate("SlotEditDialog", "Dialog", nullptr));
        pushButton->setText(QCoreApplication::translate("SlotEditDialog", "Copy from game", nullptr));
        label->setText(QCoreApplication::translate("SlotEditDialog", "Code:", nullptr));
        label_2->setText(QCoreApplication::translate("SlotEditDialog", "Costume index:", nullptr));
        label_3->setText(QCoreApplication::translate("SlotEditDialog", "Model preset:", nullptr));
        gk2Check->setText(QCoreApplication::translate("SlotEditDialog", "Flag GK2", nullptr));
        label_4->setText(QCoreApplication::translate("SlotEditDialog", "Unlock index:", nullptr));
        label_5->setText(QCoreApplication::translate("SlotEditDialog", "Voices id list:", nullptr));
        label_6->setText(QCoreApplication::translate("SlotEditDialog", "DLC:", nullptr));
        dlcComboBox->setItemText(0, QCoreApplication::translate("SlotEditDialog", "Dlc_Def", nullptr));
        dlcComboBox->setItemText(1, QCoreApplication::translate("SlotEditDialog", "Dlc_Gkb", nullptr));
        dlcComboBox->setItemText(2, QCoreApplication::translate("SlotEditDialog", "Dlc_1", nullptr));
        dlcComboBox->setItemText(3, QCoreApplication::translate("SlotEditDialog", "Dlc_2", nullptr));
        dlcComboBox->setItemText(4, QCoreApplication::translate("SlotEditDialog", "Dlc_3", nullptr));
        dlcComboBox->setItemText(5, QCoreApplication::translate("SlotEditDialog", "Dlc_4", nullptr));
        dlcComboBox->setItemText(6, QCoreApplication::translate("SlotEditDialog", "Dlc_5", nullptr));
        dlcComboBox->setItemText(7, QCoreApplication::translate("SlotEditDialog", "Dlc_6", nullptr));
        dlcComboBox->setItemText(8, QCoreApplication::translate("SlotEditDialog", "Dlc_7", nullptr));
        dlcComboBox->setItemText(9, QCoreApplication::translate("SlotEditDialog", "Dlc_8", nullptr));
        dlcComboBox->setItemText(10, QCoreApplication::translate("SlotEditDialog", "Dlc_9", nullptr));
        dlcComboBox->setItemText(11, QCoreApplication::translate("SlotEditDialog", "Dlc_10", nullptr));
        dlcComboBox->setItemText(12, QCoreApplication::translate("SlotEditDialog", "Ver_Day1", nullptr));
        dlcComboBox->setItemText(13, QCoreApplication::translate("SlotEditDialog", "Ver_TU4", nullptr));
        dlcComboBox->setItemText(14, QCoreApplication::translate("SlotEditDialog", "UD7", nullptr));
        dlcComboBox->setItemText(15, QCoreApplication::translate("SlotEditDialog", "PRB", nullptr));
        dlcComboBox->setItemText(16, QCoreApplication::translate("SlotEditDialog", "EL0", nullptr));
        dlcComboBox->setItemText(17, QCoreApplication::translate("SlotEditDialog", "Dlc_12", nullptr));
        dlcComboBox->setItemText(18, QCoreApplication::translate("SlotEditDialog", "Dlc_13", nullptr));
        dlcComboBox->setItemText(19, QCoreApplication::translate("SlotEditDialog", "Dlc_14", nullptr));
        dlcComboBox->setItemText(20, QCoreApplication::translate("SlotEditDialog", "Dlc_15", nullptr));
        dlcComboBox->setItemText(21, QCoreApplication::translate("SlotEditDialog", "Dlc_16", nullptr));
        dlcComboBox->setItemText(22, QCoreApplication::translate("SlotEditDialog", "Dlc_17", nullptr));
        dlcComboBox->setItemText(23, QCoreApplication::translate("SlotEditDialog", "Dlc_18", nullptr));

        cgk2Check->setText(QCoreApplication::translate("SlotEditDialog", "Flag CGK2", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SlotEditDialog: public Ui_SlotEditDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SLOTEDITDIALOG_H
