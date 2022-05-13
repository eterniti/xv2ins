/********************************************************************************
** Form generated from reading UI file 'cssdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CSSDIALOG_H
#define UI_CSSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CssDialog
{
public:
    QDialogButtonBox *buttonBox;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QGridLayout *gridLayout;
    QPushButton *cancelButton;
    QToolButton *addButton;
    QLabel *tipLabel;

    void setupUi(QDialog *CssDialog)
    {
        if (CssDialog->objectName().isEmpty())
            CssDialog->setObjectName(QStringLiteral("CssDialog"));
        CssDialog->resize(758, 346);
        buttonBox = new QDialogButtonBox(CssDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(290, 290, 161, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        buttonBox->setCenterButtons(true);
        scrollArea = new QScrollArea(CssDialog);
        scrollArea->setObjectName(QStringLiteral("scrollArea"));
        scrollArea->setGeometry(QRect(30, 40, 698, 226));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 696, 224));
        gridLayout = new QGridLayout(scrollAreaWidgetContents);
        gridLayout->setSpacing(0);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        scrollArea->setWidget(scrollAreaWidgetContents);
        cancelButton = new QPushButton(CssDialog);
        cancelButton->setObjectName(QStringLiteral("cancelButton"));
        cancelButton->setGeometry(QRect(330, 294, 75, 23));
        addButton = new QToolButton(CssDialog);
        addButton->setObjectName(QStringLiteral("addButton"));
        addButton->setGeometry(QRect(726, 248, 25, 19));
        QIcon icon;
        icon.addFile(QStringLiteral(":/add.png"), QSize(), QIcon::Normal, QIcon::Off);
        addButton->setIcon(icon);
        tipLabel = new QLabel(CssDialog);
        tipLabel->setObjectName(QStringLiteral("tipLabel"));
        tipLabel->setGeometry(QRect(30, 14, 461, 16));

        retranslateUi(CssDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), CssDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), CssDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(CssDialog);
    } // setupUi

    void retranslateUi(QDialog *CssDialog)
    {
        CssDialog->setWindowTitle(QApplication::translate("CssDialog", "Dialog", 0));
        cancelButton->setText(QApplication::translate("CssDialog", "Cancel", 0));
        addButton->setText(QApplication::translate("CssDialog", "...", 0));
        tipLabel->setText(QApplication::translate("CssDialog", "<html><head/><body><p><span style=\" font-weight:600;\">Tip: use middle or right button to drag a slot into another one to open a menu.</span></p></body></html>", 0));
    } // retranslateUi

};

namespace Ui {
    class CssDialog: public Ui_CssDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CSSDIALOG_H
