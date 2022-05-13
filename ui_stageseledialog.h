/********************************************************************************
** Form generated from reading UI file 'stageseledialog.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_STAGESELEDIALOG_H
#define UI_STAGESELEDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>
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
            StageSeleDialog->setObjectName(QStringLiteral("StageSeleDialog"));
        StageSeleDialog->resize(1285, 330);
        buttonBox = new QDialogButtonBox(StageSeleDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(10, 274, 1161, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        buttonBox->setCenterButtons(true);
        label = new QLabel(StageSeleDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(530, 20, 47, 13));
        pageComboBox = new QComboBox(StageSeleDialog);
        pageComboBox->setObjectName(QStringLiteral("pageComboBox"));
        pageComboBox->setGeometry(QRect(570, 18, 111, 22));
        addButton = new QToolButton(StageSeleDialog);
        addButton->setObjectName(QStringLiteral("addButton"));
        addButton->setGeometry(QRect(20, 250, 25, 19));
        QIcon icon;
        icon.addFile(QStringLiteral(":/add.png"), QSize(), QIcon::Normal, QIcon::Off);
        addButton->setIcon(icon);

        retranslateUi(StageSeleDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), StageSeleDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), StageSeleDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(StageSeleDialog);
    } // setupUi

    void retranslateUi(QDialog *StageSeleDialog)
    {
        StageSeleDialog->setWindowTitle(QApplication::translate("StageSeleDialog", "Edit slots", 0));
        label->setText(QApplication::translate("StageSeleDialog", "Page:", 0));
        addButton->setText(QApplication::translate("StageSeleDialog", "...", 0));
    } // retranslateUi

};

namespace Ui {
    class StageSeleDialog: public Ui_StageSeleDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_STAGESELEDIALOG_H
