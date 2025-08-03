/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
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
    QAction *actionSet_dark_theme;
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QTreeWidget *modsList;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuHelp;
    QMenu *menuTools;
    QMenu *menuView;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(995, 560);
        actionOpen = new QAction(MainWindow);
        actionOpen->setObjectName("actionOpen");
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/install.ico"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        actionOpen->setIcon(icon);
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName("actionExit");
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName("actionAbout");
        actionUninstall = new QAction(MainWindow);
        actionUninstall->setObjectName("actionUninstall");
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/uninstall.ico"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        actionUninstall->setIcon(icon1);
        actionAssociate_x2m_extension = new QAction(MainWindow);
        actionAssociate_x2m_extension->setObjectName("actionAssociate_x2m_extension");
        actionSlot_editor = new QAction(MainWindow);
        actionSlot_editor->setObjectName("actionSlot_editor");
        actionFind_and_delete_dead_ids = new QAction(MainWindow);
        actionFind_and_delete_dead_ids->setObjectName("actionFind_and_delete_dead_ids");
        actionClear_installation = new QAction(MainWindow);
        actionClear_installation->setObjectName("actionClear_installation");
        actionRemoveEmptyDirs = new QAction(MainWindow);
        actionRemoveEmptyDirs->setObjectName("actionRemoveEmptyDirs");
        actionChange_3_letter_code_of_x2m = new QAction(MainWindow);
        actionChange_3_letter_code_of_x2m->setObjectName("actionChange_3_letter_code_of_x2m");
        actionConvert_charalist = new QAction(MainWindow);
        actionConvert_charalist->setObjectName("actionConvert_charalist");
        actionCompile_INTERNAL_folder = new QAction(MainWindow);
        actionCompile_INTERNAL_folder->setObjectName("actionCompile_INTERNAL_folder");
        actionSlot_editor_stages = new QAction(MainWindow);
        actionSlot_editor_stages->setObjectName("actionSlot_editor_stages");
        actionSlot_editor_stages_local_mode = new QAction(MainWindow);
        actionSlot_editor_stages_local_mode->setObjectName("actionSlot_editor_stages_local_mode");
        actionTriggerPortOover = new QAction(MainWindow);
        actionTriggerPortOover->setObjectName("actionTriggerPortOover");
        actionSet_dark_theme = new QAction(MainWindow);
        actionSet_dark_theme->setObjectName("actionSet_dark_theme");
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName("centralWidget");
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(3, 0, 3, 0);
        modsList = new QTreeWidget(centralWidget);
        modsList->setObjectName("modsList");
        modsList->setContextMenuPolicy(Qt::ActionsContextMenu);
        modsList->setSelectionMode(QAbstractItemView::ExtendedSelection);
        modsList->setRootIsDecorated(false);

        horizontalLayout->addWidget(modsList);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName("menuBar");
        menuBar->setGeometry(QRect(0, 0, 995, 22));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName("menuFile");
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName("menuHelp");
        menuTools = new QMenu(menuBar);
        menuTools->setObjectName("menuTools");
        menuView = new QMenu(menuBar);
        menuView->setObjectName("menuView");
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName("mainToolBar");
        MainWindow->addToolBar(Qt::ToolBarArea::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName("statusBar");
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuView->menuAction());
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
        menuView->addAction(actionSet_dark_theme);
        mainToolBar->addAction(actionOpen);
        mainToolBar->addAction(actionUninstall);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "XV2 Mods Installer", nullptr));
        actionOpen->setText(QCoreApplication::translate("MainWindow", "Install", nullptr));
        actionExit->setText(QCoreApplication::translate("MainWindow", "Exit", nullptr));
        actionAbout->setText(QCoreApplication::translate("MainWindow", "About", nullptr));
        actionUninstall->setText(QCoreApplication::translate("MainWindow", "Uninstall", nullptr));
        actionAssociate_x2m_extension->setText(QCoreApplication::translate("MainWindow", "Associate .x2m extension", nullptr));
        actionSlot_editor->setText(QCoreApplication::translate("MainWindow", "Slot editor", nullptr));
        actionFind_and_delete_dead_ids->setText(QCoreApplication::translate("MainWindow", "Find dead ids", nullptr));
        actionClear_installation->setText(QCoreApplication::translate("MainWindow", "Clear installation", nullptr));
        actionRemoveEmptyDirs->setText(QCoreApplication::translate("MainWindow", "Remove empty directories", nullptr));
        actionChange_3_letter_code_of_x2m->setText(QCoreApplication::translate("MainWindow", "Change 3-letter code of x2m", nullptr));
        actionConvert_charalist->setText(QCoreApplication::translate("MainWindow", "Convert CharaList", nullptr));
        actionCompile_INTERNAL_folder->setText(QCoreApplication::translate("MainWindow", "Compile INTERNAL folder", nullptr));
        actionSlot_editor_stages->setText(QCoreApplication::translate("MainWindow", "Slot editor (stages)", nullptr));
        actionSlot_editor_stages_local_mode->setText(QCoreApplication::translate("MainWindow", "Slot editor (stages, local mode)", nullptr));
        actionTriggerPortOover->setText(QCoreApplication::translate("MainWindow", "Trigger port over", nullptr));
        actionSet_dark_theme->setText(QCoreApplication::translate("MainWindow", "Toggle dark theme", nullptr));
#if QT_CONFIG(tooltip)
        actionSet_dark_theme->setToolTip(QCoreApplication::translate("MainWindow", "Toggle dark theme", nullptr));
#endif // QT_CONFIG(tooltip)
        QTreeWidgetItem *___qtreewidgetitem = modsList->headerItem();
        ___qtreewidgetitem->setText(4, QCoreApplication::translate("MainWindow", "Install Date", nullptr));
        ___qtreewidgetitem->setText(3, QCoreApplication::translate("MainWindow", "Type", nullptr));
        ___qtreewidgetitem->setText(2, QCoreApplication::translate("MainWindow", "Version", nullptr));
        ___qtreewidgetitem->setText(1, QCoreApplication::translate("MainWindow", "Author", nullptr));
        ___qtreewidgetitem->setText(0, QCoreApplication::translate("MainWindow", "Name", nullptr));
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "File", nullptr));
        menuHelp->setTitle(QCoreApplication::translate("MainWindow", "Help", nullptr));
        menuTools->setTitle(QCoreApplication::translate("MainWindow", "Tools", nullptr));
        menuView->setTitle(QCoreApplication::translate("MainWindow", "View", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
