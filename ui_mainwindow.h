/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionOpen;
    QAction *actionExit;
    QAction *actionAbout;
    QAction *actionUninstall;
    QAction *actionAssociate_x2m_extension;
    QAction *actionSlot_editor;
    QAction *actionFind_and_delete_dead_ids;
    QAction *actionClear_installation;
    QAction *actionRemoveEmptyDirs;
    QAction *actionChange_3_letter_code_of_x2m;
    QAction *actionConvert_charalist;
    QAction *actionCompile_INTERNAL_folder;
    QAction *actionSlot_editor_stages;
    QAction *actionSlot_editor_stages_local_mode;
    QAction *actionTriggerPortOover;
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QTreeWidget *modsList;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuHelp;
    QMenu *menuTools;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(995, 560);
        actionOpen = new QAction(MainWindow);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/install.ico"), QSize(), QIcon::Normal, QIcon::Off);
        actionOpen->setIcon(icon);
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName(QStringLiteral("actionAbout"));
        actionUninstall = new QAction(MainWindow);
        actionUninstall->setObjectName(QStringLiteral("actionUninstall"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/uninstall.ico"), QSize(), QIcon::Normal, QIcon::Off);
        actionUninstall->setIcon(icon1);
        actionAssociate_x2m_extension = new QAction(MainWindow);
        actionAssociate_x2m_extension->setObjectName(QStringLiteral("actionAssociate_x2m_extension"));
        actionSlot_editor = new QAction(MainWindow);
        actionSlot_editor->setObjectName(QStringLiteral("actionSlot_editor"));
        actionFind_and_delete_dead_ids = new QAction(MainWindow);
        actionFind_and_delete_dead_ids->setObjectName(QStringLiteral("actionFind_and_delete_dead_ids"));
        actionClear_installation = new QAction(MainWindow);
        actionClear_installation->setObjectName(QStringLiteral("actionClear_installation"));
        actionRemoveEmptyDirs = new QAction(MainWindow);
        actionRemoveEmptyDirs->setObjectName(QStringLiteral("actionRemoveEmptyDirs"));
        actionChange_3_letter_code_of_x2m = new QAction(MainWindow);
        actionChange_3_letter_code_of_x2m->setObjectName(QStringLiteral("actionChange_3_letter_code_of_x2m"));
        actionConvert_charalist = new QAction(MainWindow);
        actionConvert_charalist->setObjectName(QStringLiteral("actionConvert_charalist"));
        actionCompile_INTERNAL_folder = new QAction(MainWindow);
        actionCompile_INTERNAL_folder->setObjectName(QStringLiteral("actionCompile_INTERNAL_folder"));
        actionSlot_editor_stages = new QAction(MainWindow);
        actionSlot_editor_stages->setObjectName(QStringLiteral("actionSlot_editor_stages"));
        actionSlot_editor_stages_local_mode = new QAction(MainWindow);
        actionSlot_editor_stages_local_mode->setObjectName(QStringLiteral("actionSlot_editor_stages_local_mode"));
        actionTriggerPortOover = new QAction(MainWindow);
        actionTriggerPortOover->setObjectName(QStringLiteral("actionTriggerPortOover"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(3, 0, 3, 0);
        modsList = new QTreeWidget(centralWidget);
        modsList->setObjectName(QStringLiteral("modsList"));
        modsList->setContextMenuPolicy(Qt::ActionsContextMenu);
        modsList->setSelectionMode(QAbstractItemView::ExtendedSelection);
        modsList->setRootIsDecorated(false);

        horizontalLayout->addWidget(modsList);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 995, 22));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QStringLiteral("menuHelp"));
        menuTools = new QMenu(menuBar);
        menuTools->setObjectName(QStringLiteral("menuTools"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuTools->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionOpen);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);
        menuHelp->addAction(actionAbout);
        menuTools->addAction(actionAssociate_x2m_extension);
        menuTools->addAction(actionSlot_editor);
        menuTools->addAction(actionSlot_editor_stages);
        menuTools->addAction(actionSlot_editor_stages_local_mode);
        menuTools->addAction(actionFind_and_delete_dead_ids);
        menuTools->addAction(actionChange_3_letter_code_of_x2m);
        menuTools->addAction(actionCompile_INTERNAL_folder);
        menuTools->addAction(actionConvert_charalist);
        menuTools->addSeparator();
        menuTools->addAction(actionClear_installation);
        menuTools->addAction(actionRemoveEmptyDirs);
        menuTools->addAction(actionTriggerPortOover);
        mainToolBar->addAction(actionOpen);
        mainToolBar->addAction(actionUninstall);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "XV2 Mods Installer", 0));
        actionOpen->setText(QApplication::translate("MainWindow", "Install", 0));
        actionExit->setText(QApplication::translate("MainWindow", "Exit", 0));
        actionAbout->setText(QApplication::translate("MainWindow", "About", 0));
        actionUninstall->setText(QApplication::translate("MainWindow", "Uninstall", 0));
        actionAssociate_x2m_extension->setText(QApplication::translate("MainWindow", "Associate .x2m extension", 0));
        actionSlot_editor->setText(QApplication::translate("MainWindow", "Slot editor", 0));
        actionFind_and_delete_dead_ids->setText(QApplication::translate("MainWindow", "Find dead ids", 0));
        actionClear_installation->setText(QApplication::translate("MainWindow", "Clear installation", 0));
        actionRemoveEmptyDirs->setText(QApplication::translate("MainWindow", "Remove empty directories", 0));
        actionChange_3_letter_code_of_x2m->setText(QApplication::translate("MainWindow", "Change 3-letter code of x2m", 0));
        actionConvert_charalist->setText(QApplication::translate("MainWindow", "Convert CharaList", 0));
        actionCompile_INTERNAL_folder->setText(QApplication::translate("MainWindow", "Compile INTERNAL folder", 0));
        actionSlot_editor_stages->setText(QApplication::translate("MainWindow", "Slot editor (stages)", 0));
        actionSlot_editor_stages_local_mode->setText(QApplication::translate("MainWindow", "Slot editor (stages, local mode)", 0));
        actionTriggerPortOover->setText(QApplication::translate("MainWindow", "Trigger port over", 0));
        QTreeWidgetItem *___qtreewidgetitem = modsList->headerItem();
        ___qtreewidgetitem->setText(4, QApplication::translate("MainWindow", "Install Date", 0));
        ___qtreewidgetitem->setText(3, QApplication::translate("MainWindow", "Type", 0));
        ___qtreewidgetitem->setText(2, QApplication::translate("MainWindow", "Version", 0));
        ___qtreewidgetitem->setText(1, QApplication::translate("MainWindow", "Author", 0));
        ___qtreewidgetitem->setText(0, QApplication::translate("MainWindow", "Name", 0));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", 0));
        menuHelp->setTitle(QApplication::translate("MainWindow", "Help", 0));
        menuTools->setTitle(QApplication::translate("MainWindow", "Tools", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
