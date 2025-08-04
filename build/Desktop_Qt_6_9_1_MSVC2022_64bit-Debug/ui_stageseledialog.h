/********************************************************************************
** Form generated from reading UI file 'stageseledialog.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_STAGESELEDIALOG_H
#define UI_STAGESELEDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QToolButton>

QT_BEGIN_NAMESPACE

class Ui_StageSeleDialog
{
public:
    QDialogButtonBox *buttonBox;
    QLabel *label;
    QComboBox *pageComboBox;
    QToolButton *addButton;

    void setupUi(QDialog *StageSeleDialog)
    {
        if (StageSeleDialog->objectName().isEmpty())
            StageSeleDialog->setObjectName("StageSeleDialog");
        StageSeleDialog->resize(1285, 330);
        buttonBox = new QDialogButtonBox(StageSeleDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(10, 274, 1161, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        buttonBox->setCenterButtons(true);
        label = new QLabel(StageSeleDialog);
        label->setObjectName("label");
        label->setGeometry(QRect(530, 20, 47, 13));
        pageComboBox = new QComboBox(StageSeleDialog);
        pageComboBox->setObjectName("pageComboBox");
        pageComboBox->setGeometry(QRect(570, 18, 111, 22));
        addButton = new QToolButton(StageSeleDialog);
        addButton->setObjectName("addButton");
        addButton->setGeometry(QRect(20, 250, 25, 19));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/add.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        addButton->setIcon(icon);

        retranslateUi(StageSeleDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, StageSeleDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, StageSeleDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(StageSeleDialog);
    } // setupUi

    void retranslateUi(QDialog *StageSeleDialog)
    {
        StageSeleDialog->setWindowTitle(QCoreApplication::translate("StageSeleDialog", "Edit slots", nullptr));
        label->setText(QCoreApplication::translate("StageSeleDialog", "Page:", nullptr));
        addButton->setText(QCoreApplication::translate("StageSeleDialog", "...", nullptr));
    } // retranslateUi

};

namespace Ui {
    class StageSeleDialog: public Ui_StageSeleDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_STAGESELEDIALOG_H
