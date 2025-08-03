/********************************************************************************
** Form generated from reading UI file 'cssdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CSSDIALOG_H
#define UI_CSSDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
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
            CssDialog->setObjectName("CssDialog");
        CssDialog->resize(758, 346);
        buttonBox = new QDialogButtonBox(CssDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setGeometry(QRect(290, 290, 161, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        buttonBox->setCenterButtons(true);
        scrollArea = new QScrollArea(CssDialog);
        scrollArea->setObjectName("scrollArea");
        scrollArea->setGeometry(QRect(30, 40, 698, 226));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName("scrollAreaWidgetContents");
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 696, 224));
        gridLayout = new QGridLayout(scrollAreaWidgetContents);
        gridLayout->setSpacing(0);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        scrollArea->setWidget(scrollAreaWidgetContents);
        cancelButton = new QPushButton(CssDialog);
        cancelButton->setObjectName("cancelButton");
        cancelButton->setGeometry(QRect(330, 294, 75, 23));
        addButton = new QToolButton(CssDialog);
        addButton->setObjectName("addButton");
        addButton->setGeometry(QRect(726, 248, 25, 19));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/add.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        addButton->setIcon(icon);
        tipLabel = new QLabel(CssDialog);
        tipLabel->setObjectName("tipLabel");
        tipLabel->setGeometry(QRect(30, 14, 461, 16));

        retranslateUi(CssDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, CssDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, CssDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(CssDialog);
    } // setupUi

    void retranslateUi(QDialog *CssDialog)
    {
        CssDialog->setWindowTitle(QCoreApplication::translate("CssDialog", "Dialog", nullptr));
        cancelButton->setText(QCoreApplication::translate("CssDialog", "Cancel", nullptr));
        addButton->setText(QCoreApplication::translate("CssDialog", "...", nullptr));
        tipLabel->setText(QCoreApplication::translate("CssDialog", "<html><head/><body><p><span style=\" font-weight:600;\">Tip: use middle or right button to drag a slot into another one to open a menu.</span></p></body></html>", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CssDialog: public Ui_CssDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CSSDIALOG_H
