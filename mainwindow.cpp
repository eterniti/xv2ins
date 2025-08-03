#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QScreen>
#include <QPushButton>
#include <QCheckBox>
#include <QTime>
#include <QProcess>
#include <QStyleFactory>

#include <map>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "Xenoverse2.h"
#include "IniFile.h"
#include "EmbFile.h"
#include "xv2ins_common.h"
#include "Config.h"
#include "cssdialog.h"
#include "cmsentrychoosedialog.h"
#include "compiledialog.h"
#include "stageseledialog.h"
#include "debug.h"

#define CHASEL_PATH "Internal/CharaSele"

#define XV2INSTALLER_INSTANCE "XV2INSTALLER_INSTANCE"

//#define DEBUG_MEASURE_TIME

enum
{
    COLUMN_NAME,
    COLUMN_AUTHOR,
    COLUMN_VERSION,
    COLUMN_TYPE,
    COLUMN_DATE
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}

static LogDialog *log_dlg;

static void log_window(const char *dbg)
{
    log_dlg->Append(dbg);
}

static void log_window_error(const char *dbg)
{
    log_dlg->SetNextBold();
    log_dlg->SetNextColor("red");
    log_dlg->Append(dbg);
}

bool MainWindow::Initialize()
{ 
    QLabel *patreonLabel = new QLabel(this);
    patreonLabel->setFixedWidth(600);
    patreonLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    patreonLabel->setOpenExternalLinks(true);
    patreonLabel->setText("If you liked my tools, you can support their development at <a href='https://www.patreon.com/eternity_tools'>https://www.patreon.com/eternity_tools</a>");
    ui->statusBar->addPermanentWidget(patreonLabel);

    statusLabel = new QLabel(this);
    ui->statusBar->addPermanentWidget(statusLabel);

    ui->modsList->sortByColumn(COLUMN_NAME, Qt::AscendingOrder);
    ui->modsList->setSortingEnabled(true);
    ui->modsList->setCurrentItem(nullptr);

    ui->modsList->header()->resizeSection(COLUMN_NAME, 450);
    ui->modsList->header()->resizeSection(COLUMN_AUTHOR, 250);
    ui->modsList->header()->resizeSection(COLUMN_VERSION, 45);
    ui->modsList->addAction(ui->actionUninstall);
    ui->actionUninstall->setDisabled(true);
    ui->actionAssociate_x2m_extension->setIcon(this->style()->standardIcon(QStyle::SP_VistaShield));

    set_debug_level(2);
    QDir::setCurrent(qApp->applicationDirPath());

    CreateMutexA(nullptr, FALSE, XV2INSTALLER_INSTANCE);
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        UPRINTF("An instance of the installer is already running.\n");
        return false;
    }

    HANDLE handle = CreateMutexA(nullptr, FALSE, "XV2QUIMP_INSTANCE");
    bool qimp_opened = (GetLastError() == ERROR_ALREADY_EXISTS);
    if (handle)
        CloseHandle(handle);

    if (qimp_opened)
    {
        UPRINTF("XV2 Mods Installer cannot run at the same time than the XV2 Quest Importer.\n");
        return false;
    }

    bool needs_update;
    bool success = Bootstrap(false, true, &needs_update);
    XV2Patcher::Init();

    if (config.dark_theme)
        ToggleDarkTheme(false);

    if (!success && !needs_update)
    {
        return false;
    }

    if (!XV2Patcher::IsInstalled())
    {
        //DPRINTF("This program needs xv2patcher to be installed.\n");
        QMessageBox box;

        box.setWindowTitle("xv2ins");
        box.setIcon(QMessageBox::Critical);
        box.setTextFormat(Qt::RichText);
        box.setText("This program needs xv2patcher to be installed.<br><br>"
                    "Download from <a href=https://videogamemods.com/xenoverse/mods/eternity-tools/'>https://videogamemods.com/xenoverse/mods/eternity-tools/</a>");
        box.exec();
        return false;
    }

    float patcher_version = XV2Patcher::GetVersion();

    if (patcher_version < MINIMUM_PATCHER_REQUIRED)
    {
        QMessageBox box;

        box.setWindowTitle("xv2ins");
        box.setIcon(QMessageBox::Critical);
        box.setTextFormat(Qt::RichText);
        box.setText(QString("This program needs xv2patcher %1 or greater (your current version is %2). Update xv2patcher.<br><br>"
                    "Download lastest xv2patcher fron <a href='https://videogamemods.com/xenoverse/mods/eternity-tools/'>https://videogamemods.com/xenoverse/mods/eternity-tools/</a>").arg(MINIMUM_PATCHER_REQUIRED).arg(patcher_version));
        box.exec();
        return false;
    }

    if (!Utils::CompareFloat(patcher_version, config.patcher_version_new))
    {
        // First time install
        if (!XV2Patcher::ConfigureDefaults())
            return false;

        config.patcher_version_new = patcher_version;
        config.Save();
    }

    if (!CheckCharaSele())
    {
        return false;
    }

    bool clean_install = false;

    if (!success && needs_update)
    {
        clean_install = true;
    }
    else if (!game_cms->FindEntryByName("XXG") || !game_cms->FindEntryByName("XXV"))
    {
        clean_install = true;
    }
    else if (success)
    {
        if (xv2fs->FileExists("data/CLEAN_MODE_TEST.txt", false, true))
            clean_install = true;
    }

    if (clean_install)
    {
        QMessageBox box;

        box.setText("It looks like you have game data from an old update, the installer cannot work with these files.\n"
                    "\nTo continue, you will have to either clear installation or port over current installation (experimental)\n"
                    "\nIn both cases, you will have another confirmation screen before.");

        QPushButton *clear = box.addButton("Clear", QMessageBox::ActionRole);
        QPushButton *restore = box.addButton("Port over (experimental)", QMessageBox::ActionRole);
        box.addButton(QMessageBox::Cancel);
        box.setDefaultButton(clear);

        box.exec();

        if (box.clickedButton() == clear)
            ClearInstallation(false);
        else if (box.clickedButton() == restore)
            RestorePart1();

        return false;
    }

    CheckRegistryAssociation();

    if (qApp->arguments().size() >= 2)
    {
        if (qApp->arguments()[1] == "--restore")
        {
            if (qApp->arguments().size() == 2)
            {
                DPRINTF("--restore needs to be followed by a directory path.\n");
                return false;
            }

            invisible_mode = true;
            LoadInstalledMods(qApp->arguments()[2]);
            RestorePart2(qApp->arguments()[2]);
            return false;
        }
    }

    LoadInstalledMods();

    if (qApp->arguments().size() >= 2)
    {       
        bool silent = false;
        bool multiple = false;
        int total = qApp->arguments().size()-1;
        int installed = 0;

        if (total > 1)
        {
            silent = MultipleModsQuestion();
            multiple = true;
        }

        for (int i = 1; i < qApp->arguments().size(); i++)
        {
            QString file = qApp->arguments()[i];

            ModEntry *mod = InstallMod(file, nullptr, silent);
            invisible_mode = true;

            if (mod)
            {
                if (!multiple)
                {
                    if (!mod->item)
                        UPRINTF("Mod succesfully installed.\n");
                    else
                        UPRINTF("Mod succesfully updated.\n");
                }

                installed++;
                config.lf_installer_open = file;
            }
        }

        if (multiple)
        {
            if (installed == total)
            {
                UPRINTF("All mods were succesfully installed or updated.\n");
            }
            else
            {
                UPRINTF("%d of %d mods were installed or updated.\n", installed, total);
            }
        }

        config.Save();
        return false;
    }

    invisible_mode = false;
    return true;
}

MainWindow::~MainWindow()
{
    delete ui;
}

std::string MainWindow::GetInstalledModsPath()
{
    static std::string pre_computed;

    if (pre_computed.length() != 0)
        return pre_computed;

    pre_computed = Utils::MakePathString(Utils::QStringToStdString(config.game_directory), INSTALLED_MODS_PATH_NEW);
    return pre_computed;
}

bool MainWindow::ProcessShutdown()
{
    config.Save();
    return true;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (ProcessShutdown())
    {
        event->accept();
    }
    else
        event->ignore();
}

void MainWindow::UpdateStatus()
{
    int num_installed = installed_mods.size();

    if (num_installed == 0)
    {
        statusLabel->setText("No mods installed.");
    }
    else if (num_installed == 1)
    {
        statusLabel->setText("1 mod installed.");
    }
    else
    {
        statusLabel->setText(QString("%1 mods installed.").arg(num_installed));
    }
}

bool MainWindow::CheckCharaSele()
{
    return true;
    /*std::string text;

    if (!Utils::ReadTextFile("Internal/CharaSele/action_script/CharaSele.as", text))
    {
        DPRINTF("The Internal directory of the program was not decoration. Why did you delete it?\n");
        return false;
    }

    size_t pos = text.find("public static const CharacterMax:int");
    if (pos != std::string::npos)
    {
        pos = text.find('=', pos);

        if (pos != std::string::npos)
        {
            uint32_t num = std::stoi(text.substr(pos+1));
            pos = text.find("ReceiveType_JoyConSingleFlag");

            if (num != XV2_MAX_SLOTS || pos == std::string::npos)
            {
                DPRINTF("The Internal folder next to the exes is from a different version. Use the one that came with this version.\n");
                return false;
            }

            return true;
        }
    }

    DPRINTF("File CharaSele.as in Internal/CharaSele/action_script is corrupted. Re-extract the installer.\n");
    return false;*/
}

void MainWindow::CheckRegistryAssociation()
{
    QString app_path = qApp->applicationFilePath();

    // *Cough* Disable this while developing
    if (app_path.startsWith("c:/msys/home/", Qt::CaseInsensitive))
        return;

    std::string path;
    if (!Utils::GetRegistryValueString(HKEY_CURRENT_USER, "Software/Classes/X2MMod/shell/open/command/", path))
        return;

    if (!Utils::EndsWith(path, "\"%1\""))
        return;

    path = path.substr(0, path.length()-4);
    Utils::TrimString(path);

    QFileInfo f1(Utils::StdStringToQString(path));
    QFileInfo f2(app_path);

    if (f1 == f2)
        return;

    QMessageBox box(this);
    QPushButton *yes;
    //QPushButton *no;

    box.setText("XV2 Installer has noticed a change in the installer path with respect to the x2m association.\n\n"
                ".x2m extension need to be registered again. Do you want to do it now?");

    yes = box.addButton("Yes", QMessageBox::YesRole);
    yes->setIcon(this->style()->standardIcon(QStyle::SP_VistaShield));
    //no = box.addButton("No", QMessageBox::NoRole);
    box.setDefaultButton(yes);

    box.exec();

    QAbstractButton *clicked = box.clickedButton();

    if (clicked != yes)
        return;

    on_actionAssociate_x2m_extension_triggered();
}

void MainWindow::ModToGui(MainWindow::ModEntry &entry)
{
    QTreeWidgetItem *item = entry.item;

    item->setText(COLUMN_NAME, entry.name);
    item->setText(COLUMN_AUTHOR, entry.author);
    item->setText(COLUMN_VERSION, QString("%1").arg(entry.version));

    QString type;

    if (entry.type == X2mType::NEW_CHARACTER)
    {
        type = "Character";
    }
    else if (entry.type == X2mType::NEW_SKILL)
    {
        type = "Skill";
    }
    else if (entry.type == X2mType::NEW_COSTUME)
    {
        type = "Costume";
    }
    else if (entry.type == X2mType::NEW_STAGE)
    {
        type = "Stage";
    }
    else if (entry.type == X2mType::NEW_QUEST)
    {
        type = "Quest";
    }
    else if (entry.type == X2mType::NEW_SUPERSOUL)
    {
        type = "Super Soul";
    }
    else
    {
        type = "Other";
    }

    item->setText(COLUMN_TYPE, type);
    //item->setText(COLUMN_DATE, QDateTime::fromTime_t(entry.last_modified).toString(Qt::TextDate));
    item->setText(COLUMN_DATE, QDateTime::fromSecsSinceEpoch(entry.last_modified).toString(Qt::ISODate).replace("T", " "));
}

MainWindow::ModEntry *MainWindow::GetSelectedMod()
{
    if (ui->modsList->topLevelItemCount() == 0)
        return nullptr;

    QTreeWidgetItem *item = ui->modsList->currentItem();
    if (!item)
        return nullptr;

    int index = item->data(0, Qt::UserRole).toInt();

    if (index < 0 || index >= installed_mods.size())
        return nullptr;

    return &installed_mods[index];
}

size_t MainWindow::GetSelectedMods(std::vector<int> &mods)
{
    mods.clear();

    QList<QTreeWidgetItem *> selection = ui->modsList->selectedItems();
    for (QTreeWidgetItem *item : selection)
    {
        int index = item->data(0, Qt::UserRole).toInt();

        if (index >= 0 && index < installed_mods.size())
        {
            mods.push_back(index);
        }
    }

    return mods.size();
}

void MainWindow::AddModToQuestCompiler(X2mFile &x2m, Xv2QuestCompiler &qc, const std::string &path)
{
    XQ_X2mMod mod;

    mod.path = path;
    mod.name = x2m.GetModName();
    mod.type = x2m.GetType();

    if (mod.type == X2mType::NEW_CHARACTER)
    {
        mod.code = x2m.GetEntryName();
        CmsEntry *cms = game_cms->FindEntryByName(mod.code);

        if (!cms)
            return;

        mod.id = cms->id;
    }
    else if (mod.type == X2mType::NEW_SKILL)
    {
        mod.code = x2m.GetSkillEntry().name;

        std::vector<CusSkill *> skills;

        if (x2m.GetSkillType() == X2mSkillType::SUPER)
        {
            game_cus->FindSuperSkillByName(mod.code, skills);
        }

        else if (x2m.GetSkillType() == X2mSkillType::ULTIMATE)
        {
            game_cus->FindUltimateSkillByName(mod.code, skills);
        }

        else if (x2m.GetSkillType() == X2mSkillType::EVASIVE)
        {
            game_cus->FindEvasiveSkillByName(mod.code, skills);
        }

        else if (x2m.GetSkillType() == X2mSkillType::BLAST)
        {
            game_cus->FindBlastSkillByName(mod.code, skills);
        }

        else if (x2m.GetSkillType() == X2mSkillType::AWAKEN)
        {
            game_cus->FindAwakenSkillByName(mod.code, skills);
        }

        mod.id = 0xFFFFFFFF;

        for (CusSkill *skill : skills)
        {
            CmsEntryXV2 *cms = dynamic_cast<CmsEntryXV2 *>(game_cms->FindEntryByID(skill->id2 / 10));
            if (!cms || cms->unk_10 != X2M_DUMMY_ID)
                continue;

            std::string ini_path = Utils::MakePathString(X2mFile::GetSkillDirectory(*skill, x2m.GetSkillType()), X2M_SKILL_INI);
            IniFile ini;
            std::string guid_str;

            if (!xv2fs->LoadFile(&ini, ini_path))
                continue;

           if (!ini.GetStringValue("General", "GUID", guid_str))
               continue;

            if (Utils::ToLowerCase(guid_str) != Utils::ToLowerCase(x2m.GetModGuid()))
                continue;

            mod.id = skill->id;
            break;
        }

        if (mod.id == 0xFFFFFFFF)
            return;
    }
    else if (mod.type == X2mType::NEW_COSTUME)
    {
        X2mCostumeEntry *costume = game_costume_file->FindCostume(x2m.GetModGuid());
        if (!costume)
            return;

        mod.costume = *costume;
    }
    else if (mod.type == X2mType::NEW_STAGE)
    {
        mod.code = x2m.GetStageDef().code;

        size_t id;
        Xv2Stage *stage = game_stage_def->GetStageByCode(mod.code, &id);

        if (!stage)
            return;

        mod.id = (uint32_t)id;
    }
    else if (mod.type == X2mType::NEW_QUEST)
    {
        // TODO
        return;
    }
    else if (mod.type == X2mType::NEW_SUPERSOUL)
    {
        X2mSuperSoul *ss = game_costume_file->FindSuperSoul(x2m.GetModGuid());
        if (!ss)
            return;

        mod.ss = *ss;
    }

    qc.PushMod(Utils::ToLowerCase(x2m.GetModGuid()), mod);
}

bool MainWindow::LoadVisitor(const std::string &path, bool, void *param)
{
    MainWindow *pthis = (MainWindow *)param;

    if (Utils::EndsWith(path, ".x2d", false))
    {
        ModEntry mod;
        X2mFile x2m;       

        if (x2m.LoadFromFile(path))
        {
            if (x2m.HasSevHL())
            {
                for (size_t i = 0; i < x2m.GetNumSevHLEntries(); i++)
                {
                    const SevEntryHL &entry = x2m.GetSevHLEntry(i);
                    SevTableEntryHL new_hl_entry;

                    new_hl_entry.char_id = x2m.GetEntryName();
                    new_hl_entry.costume_id = entry.costume_id;
                    new_hl_entry.copy_char = entry.copy_char;
                    new_hl_entry.copy_costume = entry.copy_costume;

                    pthis->sev_hl_table.push_back(new_hl_entry);
                }
            }
            else if (x2m.HasSevLL())
            {
                for (size_t i = 0; i < x2m.GetNumSevLLEntries(); i++)
                {
                    const SevEntryLL &entry = x2m.GetSevLLEntry(i);
                    SevTableEntryLL new_ll_entry;

                    new_ll_entry.char_id = x2m.GetEntryName();
                    new_ll_entry.costume_id = entry.costume_id;
                    new_ll_entry.sub_entries = entry.sub_entries;

                    pthis->sev_ll_table.push_back(new_ll_entry);
                }
            }

            if (x2m.HasTtbHL())
            {
                for (size_t i = 0; i < x2m.GetNumTtbHLEntries(); i++)
                {
                    const TtbEntryHL &entry = x2m.GetTtbHLEntry(i);
                    TtbTableEntryHL new_hl_entry;

                    new_hl_entry.char_id = x2m.GetEntryName();
                    new_hl_entry.costume_id = entry.costume_id;
                    new_hl_entry.copy_char = entry.copy_char;
                    new_hl_entry.copy_costume = entry.copy_costume;

                    pthis->ttb_hl_table.push_back(new_hl_entry);
                }
            }
            else if (x2m.HasTtbLL())
            {
                for (size_t i = 0; i < x2m.GetNumTtbLLEntries(); i++)
                {
                    const TtbEntryLL &entry = x2m.GetTtbLLEntry(i);
                    TtbTableEntryLL new_ll_entry;

                    new_ll_entry.char_id = x2m.GetEntryName();
                    new_ll_entry.entry = entry;
                    pthis->ttb_ll_table.push_back(new_ll_entry);
                }
            }

            mod.package_path = path;
            mod.index = pthis->installed_mods.size();
            mod.name = Utils::StdStringToQString(x2m.GetModName(), false);
            mod.author = Utils::StdStringToQString(x2m.GetModAuthor(), false);
            mod.version = x2m.GetModVersion();
            mod.guid = Utils::StdStringToQString(x2m.GetModGuid());
            mod.entry_name = Utils::StdStringToQString(x2m.GetEntryName());
            mod.type = x2m.GetType();

            if (!Utils::GetFileDate(path, &mod.last_modified))
                mod.last_modified = 0;

            if (x2m.GetType() == X2mType::NEW_CHARACTER)
            {
                if (x2m.HasCharaSkillDepends() || x2m.HasCharaSsDepends())
                {
                    if (x2m.HasCharaSkillDepends())
                    {
                        mod.depends = x2m.GetAllCharaSkillDepends();
                        mod.skill_sets = x2m.GetAllSkillSets();
                    }

                    if (x2m.HasCharaSsDepends())
                    {
                        std::vector<X2mDepends> &ss_dep = x2m.GetAllCharaSsDepends();
                        mod.depends.insert(mod.depends.end(), ss_dep.begin(), ss_dep.end());
                        mod.psc_entries = x2m.GetAllPscEntries();
                    }
                }

                CmsEntry *cms_entry = game_cms->FindEntryByName(x2m.GetEntryName());
                if (cms_entry)
                {
                    pthis->character_mod_map[x2m.GetModGuid()] = cms_entry->id;
                }
            }
            else if (x2m.GetType() == X2mType::NEW_SKILL)
            {
                if (x2m.HasSkillCostumeDepend())
                {
                    mod.depends.push_back(x2m.GetSkillCostumeDepend());
                    mod.skill_entry.push_back(std::pair<CusSkill, X2mSkillType>(x2m.GetSkillEntry(), x2m.GetSkillType()));
                }

                if (x2m.HasSkillCharaDepend())
                {
                    mod.depends.push_back(x2m.GetSkillCharaDepend());
                    if (mod.skill_entry.size() == 0)
                        mod.skill_entry.push_back(std::pair<CusSkill, X2mSkillType>(x2m.GetSkillEntry(), x2m.GetSkillType()));
                }
            }
            else if (x2m.GetType() == X2mType::NEW_SUPERSOUL && x2m.HasSSSkillDepend())
            {
                mod.depends.clear();
                mod.depends.push_back(x2m.GetSSSkillDepend());
            }

            mod.item = new QTreeWidgetItem();
            mod.item->setData(0, Qt::UserRole, QVariant(mod.index));
            pthis->installed_mods.push_back(mod);

            pthis->ModToGui(mod);
            pthis->ui->modsList->addTopLevelItem(mod.item);

            AddModToQuestCompiler(x2m, pthis->qc, path);

        } // if LoadFromFile
    }

    return true;
}

uint8_t *MainWindow::GetAudio(AcbFile *acb, AwbFile *awb, uint32_t cue_id, size_t *psize)
{
    bool external;
    uint32_t awb_idx = acb->CueIdToAwbIndex(cue_id, &external);

    if (awb_idx == (uint32_t)-1 || !external)
        return nullptr;

    uint64_t size;
    uint8_t *hca_buf = awb->ExtractFile(awb_idx, &size);

    *psize = (size_t)size;
    return hca_buf;
}

void MainWindow::LoadInstalledMods(const QString &restore_path)
{
    process_selection_change = false;
    installed_mods.clear();

    std::string installed_path = GetInstalledModsPath();

    if (!Utils::DirExists(installed_path))
    {
        Utils::WriteTextFile(Utils::MakePathString(installed_path, "readme.txt"),
                             "This folder contains install data for XV2 Mods Installer.\nPlease don't delete.",
                             true, true);

        std::string legacy_path = Utils::GetAppDataPath(INSTALLED_MODS_PATH_LEGACY);
        if (Utils::DirExists(legacy_path))
        {
            Utils::MoveFileOrDir(legacy_path, installed_path);
        }
    }

    if (restore_path.length() > 0)
    {
        std::string old_mods_path = Utils::MakePathString(Utils::QStringToStdString(restore_path), INSTALLED_MODS_DIR_NAME);

        if (Utils::DirExists(old_mods_path))
            Utils::CopyDir(old_mods_path, installed_path, true);
    }

    Utils::VisitDirectory(installed_path, true, false, false, LoadVisitor, this);

    UpdateStatus();
    process_selection_change = true;
    on_modsList_itemSelectionChanged();
}

bool MainWindow::ExtractQuestFiles(X2mFile &x2m, std::string &qs, std::string &cs, std::string &ds, std::string &ps, std::vector<std::string> &svec, std::vector<std::string> &sfvec)
{
    if (!x2m.ReadTextFile(Utils::MakePathString(X2M_QUEST_DIRECTORY, "quest.x2qs"), qs))
    {
        DPRINTF("Failed to read \"quest.x2qs\" in mod.\n");
        return false;
    }

    if (!x2m.ReadTextFile(Utils::MakePathString(X2M_QUEST_DIRECTORY, "chars.x2qs"), cs))
    {
        DPRINTF("Failed to read \"chars.x2qs\" in mod.\n");
        return false;
    }

    if (!x2m.ReadTextFile(Utils::MakePathString(X2M_QUEST_DIRECTORY, "dialogue.x2qs"), ds))
    {
        DPRINTF("Failed to read \"dialogue.x2qs\" in mod.\n");
        return false;
    }

    if (!x2m.ReadTextFile(Utils::MakePathString(X2M_QUEST_DIRECTORY, "positions.x2qs"), ps))
    {
        DPRINTF("Failed to read \"positions.x2qs\" in mod.\n");
        return false;
    }

    for (int i = 0; i < 4; i++)
    {
        std::string spath = Utils::MakePathString(X2M_QUEST_DIRECTORY, "script");
        std::string script;

        if (i != 0)
            spath += Utils::ToString(i);

        spath += ".x2qs";

        if (!x2m.ReadTextFile(spath, script))
        {
            if (i == 0)
            {
                DPRINTF("Failed to read \"script.x2qs\".\n");
                return false;
            }

            break;
        }

        svec.push_back(script);
        sfvec.push_back(Utils::GetFileNameString(spath));
    }

    return true;
}

MainWindow::ModEntry *MainWindow::FindModByGuid(const QString &guid)
{
    for (ModEntry &mod : installed_mods)
    {
        if (mod.guid == guid)
            return &mod;
    }

    return nullptr;
}

MainWindow::ModEntry *MainWindow::FindModByEntry(const QString &entry)
{
    for (ModEntry &mod : installed_mods)
    {
        if (mod.entry_name == entry)
            return &mod;
    }

    return nullptr;
}

bool MainWindow::CanUpdateSlots(const X2mFile &new_mod, const X2mFile &old_mod) const
{
    size_t num_slots = new_mod.GetNumSlotEntries();
    const std::string entry_name = new_mod.GetEntryName();

    if (old_mod.GetNumSlotEntries() != num_slots)
        return false;

    if (new_mod.IsInvisible() != old_mod.IsInvisible())
        return false;

    std::vector<CharaListSlotEntry *> charalist_entries;
    chara_list->FindSlotsByCode(std::string("\"") + entry_name + std::string("\""), charalist_entries);

    if (!new_mod.IsInvisible())
    {
        if (charalist_entries.size() != num_slots)
            return false;
    }
    /*else if (charalist_entries.size() > 0) // This should only happen if user manually attached a slot to an "invisible" mod
    {
        return false;
    }*/

    CmsEntryXV2 *cms_entry = dynamic_cast<CmsEntryXV2 *>(game_cms->FindEntryByName(entry_name));
    if (!cms_entry)
        return false;

    std::vector<CusSkillSet *> skill_sets;
    if (game_cus->FindSkillSetsByCharID(cms_entry->id, skill_sets) != num_slots)
        return false;

    std::vector<CsoEntry *> cso_entries;
    game_cso->FindEntriesByCharID(cms_entry->id, cso_entries);

    if (!new_mod.HasCso())
    {
        if (cso_entries.size() != 0)
            return false;
    }
    else
    {
        if (cso_entries.size() != 0 && cso_entries.size() != new_mod.GetNumCsoEntries())
            return false;
    }

    if (!new_mod.HasPsc())
    {
        std::vector<PscEntry *> psc_entries;

        if (game_psc->FindEntries(cms_entry->id, psc_entries) != 0)
            return false;
    }

    std::vector<AurCharaLink *> aur_entries;
    game_aur->FindCharaLinks(cms_entry->id, aur_entries);

    if (!new_mod.HasAur())
    {
        if (aur_entries.size() != 0)
            return false;
    }
    else
    {
        if (aur_entries.size() != 0 && aur_entries.size() != new_mod.GetNumAurEntries())
            return false;
    }

    std::vector<CmlEntry *> cml_entries;
    game_cml->FindEntriesByCharID(cms_entry->id, cml_entries);

    if (!new_mod.HasCml())
    {
        if (cml_entries.size() != 0)
            return false;
    }
    else
    {
        if (cml_entries.size() != 0 && cml_entries.size() != new_mod.GetNumCmlEntries())
            return false;
    }

    return true;
}

bool MainWindow::CanUpdateCostume(const X2mFile &new_mod, const X2mCostumeEntry *entry) const
{
    if (new_mod.GetEffectiveNumCostumePartSets() != entry->partsets.size())
        return false;

    if (new_mod.GetNumCostumeItems() != entry->idb_entries.size())
        return false;

    if (new_mod.GetNumCostumeUsedRaces() != entry->races.size())
        return false;

    for (uint8_t race : entry->races)
    {
        if (new_mod.GetNumCostumePartSets(race) == 0)
           return false;
    }

    for (uint8_t race = 0; race < X2M_CR_NUM; race++)
    {
        if (new_mod.GetNumCostumePartSets(race) == 0)
        {
            if (std::find(entry->races.begin(), entry->races.end(), race) != entry->races.end())
                return false;
        }
        else
        {
            if (std::find(entry->races.begin(), entry->races.end(), race) == entry->races.end())
                return false;
        }
    }

    for (size_t i = 0; i < entry->idb_entries.size(); i++)
    {
        uint8_t type = entry->costume_types[i];
        uint16_t idb_id = entry->idb_entries[i];

        if (type == COSTUME_TOP)
        {
            if (new_mod.GetCostumeItem(i).item_type != X2mItemType::TOP)
                return false;

            if (!game_top_idb->FindEntryByID(idb_id))
                return false;
        }
        else if (type == COSTUME_BOTTOM)
        {
            if (new_mod.GetCostumeItem(i).item_type != X2mItemType::BOTTOM)
                return false;

            if (!game_bottom_idb->FindEntryByID(idb_id))
                return false;
        }
        else if (type == COSTUME_GLOVES)
        {
            if (new_mod.GetCostumeItem(i).item_type != X2mItemType::GLOVES)
                return false;

            if (!game_gloves_idb->FindEntryByID(idb_id))
                return false;
        }
        else if (type == COSTUME_SHOES)
        {
            if (new_mod.GetCostumeItem(i).item_type != X2mItemType::SHOES)
                return false;

            if (!game_shoes_idb->FindEntryByID(idb_id))
                return false;
        }
        else
        {
            if (new_mod.GetCostumeItem(i).item_type != X2mItemType::ACCESSORY)
                return false;

            if (!game_accesory_idb->FindEntryByID(idb_id))
                return false;
        }
    }

    return true;
}

void MainWindow::ClearAttachments(std::vector<X2mFile *> &attachments)
{
    for (X2mFile * &att : attachments)
    {
        if (att)
        {
            delete att; att = nullptr;
        }
    }

    attachments.clear();
}

void MainWindow::PostProcessSkill(X2mFile *x2m)
{
    // In this function, we will assign the skill id to characters mods that linked this skill before the skill was installed
    // Also, for blast skills, assing the so called "model" to super souls

    bool commit = false;
    bool commit_talisman = false;
    bool ss_intended = (x2m->GetSkillType() == X2mSkillType::BLAST && x2m->BlastSkillSsIntended());

    uint8_t skill_guid[16];
    Utils::String2GUID(skill_guid, x2m->GetModGuid());

    for (const ModEntry &mod : installed_mods)
    {
        if (mod.depends.size() > 0 && mod.type == X2mType::NEW_CHARACTER)
        {
            uint16_t skill_id = 0xFFFF;

            for (const X2mDepends &dep : mod.depends)
            {
                if (dep.type == X2mDependsType::SKILL && memcmp(dep.guid, skill_guid, 16) == 0)
                {
                    skill_id = (uint16_t)dep.id;
                    break;
                }
            }

            if (skill_id == 0xFFFF)
                continue;

            CmsEntry *cms_entry = game_cms->FindEntryByName(Utils::QStringToStdString(mod.entry_name));
            if (!cms_entry)
                continue;

            std::vector<CusSkillSet *> skill_sets;
            game_cus->FindSkillSetsByCharID(cms_entry->id, skill_sets);

            if (skill_sets.size() != mod.skill_sets.size())
                continue;

            for (size_t i = 0; i < skill_sets.size(); i++)
            {
                CusSkillSet *game_ss = skill_sets[i];
                const CusSkillSet &orig_ss = mod.skill_sets[i];

                int min, max;

                if (x2m->GetSkillType() == X2mSkillType::SUPER)
                {
                    min = 0; max = 3;
                }
                else if (x2m->GetSkillType() == X2mSkillType::ULTIMATE)
                {
                    min = 4; max = 5;
                }
                else if (x2m->GetSkillType() == X2mSkillType::EVASIVE)
                {
                    min = max = 6;
                }
                else if (x2m->GetSkillType() == X2mSkillType::BLAST)
                {
                    min = max = 7;
                }
                else // Awaken
                {
                    min = max = 8;
                }

                for (int i = min; i <= max; i++)
                {
                    if (orig_ss.char_skills[i] == skill_id && game_ss->char_skills[i] == 0xFFFF)
                    {
                        game_ss->char_skills[i] = x2m->GetSkillEntry().id;
                        commit = true;
                    }
                }
            }
        }
        else if (mod.depends.size() > 0 && mod.type == X2mType::NEW_SUPERSOUL && ss_intended)
        {
            if (memcmp(skill_guid, mod.depends.front().guid, 16) == 0)
            {
                std::string ss_guid_str = Utils::QStringToStdString(mod.guid);
                uint8_t ss_guid[16];
                Utils::String2GUID(ss_guid, ss_guid_str);

                X2mSuperSoul *ss = x2m->FindInstalledSS(ss_guid);
                if (ss)
                {
                    CusSkill *skill = X2mFile::FindInstalledSkill(skill_guid, X2mSkillType::BLAST);
                    if (skill)
                    {
                        if (Xenoverse2::SetBlastToTalisman(ss->idb_id, skill->id2, true))
                        {
                            commit_talisman = true;
                        }
                        else
                        {
                            DPRINTF("SetBlastToTalisman failed.\n");
                        }
                    }
                }
            }
        }
    }

    if (commit && !Xenoverse2::CommitSystemFiles(false, false, false))
    {
        DPRINTF("CommitSystemFiles failed in PostProcessSkill.\n");
    }

    if (commit_talisman && !Xenoverse2::CommitIdb(false, false, true, false))
    {
        DPRINTF("CommitIdb failed in PostProcessSkill.\n");
    }
}

void MainWindow::PostProcessCostume(X2mFile *x2m)
{
    // In this function, we will assign the costume partset to skill mods that linked this costume before the costume was installed
    bool commit = false;

    uint8_t costume_guid[16];
    Utils::String2GUID(costume_guid, x2m->GetModGuid());

    for (const ModEntry &mod : installed_mods)
    {
        if (mod.depends.size() > 0 && mod.type == X2mType::NEW_SKILL)
        {
            uint16_t partset = 0xFFFF;

            for (const X2mDepends &dep : mod.depends)
            {
                if (dep.type == X2mDependsType::COSTUME && memcmp(dep.guid, costume_guid, 16) == 0)
                {
                    partset = (uint16_t)dep.id;
                    break;
                }
            }

            if (partset == 0xFFFF || mod.skill_entry.size() == 0)
                continue;

            uint8_t skill_guid[16];
            Utils::String2GUID(skill_guid, Utils::QStringToStdString(mod.guid));

            CusSkill *skill = X2mFile::FindInstalledSkill(skill_guid,  mod.skill_entry.front().second);
            if (!skill)
                continue;

            if (skill->partset == 0xFFFF)
            {
                X2mCostumeEntry *costume = x2m->FindInstalledCostume();
                if (costume && costume->partsets.size() > 0)
                {
                    skill->partset = costume->partsets.front();
                    commit = true;
                }
            }
        }
    }

    if (commit && !Xenoverse2::CommitSystemFiles(false, false, false))
    {
        DPRINTF("CommitSystemFiles failed in PostProcessCostume.\n");
    }
}

void MainWindow::PostProcessSuperSoul(X2mFile *x2m)
{
    // In this function, we will assign the supersoul id to characters mods that linked this ss before the ss was installed
    bool commit = false;

    uint8_t ss_guid[16];
    Utils::String2GUID(ss_guid, x2m->GetModGuid());

    for (const ModEntry &mod : installed_mods)
    {
        if (mod.depends.size() > 0 && mod.type == X2mType::NEW_CHARACTER)
        {
            uint16_t ss_id = 0xFFFF;

            for (const X2mDepends &dep : mod.depends)
            {
                if (dep.type == X2mDependsType::SUPERSOUL && memcmp(dep.guid, ss_guid, 16) == 0)
                {
                    ss_id = (uint16_t)dep.id;
                    break;
                }
            }

            if (ss_id == 0xFFFF)
                continue;

            CmsEntry *cms_entry = game_cms->FindEntryByName(Utils::QStringToStdString(mod.entry_name));
            if (!cms_entry)
                continue;

            std::vector<PscSpecEntry *> psc_specs;
            game_psc->FindAllSpecs(cms_entry->id, psc_specs);

            if (psc_specs.size() != (mod.psc_entries.size() * game_psc->GetNumConfigs()))
            {
                DPRINTF("%s: warning not expected number of psc entries (linking to \"%s\" cannot be done).", FUNCNAME, Utils::QStringToStdString(mod.name).c_str());
                continue;
            }

            for (size_t i = 0; i < mod.psc_entries.size(); i++)
            {
                const PscSpecEntry &orig_spec = mod.psc_entries[i];

                for (size_t c = 0; c < game_psc->GetNumConfigs(); c++)
                {
                    PscSpecEntry *game_spec = psc_specs[i + c*mod.psc_entries.size()];

                    if (orig_spec.talisman == ss_id && game_spec->talisman == 0xFFFFFFFF)
                    {
                        game_spec->talisman = x2m->GetSSItem().idb.id;
                        commit = true;                        
                    }
                }
            }
        }
    }

    if (commit && !Xenoverse2::CommitSystemFiles(false, false, false))
    {
        DPRINTF("CommitSystemFiles failed in PostProcessSkill.\n");
    }
}

void MainWindow::PostProcessCharacter(X2mFile *x2m)
{
    CmsEntry *cms_entry = game_cms->FindEntryByName(x2m->GetEntryName());
    if (!cms_entry) // Should never happen because we just installed this mod, but you never know...
        return;

    character_mod_map[x2m->GetModGuid()] = cms_entry->id;

    // Code from here is to install this character in the cus of skills that linked it, in the case the character was installed later
    bool commit = false;

    uint8_t char_guid[16];
    Utils::String2GUID(char_guid, x2m->GetModGuid());

    for (const ModEntry &mod : installed_mods)
    {
        if (mod.depends.size() > 0 && mod.type == X2mType::NEW_SKILL)
        {
            uint16_t id = 0xFFFF;

            for (const X2mDepends &dep : mod.depends)
            {
                if (dep.type == X2mDependsType::CHARACTER && memcmp(dep.guid, char_guid, 16) == 0)
                {
                    id = (uint16_t)dep.id;
                    break;
                }
            }

            if (id == 0xFFFF || mod.skill_entry.size() == 0)
                continue;

            uint8_t skill_guid[16];
            Utils::String2GUID(skill_guid, Utils::QStringToStdString(mod.guid));

            CusSkill *skill = X2mFile::FindInstalledSkill(skill_guid,  mod.skill_entry.front().second);
            if (!skill)
                continue;

            if (skill->model == 0xFFFF)
            {
                skill->model = (uint16_t)cms_entry->id;
                commit = true;
            }
        }
    }

    if (commit && !Xenoverse2::CommitSystemFiles(false, false, false))
    {
        DPRINTF("CommitSystemFiles failed in PostProcessCharacter.\n");
    }
}

static bool VerifyCostumeMsg()
{
    int lang = (global_lang >= 0) ? global_lang : XV2_LANG_ENGLISH;

    size_t num_names = game_cac_costume_names[lang]->GetNumEntries();
    size_t num_descs = game_cac_costume_descs[lang]->GetNumEntries();
    size_t num_names_acc = game_accesory_names[lang]->GetNumEntries();
    size_t num_descs_acc = game_accesory_descs[lang]->GetNumEntries();

    int ls = 0, le = XV2_LANG_NUM;
    if (global_lang >= 0)
    {
        ls = global_lang;
        le = global_lang+1;
    }

    for (int i = ls; i < le; i++)
    {
        if (game_cac_costume_names[i]->GetNumEntries() != num_names)
        {
            DPRINTF("Cannot install costume x2m: this functionality requires that the proper_noun_costume_name_* files of all languages to be synchronized in number of entries.\n");
            return false;
        }

        if (game_cac_costume_descs[i]->GetNumEntries() != num_descs)
        {
            DPRINTF("Cannot install costume x2m: this functionality requires that the proper_noun_costume_info_* files of all languages to be synchronized in number of entries.\n");
            return false;
        }

        if (game_accesory_names[i]->GetNumEntries() != num_names_acc)
        {
            DPRINTF("Cannot install costume x2m: this functionality requires that the proper_noun_accessory_name_* files of all languages to be synchronized in number of entries.\n");
            return false;
        }

        if (game_accesory_descs[i]->GetNumEntries() != num_descs_acc)
        {
            DPRINTF("Cannot install costume x2m: this functionality requires that the proper_noun_accessory_info_* files of all languages to be synchronized in number of entries.\n");
            return false;
        }
    }

    return true;
}

static void log_empty(const char *)
{
}

static std::string compiler_output;

static void CompilerOutput(const char *dbg)
{
    compiler_output += dbg;
}

MainWindow::ModEntry *MainWindow::InstallMod(const QString &path, ModEntry *reinstall_mod, bool silent_mode, bool restore_mode)
{
    X2mFile x2m, old;
    X2mFile *x2d = nullptr;
    bool update = false;
    QWidget *parent = (invisible_mode) ? this : nullptr;
    uint32_t depends_cms = 0xFFFFFFFF; // For skills with a character link/embed

    const std::string path_std = Utils::QStringToStdString(path);

    if (!x2m.LoadFromFile(path_std))
        return nullptr;

    if (true)
    {
        const std::string mod_guid = x2m.GetModGuid();
        const std::string test_str = "ZTQyMjc2ZDgtZGEyNy1kODdmLTk1MTktNzgzZmIwNzE3Mjk1AA==";

        size_t test_size;
        uint8_t *test_buf = Utils::Base64Decode(test_str, &test_size);

        if (test_buf)
        {
            const std::string test_str = (char *)test_buf;
            delete[] test_buf;

            if (mod_guid == test_str)
            {
                const std::string test_str = "VGhpcyBtb2QgaGFzIGJlZW4gZGlzdHJpYnV0ZWQgaW4gYSBwYWNrIHdpdGhvdXQgdGhlIGF1dGhvciBwZXJtaXNzaW9uIGFuZCB3b24ndCBiZSBpbnN0YWxsZWQuIERvbid0IHN1cHBvcnQgdGhpZXZlcywgc3VwcG9ydCBvcmlnaW5hbCBtb2QgYXV0aG9ycyEA";
                uint8_t *test_buf = Utils::Base64Decode(test_str, &test_size);

                if (test_buf)
                {
                    const std::string test_str = (char *)test_buf;
                    delete[] test_buf;

                    DPRINTF("%s", test_str.c_str());
                }

                return nullptr;
            }
        }
    }

    if (x2m.HasDangerousJungle())
    {
        QString text = "This mod have files that may affect the game system wide.\n"
                       "It could destroy/delete/affect other mods and/or even vanilla content.\n\n"
                       "Do you want to install it anyway?";

        if (QMessageBox::question(parent, "Dangerous jungle", text,
                                  QMessageBox::StandardButtons(QMessageBox::Yes|QMessageBox::No),
                                  QMessageBox::No) != QMessageBox::Yes)
        {
            return nullptr;
        }
    }

    X2mType type = x2m.GetType();
    std::vector<size_t> slot_positions;
    QString message;

    message = "Name: " + Utils::StdStringToQString(x2m.GetModName(), false) + "\n";
    message += "Author: " + Utils::StdStringToQString(x2m.GetModAuthor(), false) + "\n";
    message += "Version: " + QString("%1").arg(x2m.GetModVersion()) + "\n\n";

    ModEntry *mod;

    if (reinstall_mod)
        mod = nullptr;
    else
        mod = FindModByGuid(Utils::StdStringToQString(x2m.GetModGuid()));

    std::vector<X2mFile *> attachments;
    QString depends_message;
    std::vector<CusSkillSet> temp_skill_sets; // We only need this for chara mods with skill depends
    std::vector<PscSpecEntry> temp_psc_entries; // We only need this for chara mods with skill depends
    std::vector<CusSkill> temp_skill_entry; // We only need this for skill mods with costume depends

    if (type == X2mType::NEW_CHARACTER)
    {
        if (x2m.HasCharaSkillDepends())
        {
            temp_skill_sets = x2m.GetAllSkillSets(); // We need a copy before id-assign

            for (size_t i = 0 ; i < x2m.GetNumCharaSkillDepends(); i++)
            {
                if (x2m.CharaSkillDependsHasAttachment(i))
                {
                    const X2mDepends &dep = x2m.GetCharaSkillDepends(i);
                    X2mFile *skill_x2m = x2m.LoadCharaSkillDependsAttachment(dep.guid);

                    if (!skill_x2m)
                        continue;

                    ModEntry *skill_mod = FindModByGuid(Utils::StdStringToQString(skill_x2m->GetModGuid()));
                    if (skill_mod)
                    {
                        if (skill_mod->version >= skill_x2m->GetModVersion())
                        {
                            delete skill_x2m;
                            continue;
                        }

                        attachments.push_back(skill_x2m);
                    }
                    else
                    {
                        attachments.push_back(skill_x2m);
                    }
                }
            } // end for            
        }

        if (x2m.HasCharaSsDepends())
        {
            temp_psc_entries = x2m.GetAllPscEntries(); // We need a copy before id-assign

            for (size_t i = 0 ; i < x2m.GetNumCharaSsDepends(); i++)
            {
                if (x2m.CharaSsDependsHasAttachment(i))
                {
                    const X2mDepends &dep = x2m.GetCharaSsDepends(i);
                    X2mFile *ss_x2m = x2m.LoadCharaSsDependsAttachment(dep.guid);

                    if (!ss_x2m)
                        continue;

                    ModEntry *ss_mod = FindModByGuid(Utils::StdStringToQString(ss_x2m->GetModGuid()));
                    if (ss_mod)
                    {
                        if (ss_mod->version >= ss_x2m->GetModVersion())
                        {
                            delete ss_x2m;
                            continue;
                        }

                        attachments.push_back(ss_x2m);
                    }
                    else
                    {
                        attachments.push_back(ss_x2m);
                    }
                }
            } // end for
        }

        if (attachments.size() > 0 && !reinstall_mod)
        {
            if (attachments.size() == 1)
            {
                depends_message = "The following mod will also be installed/updated:\n";
            }
            else
            {
                depends_message = "The following mods will also be installed/updated:\n";
            }

            for (X2mFile *skill_x2m : attachments)
            {
                depends_message += "- " + Utils::StdStringToQString(skill_x2m->GetModName(), false) + " (by " + Utils::StdStringToQString(skill_x2m->GetModAuthor(), false) + ")\n";
            }
        }
    }
    else if (type == X2mType::NEW_SKILL)
    {
        if (x2m.HasSkillCostumeDepend())
        {
            temp_skill_entry.push_back(x2m.GetSkillEntry());

            if (x2m.SkillCostumeDependHasAttachment())
            {
                X2mFile *costume_x2m = x2m.LoadSkillCostumeDependAttachment();
                if (costume_x2m)
                {
                    ModEntry *costume_mod = FindModByGuid(Utils::StdStringToQString(costume_x2m->GetModGuid()));
                    if (costume_mod)
                    {
                        if (costume_mod->version >= costume_x2m->GetModVersion())
                        {
                            delete costume_x2m;
                        }
                        else
                        {
                            attachments.push_back(costume_x2m);
                        }
                    }
                    else
                    {
                        attachments.push_back(costume_x2m);
                    }
                }
            }
        }

        if (x2m.HasSkillCharaDepend())
        {
            if (temp_skill_entry.size() == 0)
                temp_skill_entry.push_back(x2m.GetSkillEntry());

            if (x2m.SkillCharaDependHasAttachment())
            {
                X2mFile *char_x2m = x2m.LoadSkillCharaDependAttachment();
                if (char_x2m)
                {
                    ModEntry *char_mod = FindModByGuid(Utils::StdStringToQString(char_x2m->GetModGuid()));
                    if (char_mod)
                    {
                        if (char_mod->version >= char_x2m->GetModVersion())
                        {
                            delete char_x2m;
                        }
                        else
                        {
                            attachments.push_back(char_x2m);
                        }
                    }
                    else
                    {
                        attachments.push_back(char_x2m);
                    }
                }
            }
        }

        if (attachments.size() != 0 && !reinstall_mod)
        {
            depends_message = (attachments.size() == 1) ? "The following mod will also be installed/updated:\n" : "The following mods will also be installed/updated:\n";

            for (X2mFile *att : attachments)
            {
                depends_message += "- " + Utils::StdStringToQString(att->GetModName(), false) + " (by " + Utils::StdStringToQString(att->GetModAuthor(), false) + ")\n";
            }
        }
    }
    else if (type == X2mType::NEW_COSTUME)
    {
        if (!VerifyCostumeMsg())
            return nullptr;
    }
    else if (x2m.GetType() == X2mType::NEW_QUEST)
    {
        if (x2m.GetQuestAttachments(attachments) > 0)
        {
            for (size_t i = 0; i < attachments.size(); i++)
            {
                X2mFile *att_x2m = attachments[i];
                //DPRINTF("%s\n", att_x2m->GetModName().c_str());
                ModEntry *existing_mod = FindModByGuid(Utils::StdStringToQString(att_x2m->GetModGuid()));

                if (existing_mod)
                {
                    if (existing_mod->version >=  att_x2m->GetModVersion())
                    {
                        delete att_x2m;
                        attachments.erase(attachments.begin()+i);
                        i--;
                        continue;
                    }
                }
            }

            if (attachments.size() > 0 && !reinstall_mod)
            {
                if (attachments.size() == 1)
                {
                    depends_message = "The following mod will also be installed/updated:\n";
                }
                else
                {
                    depends_message = "The following mods will also be installed/updated:\n";
                }

                for (X2mFile *skill_x2m : attachments)
                {
                    depends_message += "- " + Utils::StdStringToQString(skill_x2m->GetModName(), false) + " (by " + Utils::StdStringToQString(skill_x2m->GetModAuthor(), false) + ")\n";
                }
            }
        }
    }
    else if (x2m.GetType() == X2mType::NEW_SUPERSOUL)
    {
        if (x2m.HasSSSkillDepend())
        {
            if (x2m.SSSkillDependHasAttachment())
            {
                X2mFile *ss_x2m = x2m.LoadSSSkillDependAttachment();
                if (ss_x2m)
                {
                    ModEntry *ss_mod = FindModByGuid(Utils::StdStringToQString(ss_x2m->GetModGuid()));
                    if (ss_mod)
                    {
                        if (ss_mod->version >= ss_x2m->GetModVersion())
                        {
                            delete ss_x2m;
                        }
                        else
                        {
                            attachments.push_back(ss_x2m);
                        }
                    }
                    else
                    {
                        attachments.push_back(ss_x2m);
                    }
                }
            }

            if (attachments.size() != 0 && !reinstall_mod)
            {
                X2mFile *ss_x2m = attachments.front();

                depends_message = "The following mod will also be installed/updated:\n";
                depends_message += "- " + Utils::StdStringToQString(ss_x2m->GetModName(), false) + " (by " + Utils::StdStringToQString(ss_x2m->GetModAuthor(), false) + ")\n";
            }
        }
    }

    if (mod) // Update
    {
        if (!silent_mode)
        {
            QMessageBox::StandardButton default_button;

            if (x2m.GetModVersion() > mod->version)
            {
                message += "Do you want to update this mod?\n";
                default_button = QMessageBox::Yes;
            }
            else
            {
                message += "This mod is already installed. Do you want to reinstall it?\n";
                default_button = QMessageBox::No;
            }

            if (!depends_message.isEmpty())
            {
                message += "\n" + depends_message + "\n";
            }

            if (QMessageBox::question(parent, "Install", message,
                                      QMessageBox::StandardButtons(QMessageBox::Yes|QMessageBox::No),
                                      default_button) == QMessageBox::No)
            {
                ClearAttachments(attachments);
                return nullptr;
            }
        }

        update = true;

        if (!old.LoadFromFile(mod->package_path))
        {
            DPRINTF("Cannot load old mod data, needed for update/reinstall.\n");
            return nullptr;
        }

        bool needs_reinstall = false;

        if (type != old.GetType())
        {
            //DPRINTF("Update/reinstall of mod cannot proceed because new mod is of different type.\n");
            needs_reinstall = true;
        }

        else if (type == X2mType::NEW_CHARACTER)
        {
            if (old.GetEntryName() != x2m.GetEntryName())
            {
                //DPRINTF("Update/reinstall of mod cannot proceed because new mod uses different entry name.\n");
                needs_reinstall = true;
            }

            else if (!CanUpdateSlots(x2m, old))
            {
                needs_reinstall = true;
            }
        }

        else if (type == X2mType::NEW_SKILL)
        {
            if (old.GetSkillEntry().name != x2m.GetSkillEntry().name)
            {
                needs_reinstall = true;
            }
            else if (old.GetSkillType() != x2m.GetSkillType())
            {
                needs_reinstall = true;
            }
        }

        else if (type == X2mType::NEW_COSTUME)
        {
            X2mCostumeEntry *update_entry = x2m.FindInstalledCostume();

            if (update_entry && !CanUpdateCostume(x2m, update_entry))
            {
                needs_reinstall = true;
            }
        }

        else if (type == X2mType::NEW_STAGE)
        {
            if (old.GetStageDef().code != x2m.GetStageDef().code)
            {
                needs_reinstall = true;
            }
        }

        if (needs_reinstall)
        {
            //UPRINTF("This mod update will triger an internal uninstall + install.\n");

            if (!UninstallMod(*mod, false, restore_mode))
            {
                DPRINTF("Mod required internal uninstall+install, but uninstall part failed.\n");
                ClearAttachments(attachments);

                if (!restore_mode)
                {
                    DPRINTF("Program will close now.\n");                    

                    if (!invisible_mode)
                        qApp->exit();
                    else
                        exit(0);
                }

                return nullptr;
            }

            return InstallMod(path, mod, silent_mode, restore_mode);
        }
    }
    else // New install
    {
        if (type == X2mType::NEW_CHARACTER)
        {
            QString col_message;
            const std::string entry_name = x2m.GetEntryName();

            if (!reinstall_mod)
            {
                ModEntry *other_mod = FindModByEntry(Utils::StdStringToQString(entry_name));
                if (other_mod)
                {
                    /*DPRINTF("Mod collision: a mod that uses the same character code is already installed:\n"
                            "Mod name: %s\n"
                            "Mod author: %s\n"
                            "Character code: %s\n",
                            Utils::QStringToStdString(other_mod->name).c_str(),
                            Utils::QStringToStdString(other_mod->author).c_str(),
                            entry_name.c_str());

                    ClearAttachments(attachments);
                    return nullptr;*/

                    col_message = "Mod collision: a mod that uses the same character code is already installed:\n"
                              "Mod name: " + other_mod->name + "\n"
                              "Mod author: " + other_mod->author + "\n"
                              "Character code: " + other_mod->entry_name + "\n";

                }
                else
                {
                    if (game_cms->FindEntryByName(entry_name))
                    {
                        /*DPRINTF("Collision: a character that uses the same code that this one is already in the game. (%s)\n", entry_name.c_str());
                        ClearAttachments(attachments);
                        return nullptr;*/
                        col_message = "Collision: a character that uses the same code that this one is already in the game "
                                  "(" + Utils::StdStringToQString(entry_name) + ")\n";
                    }
                }
            }
            else
            {
                if (game_cms->FindEntryByName(entry_name))
                {
                    col_message = "Collision: a character that uses the same code that this one is already in the game "
                              "(" + Utils::StdStringToQString(entry_name) + ")\n";
                }
            } // End reinstall_mod

            if (!col_message.isEmpty())
            {
                if (restore_mode)
                {
                    DPRINTF("COLLISION of character code shouldn't happen in restore mode.");
                    return nullptr;
                }

                col_message += "\nXV2 Installer can fix this issue by renaming the 3-letter code of the mod you want to install.\n"
                           "(Note: This process only takes place in RAM, your x2m file will be unchaged in disk, use the Tools menu to do that)\n"
                           "Although the process works most of the times, there aren't guarantees for it to work the 100% of times.\n"
                           "\nDo you want to proceed?";

                if (QMessageBox::question(this, "Rename the mod code?", col_message,  QMessageBox::StandardButtons(QMessageBox::Yes|QMessageBox::No),
                                          QMessageBox::Yes) != QMessageBox::Yes)
                {
                    ClearAttachments(attachments);
                    return nullptr;
                }

                std::string new_entry;

                while (1)
                {
                    for (int i = 0; i < 3; i++)
                    {
                        while (1)
                        {
                            char b;

                            Utils::GetRandomData(&b, 1);

                            if ((b >= 'A' && b <= 'Z') || (b >= '0' && b <= '9'))
                            {
                                if (new_entry.size() == 0 && b == 'X')
                                    continue;

                                new_entry += b;
                                break;
                            }
                        }
                    } // end for

                    if (!Xenoverse2::IsOriginalChara(new_entry) && !game_cms->FindEntryByName(new_entry))
                        break;
                }

                if (!restore_mode)
                    redirect_uprintf(log_empty);

                bool ret = ConvertModEntry(&x2m, new_entry);

                if (!restore_mode)
                    redirect_uprintf((RedirectFunc)nullptr);

                if (!ret)
                {
                    DPRINTF("Mod rename failed.\n");
                    ClearAttachments(attachments);
                    return nullptr;
                }
            } // End collision

            QMessageBox box(parent);
            QPushButton *new_slot;
            QPushButton *existing_slot;
            QPushButton *existing_slot_each = nullptr;

            box.setWindowTitle("Install");

            int num_slots = (int) x2m.GetNumSlotEntries();

            if (!x2m.IsInvisible())
            {
                if (num_slots == 1)
                {
                    message += "Where do you want to install the mod?\n";
                    box.setText(message);

                    new_slot = box.addButton("Install to new slot", QMessageBox::YesRole);
                    existing_slot = box.addButton("Append to existing slot", QMessageBox::YesRole);

                    box.setDefaultButton(new_slot);
                    box.addButton(QMessageBox::Cancel);
                }
                else
                {
                    message += "This mod has multiple slots/costumes. Where do you want to install them?\n";
                    box.setText(message);

                    new_slot = box.addButton("Install all of them in a new slot", QMessageBox::YesRole);
                    existing_slot = box.addButton("Append all of them to an existing slot", QMessageBox::YesRole);
                    existing_slot_each = box.addButton("Select existing slot for every costume/slot", QMessageBox::YesRole);

                    box.setDefaultButton(new_slot);
                    box.addButton(QMessageBox::Cancel);
                }
            }
            else
            {
                message += "Do you want to install this mod?\n";
                box.setText(message);

                new_slot = box.addButton(QMessageBox::Ok); // Not really new slot, though...
                box.addButton(QMessageBox::Cancel);
            }

            if (!depends_message.isEmpty())
            {
                message += "\n" + depends_message + "\n";
                box.setText(message);
            }

            if (silent_mode)
            {
                if (chara_list->GetNumSlots() >= XV2_MAX_SLOTS)
                {
                    DPRINTF("Number of max slots is already at max.\n");
                    ClearAttachments(attachments);
                    return nullptr;
                }
            }
            else
            {
                box.exec();

                QAbstractButton *clicked = box.clickedButton();

                if (clicked == new_slot)
                {
                    if (!x2m.IsInvisible() && chara_list->GetNumSlots() >= XV2_MAX_SLOTS)
                    {
                        DPRINTF("Number of max slots is already at max.\n");
                        ClearAttachments(attachments);
                        return nullptr;
                    }
                }
                else if (clicked == existing_slot)
                {
                    CssDialog dialog(CssMode::SELECT, parent);

                    if (dialog.exec())
                    {
                        int result = dialog.GetResultInteger();

                        /*if (result == 0)
                        {
                            DPRINTF("That entry cannot be selected.\n");
                            return nullptr;
                        }*/

                         int free_slots = XV2_MAX_SUBSLOTS - (int)(*chara_list)[result].entries.size();

                         if (free_slots < num_slots)
                         {
                             if (free_slots == 0)
                             {
                                 DPRINTF("That slot is full. Cannot install to it.\n");
                             }
                             else
                             {
                                 DPRINTF("There are no room in that slot (free=%d)\n", free_slots);
                             }

                             ClearAttachments(attachments);
                             return nullptr;
                         }

                         slot_positions.resize(num_slots, result);
                    }
                    else
                    {
                        ClearAttachments(attachments);
                        return nullptr;
                    }
                }
                else if (num_slots > 1 && clicked == existing_slot_each)
                {
                    std::vector<int> free_slots;

                    free_slots.resize(chara_list->GetNumSlots());

                    for (size_t i = 0; i < free_slots.size(); i++)
                    {
                        free_slots[i] = XV2_MAX_SUBSLOTS - (int)(*chara_list)[i].entries.size();
                    }

                    for (int i = 0; i < num_slots; i++)
                    {
                        const X2mSlotEntry &x_entry = x2m.GetSlotEntry(i);

                        UPRINTF("Select slot for costume \"%s\"\n", x_entry.costume_name[XV2_LANG_ENGLISH].c_str());

                        CssDialog dialog(CssMode::SELECT, parent);

                        if (dialog.exec())
                        {
                            int result = dialog.GetResultInteger();

                            /*if (result == 0)
                            {
                                DPRINTF("That entry cannot be selected.\n");
                                return nullptr;
                            }*/

                            if (free_slots[result] <= 0)
                            {
                                DPRINTF("There is no room on that slot.\n");
                                ClearAttachments(attachments);
                                return nullptr;
                            }

                            free_slots[result] -= 1;
                            slot_positions.push_back(result);
                        }
                        else
                        {
                            ClearAttachments(attachments);
                            return nullptr;
                        }
                    }
                }
                else // Cancel button or close
                {
                    ClearAttachments(attachments);
                    return nullptr;
                }
            } // else non-silent mode
        }
        else // Skill, costume, stage or replacer mod
        {            
            if (type == X2mType::NEW_SKILL)
            {
                message.chop(1);

                message += "Skill type: ";

                if (x2m.GetSkillType() == X2mSkillType::SUPER)
                {
                    message += "Super";
                }
                else if (x2m.GetSkillType() == X2mSkillType::ULTIMATE)
                {
                    message += "Ultimate";
                }
                else if (x2m.GetSkillType() == X2mSkillType::EVASIVE)
                {
                    message += "Evasive";
                }
                else if (x2m.GetSkillType() == X2mSkillType::BLAST)
                {
                    message += "Blast";
                }
                else if (x2m.GetSkillType() == X2mSkillType::AWAKEN)
                {
                    message += "Awaken";
                }

                message += "\n\n";
            }
            else if (type == X2mType::NEW_STAGE)
            {
                Xv2Stage *stage = game_stage_def->GetStageByCode(x2m.GetStageDef().code);

                if (stage)
                {
                    DPRINTF("Mod collision: a mod with similar stage code is already installed.\n"
                            "(%s)\n", stage->GetName(XV2_LANG_ENGLISH).c_str());
                    return nullptr;
                }
            }

            if (!depends_message.isEmpty())
            {
                message += "\n" + depends_message + "\n";
            }

            message += "Do you want to install the following mod?\n";

            if (!silent_mode && QMessageBox::question(parent, "Install", message,
                                      QMessageBox::StandardButtons(QMessageBox::Yes|QMessageBox::No),
                                      QMessageBox::Yes) == QMessageBox::No)
            {
                ClearAttachments(attachments);
                return nullptr;
            }
        }
    }

    // Handle attachments now

    std::string temp_att_path = Utils::GetTempFileLocal(".x2m");

    for (X2mFile * &att_x2m : attachments)
    {
        bool fail = false;

        if (!att_x2m->SaveToFile(temp_att_path, false, true))
        {
            DPRINTF("Failed to install attachment mod: %s (failure at save)\n", att_x2m->GetModName().c_str());
            fail = true;
        }

        if (!fail && !InstallMod(Utils::StdStringToQString(temp_att_path), nullptr, true, restore_mode))
        {
            DPRINTF("Failed to install attachment mod: %s\n", att_x2m->GetModName().c_str());
            fail = true;
        }

        delete att_x2m;
        att_x2m = nullptr;

        Utils::RemoveFile(temp_att_path);

        if (fail)
        {
            ClearAttachments(attachments);
            return nullptr;
        }
    }

    ClearAttachments(attachments);

    if (x2m.GetType() == X2mType::NEW_SKILL && x2m.HasSkillCharaDepend())
    {
        auto it = character_mod_map.find(Utils::GUID2String(x2m.GetSkillCharaDepend().guid));
        if (it != character_mod_map.end())
            depends_cms = it->second;
    }

    //UPRINTF("Calling CreateDummyPackage");

    x2d = x2m.CreateDummyPackage();

    if (!x2d)
    {
        DPRINTF("Failed in creating dummy package (internal bug).\n");
        return nullptr;
    }

    enum
    {
        // In this phase, everything happens in ram. No need to undo anything if we fail, but program must leave
        PHASE_RAM,
        // In this phase, the charalist is saved to temporal file, and the whole charasele is compiled
        // and injected in iggy.
        // The inject happens on RAM, and the list is not saved yet in Roamign/XV2INS, so there is no need
        // to undo anything yet if we fail here.
        PHASE_COMPILE,
        // Files phase: in this phase files of character are written, and also the "jungle"
        // These phases need to be undo in case of error. Problematic case would be if "jungle" entered in conflig
        // with other mod. But again, there is no support for what mods with "jungle" can do.
        PHASE_FILE_BATTLE_PORTRAIT,
        PHASE_FILE_CHARA,
        PHASE_FILE_SKILL, /* For skills */
        PHASE_FILE_COSTUME, /* For costumes */
        PHASE_FILE_STAGE, /* For stages */
        PHASE_FILE_STAGE_LIGHTING, /* For stages */
        PHASE_FILE_JUNGLE,
        // In this phase, glonal files are commited to disk. Things can still be undone on error
        // by calling the uninstall routines and comitting again. (but chances are that what cases first
        // comits to fail, will cause the error again).
        PHASE_COMMIT_DUAL_SKILL,
        PHASE_COMMIT_SEL_PORTRAIT,
        PHASE_COMMIT_PREBAKED,
        PHASE_COMMIT_CHARA_NAMES,
        PHASE_COMMIT_CHARA_COSTUME_NAMES,
        PHASE_COMMIT_SKILL_NAMES, /* For skills */
        PHASE_COMMIT_SKILL_DESCS, /* For skills */
        PHASE_COMMIT_SKILL_HOWS, /* For skills */
        PHASE_COMMIT_SKILL_BTLHUD_TEXT, /* For skills */
        PHASE_COMMIT_SKILL_IDB, /* For skills */
        PHASE_COMMIT_COSTUME_NAMES, /* For costumes */
        PHASE_COMMIT_COSTUME_DESCS, /* For sostumes */
        PHASE_COMMIT_COSTUME_BCS, /* For costumes and skills */
        PHASE_COMMIT_COSTUME_IDB, /* For costumes */
        PHASE_COMMIT_COSTUME_DFILE, /* For costumes and skills */
        PHASE_COMMIT_STAGE_NAMES, /* For stage */
        PHASE_COMMIT_STAGE_EMB, /* For stage */
        PHASE_COMMIT_STAGE_SLOTS, /* For stage */
        PHASE_COMMIT_VFX, /* For vfx */
        PHASE_COMMIT_BGM, /* For stage */
        PHASE_COMMIT_SS_NAMES, /* For super soul */
        PHASE_COMMIT_SS_DESCS, /* For super soul */
        PHASE_COMMIT_SS_HOWS, /* For super soul */
        PHASE_COMMIT_SS_IDB, /* For super soul */
        PHASE_COMMIT_SS_DFILE, /* For super soul */
        PHASE_COMMIT_SYSTEM_FILES,
        PHASE_COMMIT_SOUND,
        PHASE_COMMIT_COMMON_DIALOGUE,
        // This phase is a bit more critical, if we fail here, we will be in an incosistent installation
        PHASE_COMMIT_CHARA_LIST,        
    };

    bool error = true;
    int phase = PHASE_RAM;

    if (type == X2mType::NEW_CHARACTER)
    {
        //UPRINTF("Calling InstallCharaName");

        if (!x2m.InstallCharaName())
        {
            DPRINTF("InstallCharaName failed.\n");
            goto out;
        }

        //UPRINTF("Calling InstallCostumeName");
        if (!x2m.InstallCostumeNames())
        {
            DPRINTF("InstallCostumeNames failed.\n");
            goto out;
        }

        if (update && slot_positions.size() != 0)
        {
            DPRINTF("%s: Internal bug.\n", FUNCNAME);
            goto out;
        }

        if (slot_positions.size() != 0)
        {
            //UPRINTF("Calling InstallSlots v1\n");

            if (!x2m.InstallSlots(slot_positions))
            {
                DPRINTF("InstallSlots on positions failed.\n");
                goto out;
            }
        }
        else
        {
            //UPRINTF("Calling InstallSlots v2, update_mode=%d\n", update);

            if (!x2m.InstallSlots(update))
            {
                DPRINTF("InstallSlots failed. update_mode=%d\n", update);
                goto out;
            }
        }

        //UPRINTF("Calling InstallCms update_mode=%d", update);

        if (!x2m.InstallCms(update))
        {
            DPRINTF("InstallCms failed.\n");
            goto out;
        }

        //UPRINTF("Calling InstallCus update_mode=%d", update);

        if (!x2m.InstallCus(update))
        {
            DPRINTF("InstallCus failed. update_mode=%d\n", update);
            goto out;
        }

        //UPRINTF("Calling InstallCso update_mode=%d", update);

        if (!x2m.InstallCso(update))
        {
            DPRINTF("InstallCso failed. update_mode=%d\n", update);
            goto out;
        }

        //UPRINTF("Calling InstallPsc");

        if (!x2m.InstallPsc())
        {
            DPRINTF("InstallPsc failed.\n");
            goto out;
        }

        //UPRINTF("Calling InstallAur update_mode=%d", update);

        if (!x2m.InstallAur(update))
        {
            DPRINTF("InstallAur failed. update_mode=%d\n", update);
            goto out;
        }

        //UPRINTF("Calling InstallSev");

        if (!x2m.InstallSevAudio())
        {
            DPRINTF("InstallSevAudio failed.\n");
            goto out;
        }

        // Update x2d sev entries to make them "uncustom", needed for global table rebuilds
        x2d->CopySevLL(x2m);

        if (!x2m.InstallSev(sev_hl_table, sev_ll_table))
        {
            DPRINTF("InstallSev failed.\n");
            goto out;
        }

        //UPRINTF("Calling InstallCml update_mode=%d", update);

        if (!x2m.InstallCml(update))
        {
            DPRINTF("InstallCml failed. update_mode=%d\n", update);
            goto out;
        }

        //UPRINTF("Calling InstallHci");

        if (!x2m.InstallHci())
        {
            DPRINTF("InstallHci failed.\n");
            goto out;
        }

        // DPRINTF("Calling InstallTtb");

        if (!x2m.InstallTtbAudio())
        {
            DPRINTF("InstallTtbAudio failed.\n");
            goto out;
        }

        // Update x2d ttb entries to make them "uncustom", needed for global table rebuilds
        x2d->CopyTtbLL(x2m);

        if (!x2m.InstallTtbSubtitles())
        {
            DPRINTF("InstallTtbSubtitles failed.\n");
            goto out;
        }

        if (!x2m.InstallTtb(ttb_hl_table, ttb_ll_table))
        {
            DPRINTF("InstallTtb failed.\n");
            goto out;
        }

        // DPRINTF("Calling InstallTtc");

        if (!x2m.InstallTtc())
        {
            DPRINTF("InstallTtc failed.\n");
            goto out;
        }

        // DPRINTF("Calling InstallCnc");

        if (!x2m.InstallCnc())
        {
            DPRINTF("InstallCnc failed.\n");
            goto out;
        }

        if (!x2m.InstallCharVfx())
        {
            DPRINTF("InstallCharVfx failed.\n");
            goto out;
        }

        // DPRINTF("Calling InstallIkd");

        if (!x2m.InstallIkd())
        {
            DPRINTF("InstallIkd failed.\n");
            goto out;
        }

        // DPRINTF("Calling InstallVlc");

        if (!x2m.InstallVlc())
        {
            DPRINTF("InstallVlc failed.\n");
            goto out;
        }

        //UPRINTF("Calling InstallSelPortrait");

        if (!x2m.InstallSelPortrait())
        {
            DPRINTF("InstallSelPortrait failed.\n");
            goto out;
        }

        //UPRINTF("Calling InstallPreBaked");

        if (!x2m.InstallPreBaked())
        {
            DPRINTF("InstallPreBaked failed.\n");
            goto out;
        }

        phase = PHASE_COMPILE;

        //UPRINTF("Calling CompileCharaSel");

        /*if (!Xenoverse2::CompileCharaSel(Utils::QStringToStdString(config.flex_path)))
        {
            DPRINTF("CompileCharaSel failed.\n");
            goto out;
        }*/

        phase = PHASE_FILE_BATTLE_PORTRAIT;

        //UPRINTF("Calling InstallBtlPortrait");

        if (!x2m.InstallBtlPortrait(update))
        {
            DPRINTF("InstallBtlPortrait failed.\n"
                    "\nTHIS IS USUALLY CAUSED BY A NON WRITABLE GAME DIRECTORY\n"
                    "EITHER FIX YOUR PERMISSIONS, OR REINSTALL THE GAME IN OTHER PLACE OR RUN THE PROGRAM ALWAYS AS ADMIN.\n"
                    "AND ALSO, TELL YOUR FUCKING CRACKERS TO STOP MAKING INSTALLERS THAT DEFAULT TO NON-WRITABLE DIRECTORIES.\n");
            goto out;
        }

        phase = PHASE_FILE_CHARA;

        if (update)
        {
            //UPRINTF("Calling UninstallCharaFiles");
            old.UninstallCharaFiles(false);
        }

        //UPRINTF("Calling InstallCharaFiles");

        if (!x2m.InstallCharaFiles())
        {
            DPRINTF("InstallCharaFiles failed.\n");
            goto out;
        }

        phase = PHASE_FILE_JUNGLE;
    }
    else if (type == X2mType::NEW_SKILL)
    {
        update = x2m.FindInstalledSkill();

        // Pup must go before cus, as it will assign the pup id to the cus skill entry too
        if (!x2m.InstallPupSkill())
        {
            DPRINTF("InstallPupSkill failed.\n");
            goto out;
        }

        // Aura must go before cus
        if (!x2m.InstallAuraSkill())
        {
            DPRINTF("InstallAuraSkill failed.\n");
            goto out;
        }

        if (!x2m.InstallCusSkill(depends_cms))
        {
            DPRINTF("InstallCusSkill failed.\n");
            goto out;
        }

        if (!x2m.InstallSkillName())
        {
            DPRINTF("InstallSkillName failed.\n");
            goto out;
        }

        if (!x2m.InstallSkillDesc())
        {
            DPRINTF("InstallSkillDesc failed.\n");
            goto out;
        }

        if (!x2m.InstallSkillHow())
        {
            DPRINTF("InstallSkillHow failed.\n");
            goto out;
        }

        if (!x2m.InstallIdbSkill())
        {
            DPRINTF("InstallIdbSkill failed.\n");
            goto out;
        }

        if (!x2m.InstallSkillBodies())
        {
            DPRINTF("InstallSkillBodies failed.\n");
            goto out;
        }

        phase = PHASE_FILE_SKILL;

        if (update)
        {
            x2m.UninstallSkillFiles(false);
        }

        if (!x2m.InstallSkillFiles())
        {
            DPRINTF("InstallSkillFiles failed.\n");
            goto out;
        }

        phase = PHASE_FILE_JUNGLE;
    }
    else if (type == X2mType::NEW_COSTUME)
    {
        update = (x2m.FindInstalledCostume() != nullptr);

        if (!x2m.InstallCostumePartSets())
        {
            DPRINTF("InstallCostumePartSets failed.\n");
            goto out;
        }

        if (!x2m.InstallCostumeCostumeNames())
        {
            DPRINTF("InstalCostumeCostumeNames failed.\n");
            goto out;
        }

        if (!x2m.InstallCostumeAccessoryNames())
        {
            DPRINTF("InstallCostumeAccessoryNames failed.\n");
            goto out;
        }

        if (!x2m.InstallCostumeCostumeDescs())
        {
            DPRINTF("InstallCostumeCostumeDescs failed.\n");
            goto out;
        }

        if (!x2m.InstallCostumeAccessoryDescs())
        {
            DPRINTF("InstallCostumeAccessoryDescs failed.\n");
            goto out;
        }

        if (!x2m.InstallCostumeIdb())
        {
            DPRINTF("InstallCostumeIdb failed.\n");
            goto out;
        }

        if (!x2m.InstallCostumeFile())
        {
            DPRINTF("InstallCostumeFile failed.\n");
            goto out;
        }

        phase = PHASE_FILE_COSTUME;

        if (update)
        {
            x2m.UninstallCostumeFiles();
        }

        if (!x2m.InstallCostumeFiles())
        {
            DPRINTF("InstallCostumeFiles failed.\n");
            goto out;
        }

        phase = PHASE_FILE_JUNGLE;
    }
    else if (type == X2mType::NEW_STAGE)
    {
        update = (game_stage_def->GetStageByCode(x2m.GetStageDef().code) != nullptr);

        if (!x2m.InstallStageDef())
        {
            DPRINTF("InstallStageDef failed.\n");
            goto out;
        }

        if (!x2m.InstallStageName())
        {
            DPRINTF("InstallStageName failed.\n");
            goto out;
        }

        if (!x2m.InstallStageSelImages())
        {
            DPRINTF("InstallStageSelImages failed.\n");
            goto out;
        }

        if (!x2m.InstallStageQstPortrait())
        {
            DPRINTF("InstallStageQstPortrait failed.\n");
            goto out;
        }

        if (!x2m.InstallStageSlot())
        {
            DPRINTF("InstallStageSlot failed.\n");
            goto out;
        }

        if (!x2m.InstallStageVfx())
        {
            DPRINTF("InstallStageVfx failed.\n");
            goto out;
        }

        if (!x2m.InstallStageBgm())
        {
            DPRINTF("InstallStageBgm failed.\n");
            goto out;
        }

        phase = PHASE_FILE_STAGE;

        if (update)
        {
            old.UninstallStageFiles();
        }

        if (!x2m.InstallStageFiles())
        {
            DPRINTF("InstallStageFiles failed.\n");
            goto out;
        }

        phase = PHASE_FILE_STAGE_LIGHTING;

        if (!x2m.InstallStageLighting())
        {
            DPRINTF("InstallStageLighting failed.\n");
            goto out;
        }

        phase = PHASE_FILE_JUNGLE;
    }
    else if (type == X2mType::NEW_QUEST)
    {
        std::string qs, cs, ds, ps;
        std::vector<std::string> svec, sfvec;

        if (!ExtractQuestFiles(x2m, qs, cs, ds, ps, svec, sfvec))
        {
            goto out;
        }

        qc.SetResourceLoad("", &x2m);
        qc.SetTestMode(true);

        compiler_output.clear();

        if (!restore_mode)
            redirect_dprintf(CompilerOutput);

        bool ret = qc.CompileQuest(qs, "quest.x2qs", cs, "chars.x2qs", ds, "dialogue.x2qs", ps, "positions.x2qs", svec, sfvec);

        if (!restore_mode)
            redirect_dprintf((RedirectFunc)nullptr);

        if (!ret)
        {
            if (compiler_output.length() > 0)
            {
                DPRINTF("%s", compiler_output.c_str());
            }

            DPRINTF("Compilation of quest failed!\n");
            goto out;
        }

        std::string quest_name = qc.GetCompiledQuestName();
        if (!Utils::BeginsWith(quest_name, "TMQ_", false) && !Utils::BeginsWith(quest_name, "HLQ_", false))
        {
            DPRINTF("The quest is not a parallel (TMQ) or expert (HLQ) one. X2m can only support these types.\n");
            goto out;
        }

        if (qc.IsVanilla())
        {
            DPRINTF("\"%s\" is the name of a vanilla quest. You must specify a Quest name that doesn't match any vanilla quest.\n", quest_name.c_str());
            goto out;
        }

        if (quest_name.length() > MAX_NEW_QUEST_NAME)
        {
            DPRINTF("The name of the quest object is too big (%Id characters). Installer only allows a max of 12 characters for the quest object name.\n", quest_name.length());
            goto out;
        }

        qc.SetTestMode(false);

        if (!qc.CompileQuest(qs, "quest.x2qs", cs, "chars.x2qs", ds, "dialogue.x2qs", ps, "positions.x2qs", svec, sfvec))
        {
            DPRINTF("Quest installation failed!!!\n");
            goto out;
        }

        phase = PHASE_FILE_JUNGLE;
    }
    else if (type == X2mType::NEW_SUPERSOUL)
    {
        if (!x2m.InstallSSName())
        {
            DPRINTF("InstallSSName failed.\n");
            goto out;
        }

        if (!x2m.InstallSSDesc())
        {
            DPRINTF("InstallSSDesc failed.\n");
            goto out;
        }

        if (!x2m.InstallSSHow())
        {
            DPRINTF("InstallSSHow failed.\n");
            goto out;
        }

        if (!x2m.InstallSSIdb())
        {
            DPRINTF("InstallCostumeIdb failed.\n");
            goto out;
        }

        if (!x2m.InstallSSFile())
        {
            DPRINTF("InstallSSFile failed.\n");
            goto out;
        }

        phase = PHASE_FILE_JUNGLE;
    }
    else // Replacer mod
    {
        phase = PHASE_FILE_JUNGLE;
    }

    if (x2m.JungleExists())
    {
        if (update)
        {
            //UPRINTF("Calling UninstallJungle");
            old.UninstallJungle();
        }

        //UPRINTF("Calling InstallJungle");

        if (!x2m.InstallJungle())
        {
            DPRINTF("InstallJungle failed.\n");
            goto out;
        }
    }
    else
    {
        if (update && old.JungleExists())
        {
            //UPRINTF("Calling UninstallJungle 2.\n");
            old.UninstallJungle();
        }
    }

    if (type == X2mType::NEW_CHARACTER)
    {
        phase = PHASE_COMMIT_DUAL_SKILL;

        if (!Xenoverse2::CommitDualSkill(true, false))
        {
            DPRINTF("CommitDualSkill failed.\n");
            goto out;
        }

        phase = PHASE_COMMIT_SEL_PORTRAIT;

        //UPRINTF("Calling CommitSelPort");

        if (!Xenoverse2::CommitSelPort())
        {
            DPRINTF("CommitSelPort failed.\n");
            goto out;
        }        

        phase = PHASE_COMMIT_PREBAKED;

        //UPRINTF("Calling CommitPreBaked");

        if (!Xenoverse2::CommitPreBaked())
        {
            DPRINTF("CommitPreBaked failed.\n");
            goto out;
        }

        phase = PHASE_COMMIT_CHARA_NAMES;

        //UPRINTF("Calling CommitCharaNames");

        if (!Xenoverse2::CommitCharaNames())
        {
            DPRINTF("CommitCharaNames failed.\n");
            goto out;
        }

        phase = PHASE_COMMIT_CHARA_COSTUME_NAMES;

        //UPRINTF("Calling CommitCharaCostumeNames");

        if (!Xenoverse2::CommitCharaCostumeNames())
        {
            DPRINTF("CommitCharaCostumeNames failed.\n");
            goto out;
        }

        phase = PHASE_COMMIT_VFX;

        if (!Xenoverse2::CommitVfx())
        {
            DPRINTF("CommitVfx failed.\n");
            goto out;
        }

        phase = PHASE_COMMIT_SYSTEM_FILES;

        //UPRINTF("Calling CommitSystemFiles");

        if (!Xenoverse2::CommitSystemFiles(false, true, true))
        {
            DPRINTF("CommitSystemFiles failed.\n");
            goto out;
        }

        phase = PHASE_COMMIT_SOUND;

        //UPRINTF("Calling CommitSound");

        if (!Xenoverse2::CommitSound(true, true, false, true))
        {
            DPRINTF("CommitSound failed.\n");
            goto out;
        }

        phase = PHASE_COMMIT_COMMON_DIALOGUE;

        if (!Xenoverse2::CommitCommonDialogue())
        {
            DPRINTF("CommitCommonDialogue failed.\n");
            goto out;
        }

        phase = PHASE_COMMIT_CHARA_LIST;

        //UPRINTF("Calling CommitCharaList");

        if (!Xenoverse2::CommitCharaList(true, false))
        {
            DPRINTF("CommotCharaList failed!!!!!!!!!!!!!!!!!!!\n");
            goto out;
        }
    }
    else if (type == X2mType::NEW_SKILL)
    {
        phase = PHASE_COMMIT_PREBAKED;

        if (!Xenoverse2::CommitPreBaked())
        {
            DPRINTF("CommitPreBaked failed.\n");
            goto out;
        }

        phase = PHASE_COMMIT_SKILL_NAMES;

        if (x2m.GetSkillType() != X2mSkillType::BLAST)
        {
            if (!Xenoverse2::CommitSkillNames())
            {
                DPRINTF("CommitSkillNames failed.\n");
                goto out;
            }

            phase = PHASE_COMMIT_SKILL_DESCS;

            if (!Xenoverse2::CommitSkillDescs())
            {
                DPRINTF("CommitSkillDescs failed.\n");
                goto out;
            }

            phase = PHASE_COMMIT_SKILL_HOWS;

            if (!Xenoverse2::CommitSkillHows())
            {
                DPRINTF("CommitSkillHows failed.\n");
                goto out;
            }
        }
        else
        {
            if (x2m.BlastSkillSsIntended())
            {
                if (!Xenoverse2::CommitShopText())
                {
                    DPRINTF("CommitShopText failed.\n");
                    goto out;
                }
            }
        }

        phase = PHASE_COMMIT_SKILL_BTLHUD_TEXT;

        if (!Xenoverse2::CommitBtlHudText())
        {
            DPRINTF("CommitBtlHudText failed.\n");
            goto out;
        }

        phase = PHASE_COMMIT_SKILL_IDB;

        if (!Xenoverse2::CommitIdb(false, false, false, true))
        {
            DPRINTF("CommitIdb failed.\n");
            goto out;
        }

        phase = PHASE_COMMIT_COSTUME_BCS;

        if (!Xenoverse2::CommitCac())
        {
            DPRINTF("CommitCac failed.\n");
            goto out;
        }

        phase = PHASE_COMMIT_COSTUME_DFILE;

        if (!Xenoverse2::CommitCostumeFile())
        {
            DPRINTF("CommitCostumeFile failed.\n");
            goto out;
        }

        phase = PHASE_COMMIT_SYSTEM_FILES;

        if (!Xenoverse2::CommitSystemFiles(true, false, false))
        {
            DPRINTF("CommitSystemFiles failed.\n");
            goto out;
        }
    }
    else if (type == X2mType::NEW_COSTUME)
    {
        phase = PHASE_COMMIT_COSTUME_NAMES;

        if (!Xenoverse2::CommitCacCostumeNames())
        {
            DPRINTF("CommitCacCostumeNames failed.\n");
            goto out;
        }

        phase = PHASE_COMMIT_COSTUME_DESCS;

        if (!Xenoverse2::CommitCacCostumeDescs())
        {
            DPRINTF("CommitCacCostumeDescs failed.\n");
            goto out;
        }

        phase = PHASE_COMMIT_COSTUME_BCS;

        if (!Xenoverse2::CommitCac())
        {
            DPRINTF("CommitCac failed.\n");
            goto out;
        }

        phase = PHASE_COMMIT_COSTUME_DFILE;

        if (!Xenoverse2::CommitCostumeFile())
        {
            DPRINTF("CommitCostumeFile failed.\n");
            goto out;
        }

        phase = PHASE_COMMIT_COSTUME_IDB;

        if (!Xenoverse2::CommitIdb(true, true, false, false))
        {
            DPRINTF("CommitIdb failed.\n");
            goto out;
        }

        phase = PHASE_COMMIT_COSTUME_DFILE;

        if (!Xenoverse2::CommitCostumeFile())
        {
            DPRINTF("CommitCostumeFile failed.\n");
            goto out;
        }
    }
    else if (type == X2mType::NEW_STAGE)
    {
        phase = PHASE_COMMIT_STAGE_NAMES;

        // Stage names go in def, no need to commit this
        /*if (!Xenoverse2::CommitStageNames())
        {
            DPRINTF("CommitStageNames failed.\n");
            goto out;
        }*/

        phase = PHASE_COMMIT_STAGE_EMB;

        if (!Xenoverse2::CommitStageEmb())
        {
            DPRINTF("CommitStageEmb failed.\n");
            goto out;
        }

        phase = PHASE_COMMIT_STAGE_SLOTS;

        if (!Xenoverse2::CommitStageSlots())
        {
            DPRINTF("CommitStageSlots failed.\n");
            goto out;
        }

        phase = PHASE_COMMIT_VFX;

        if (!Xenoverse2::CommitVfx())
        {
            DPRINTF("CommitVfx failed.\n");
            goto out;
        }

        phase = PHASE_COMMIT_BGM;

        if (!Xenoverse2::CommitBgm())
        {
            DPRINTF("CommitBgm failed.\n");
            goto out;
        }

        phase = PHASE_COMMIT_SYSTEM_FILES;

        if (!Xenoverse2::CommitSystemFiles(false, false, false))
        {
            DPRINTF("CommitSystemFiles failed.\n");
            goto out;
        }
    }
    else if (type == X2mType::NEW_QUEST)
    {
        if (!qc.Maintenance())
        {
            DPRINTF("Quest installation failed (at maintenance).\n");
            goto out;
        }

        if (!qc.CommitCompiledFiles())
        {
            DPRINTF("Quest installation failed (at commit regular quest files).\n");
            goto out;
        }

        if (!qc.CommitTitle())
        {
            DPRINTF("Quest installation failed (at commit title).\n");
            goto out;
        }

        if (!qc.CommitDialogue())
        {
            DPRINTF("Quest installation failed (at commit dialogue).\n");
            goto out;
        }

        if (!qc.CommitDialogueAudio())
        {
            DPRINTF("Quest installation failed (at commit audio).\n");
            goto out;
        }

        if (!qc.CommitActiveQxd())
        {
            DPRINTF("Quest installation failed (at commit qxd).\n");
            goto out;
        }

        if (!qc.CommitQsf())
        {
            DPRINTF("Quest installation failed (at commit qsf).\n");
            goto out;
        }
    }
    else if (type == X2mType::NEW_SUPERSOUL)
    {
        phase = PHASE_COMMIT_SS_NAMES;

        if (!Xenoverse2::CommitTalismanNames())
        {
            DPRINTF("CommitTalismanNames failed.\n");
            goto out;
        }

        phase = PHASE_COMMIT_SS_DESCS;

        if (!Xenoverse2::CommitTalismanDescs())
        {
            DPRINTF("CommitTalismanDescs failed.\n");
            goto out;
        }

        phase = PHASE_COMMIT_SS_HOWS;

        if (!Xenoverse2::CommitTalismanHows())
        {
            DPRINTF("CommitTalismanHows failed.\n");
            goto out;
        }

        phase = PHASE_COMMIT_SS_IDB;

        if (!Xenoverse2::CommitIdb(false, false, true, false))
        {
            DPRINTF("CommitIdb failed.\n");
            goto out;
        }

        phase = PHASE_COMMIT_SS_DFILE;

        if (!Xenoverse2::CommitCostumeFile())
        {
            DPRINTF("CommitCostumeFile failed.\n");
            goto out;
        }
    }

    //UPRINTF("[Aparently success]");

    error = false;

out:

    if (error)
    {
        bool undo_success = true;

        DPRINTF("Install failed on phase %d. Will try to undo.\n", phase);

        if (type == X2mType::NEW_CHARACTER)
        {
            if (phase >= PHASE_FILE_BATTLE_PORTRAIT)
            {
                if (!x2m.UninstallBtlPortrait())
                    undo_success = false;
            }

            if (phase >= PHASE_FILE_CHARA)
            {
                if (!x2m.UninstallCharaFiles(true))
                    undo_success = false;
            }
        }
        else if (type == X2mType::NEW_SKILL)
        {
            if (phase >= PHASE_FILE_SKILL)
            {
                if (!x2m.UninstallSkillFiles(true))
                    undo_success = false;
            }
        }
        else if (type == X2mType::NEW_COSTUME)
        {
            if (phase >= PHASE_FILE_COSTUME)
            {
                if (!x2m.UninstallCostumeFiles())
                    undo_success = false;
            }
        }
        else if (type == X2mType::NEW_STAGE)
        {
            if (phase >= PHASE_FILE_STAGE_LIGHTING)
            {
                if (!x2m.UninstallStageLighting())
                    undo_success = false;
            }

            if (phase >= PHASE_FILE_STAGE)
            {
                if (!x2m.UninstallStageFiles())
                    undo_success = false;
            }
        }

        if (x2m.JungleExists() && phase >= PHASE_FILE_JUNGLE)
        {
            if (!x2m.UninstallJungle())
                undo_success = false;
        }

        if (type == X2mType::NEW_CHARACTER)
        {            
            if (phase >= PHASE_COMMIT_DUAL_SKILL)
            {
                if (x2m.UninstallCnc())
                {
                    if (!Xenoverse2::CommitDualSkill(true, false))
                        undo_success = false;
                }
                else
                {
                    undo_success = true;
                }
            }

            if (phase >= PHASE_COMMIT_SEL_PORTRAIT)
            {
                if (x2m.UninstallSelPortrait())
                {
                    if (!Xenoverse2::CommitSelPort())
                        undo_success = false;
                }
                else
                {
                    undo_success = false;
                }
            }

            if (phase >= PHASE_COMMIT_PREBAKED)
            {
                if (x2m.UninstallPreBaked())
                {
                    if (!Xenoverse2::CommitPreBaked())
                        undo_success = false;
                }
                else
                {
                    undo_success = false;
                }
            }

            if (phase >= PHASE_COMMIT_CHARA_NAMES)
            {
                if (x2m.UninstallCharaName())
                {
                    if (!Xenoverse2::CommitCharaNames())
                        undo_success = false;
                }
                else
                {
                    undo_success = false;
                }
            }

            if (phase >= PHASE_COMMIT_CHARA_COSTUME_NAMES)
            {
                if (x2m.UninstallCostumeNames())
                {
                    if (!Xenoverse2::CommitCharaCostumeNames())
                        undo_success = false;
                }
                else
                {
                    undo_success = false;
                }
            }

            if (phase >= PHASE_COMMIT_VFX)
            {
                if (x2m.UninstallCharVfx())
                {
                    if (!Xenoverse2::CommitVfx())
                        undo_success = false;
                }
                else
                {
                    undo_success = false;
                }
            }

            if (phase >= PHASE_COMMIT_SYSTEM_FILES)
            {
                if (x2m.UninstallCms() && x2m.UninstallCus() && x2m.UninstallCso() &&  x2m.UninstallPsc() &&
                    x2m.UninstallAur() && x2m.UninstallSevAudio() && x2m.UninstallSev(sev_hl_table, sev_ll_table) &&
                    x2m.UninstallCml() && x2m.UninstallHci() && x2m.UninstallIkd() && x2m.UninstallVlc())
                {
                    if (!Xenoverse2::CommitSystemFiles(false, true, true))
                        undo_success = false;
                }
                else
                {
                    undo_success = false;
                }
            }

            if (phase >= PHASE_COMMIT_COMMON_DIALOGUE)
                undo_success = false;

            if (phase >= PHASE_COMMIT_SOUND)
                undo_success = false;
        }
        else if (type == X2mType::NEW_SKILL)
        {
            if (phase >= PHASE_COMMIT_SKILL_NAMES)
            {
                if (x2m.UninstallSkillName())
                {
                    if (x2m.GetSkillType() != X2mSkillType::BLAST)
                    {
                        if (!Xenoverse2::CommitSkillNames() || !Xenoverse2::CommitBtlHudText())
                            undo_success = false;
                    }
                    else
                    {
                        if (x2m.BlastSkillSsIntended() && !Xenoverse2::CommitShopText())
                            undo_success = false;
                    }
                }
                else
                {
                    undo_success = false;
                }
            }

            if (phase >= PHASE_COMMIT_SKILL_DESCS)
            {
                if (x2m.UninstallSkillDesc())
                {
                    if (!Xenoverse2::CommitSkillDescs())
                        undo_success = false;
                }
                else
                {
                    undo_success = false;
                }
            }

            if (phase >= PHASE_COMMIT_SKILL_HOWS)
            {
                if (x2m.UninstallSkillHow())
                {
                    if (!Xenoverse2::CommitSkillHows())
                        undo_success = false;
                }
                else
                {
                    undo_success = false;
                }
            }

            if (phase >= PHASE_COMMIT_SKILL_IDB)
            {
                if (x2m.UninstallIdbSkill())
                {
                    if (!Xenoverse2::CommitIdb(false, false, false, true))
                        undo_success = false;
                }
                else
                {
                    undo_success = false;
                }
            }

            if (phase >= PHASE_COMMIT_COSTUME_BCS)
            {
                if (x2m.UninstallSkillBodies())
                {
                    if (!Xenoverse2::CommitCac())
                        undo_success = false;
                }
                else
                {
                    undo_success = false;
                }
            }

            if (phase >= PHASE_COMMIT_COSTUME_DFILE)
            {
                if (x2m.UninstallSkillBodies())
                {
                    if (!Xenoverse2::CommitCostumeFile())
                        undo_success = false;
                }
                else
                {
                    undo_success = false;
                }
            }

            if (phase >= PHASE_COMMIT_SYSTEM_FILES)
            {
                if (x2m.UninstallCusSkill())
                {
                    if (!Xenoverse2::CommitSystemFiles(true, false, false))
                        undo_success = false;
                }
                else
                {
                    undo_success = false;
                }
            }
        }
        else if (type == X2mType::NEW_COSTUME)
        {
            if (phase >= PHASE_COMMIT_COSTUME_NAMES)
            {
                if (x2m.UninstallCostumeCostumeNames() && x2m.UninstallCostumeAccessoryNames())
                {
                    if (!Xenoverse2::CommitCacCostumeNames())
                        undo_success = false;
                }
                else
                {
                    undo_success = false;
                }
            }

            if (phase >= PHASE_COMMIT_COSTUME_DESCS)
            {
                if (x2m.UninstallCostumeCostumeDescs() && x2m.UninstallCostumeAccessoryDescs())
                {
                    if (!Xenoverse2::CommitCacCostumeDescs())
                        undo_success = false;
                }
                else
                {
                    undo_success = false;
                }
            }

            if (phase >= PHASE_COMMIT_COSTUME_BCS)
            {
                if (x2m.UninstallCostumePartSets())
                {
                    if (!Xenoverse2::CommitCac())
                        undo_success = false;
                }
                else
                {
                    undo_success = false;
                }
            }

            if (phase >= PHASE_COMMIT_COSTUME_IDB)
            {
                if (x2m.UninstallCostumeIdb())
                {
                    if (!Xenoverse2::CommitIdb(true, true, false, false))
                        undo_success = false;
                }
                else
                {
                    undo_success = false;
                }
            }

            if (phase >= PHASE_COMMIT_COSTUME_DFILE)
            {
                if (x2m.UninstallCostumeFile())
                {
                    if (!Xenoverse2::CommitCostumeFile())
                        undo_success = false;
                }
                else
                {
                    undo_success = false;
                }
            }
        }
        else if (type == X2mType::NEW_STAGE)
        {
            if (phase >= PHASE_COMMIT_STAGE_NAMES)
            {
                if (x2m.UninstallStageName())
                {
                    // Stage names go in def, no need to commit this
                    /*if (!Xenoverse2::CommitStageNames())
                        undo_success = false; */
                }
                else
                {
                    undo_success = false;
                }
            }

            if (phase >= PHASE_COMMIT_STAGE_EMB)
            {
                if (x2m.UninstallStageSelImages() && x2m.UninstallStageQstPortrait())
                {
                    if (!Xenoverse2::CommitStageEmb())
                        undo_success = false;
                }
                else
                {
                    undo_success = false;
                }
            }

            if (phase >= PHASE_COMMIT_STAGE_SLOTS)
            {
                if (x2m.UninstallStageSlot())
                {
                    if (!Xenoverse2::CommitStageSlots())
                        undo_success = false;
                }
                else
                {
                    undo_success = false;
                }
            }

            if (phase >= PHASE_COMMIT_VFX)
            {
                if (x2m.UninstallStageVfx())
                {
                    if (!Xenoverse2::CommitVfx())
                        undo_success = false;
                }
                else
                {
                    undo_success = false;
                }
            }

            if (phase >= PHASE_COMMIT_BGM)
            {
                if (x2m.UninstallStageBgm())
                {
                    if (!Xenoverse2::CommitBgm())
                        undo_success = false;
                }
                else
                {
                    undo_success = false;
                }
            }

            if (phase >= PHASE_COMMIT_SYSTEM_FILES)
            {
                if (x2m.UninstallStageDef())
                {
                    if (!Xenoverse2::CommitSystemFiles(false, false, false))
                        undo_success = false;
                }
                else
                {
                    undo_success = false;
                }
            }

        }
        else if (type == X2mType::NEW_SUPERSOUL)
        {
            if (phase >= PHASE_COMMIT_SS_NAMES)
            {
                if (x2m.UninstallSSName())
                {
                    if (!Xenoverse2::CommitTalismanNames())
                        undo_success = false;
                }
                else
                {
                    undo_success = false;
                }
            }

            if (phase >= PHASE_COMMIT_SS_DESCS)
            {
                if (x2m.UninstallSSDesc())
                {
                    if (!Xenoverse2::CommitTalismanDescs())
                        undo_success = false;
                }
                else
                {
                    undo_success = false;
                }
            }

            if (phase >= PHASE_COMMIT_SS_HOWS)
            {
                if (x2m.UninstallSSHow())
                {
                    if (!Xenoverse2::CommitTalismanHows())
                        undo_success = false;
                }
                else
                {
                    undo_success = false;
                }
            }

            if (phase >= PHASE_COMMIT_SS_IDB)
            {
                if (x2m.UninstallSSIdb())
                {
                    if (!Xenoverse2::CommitIdb(false, false, true, false))
                        undo_success = false;
                }
                else
                {
                    undo_success = false;
                }
            }

            if (phase >= PHASE_COMMIT_SS_DFILE)
            {
                if (x2m.UninstallSSFile())
                {
                    if (!Xenoverse2::CommitCostumeFile())
                        undo_success = false;
                }
                else
                {
                    undo_success = false;
                }
            }
        }// End type

        if (undo_success)
        {
            UPRINTF("Installation was undone.\n");
        }
        else
        {
            DPRINTF("Installation couldn't be undone. The game may be in an inconsistent state now.\n");
        }

        delete x2d;

        if (!restore_mode)
        {
            DPRINTF("Program will close now.\n");

            if (!invisible_mode)
                qApp->exit();
            else
                exit(0);
        }

        return nullptr;
    } // if error

    const std::string dummy_path = GetInstalledModsPath() + "/" + x2m.GetModGuid() + ".x2d";

    if (!restore_mode)
    {
        bool ret = x2d->SaveToFile(dummy_path, true, true);
        delete x2d;

        if (!ret)
        {
            DPRINTF("Install was ok, but failed in saving mod data for installer!.\n"
                    "Installer will be in an inconsistent state.\n");

            DPRINTF("Program will close now.\n");

            if (!invisible_mode)
                qApp->exit();
            else
                exit(0);

            return nullptr;
        }
    }
    else
    {
        delete x2d;
    }

    if (!mod)
    {
        if (reinstall_mod)
        {
            mod = reinstall_mod;            
        }
        else
        {
            mod = new ModEntry();
            mod->index = -1;
            mod->item = nullptr;
        }
    }
    else
    {
    }

    mod->name = Utils::StdStringToQString(x2m.GetModName(), false);
    mod->author = Utils::StdStringToQString(x2m.GetModAuthor(), false);
    mod->version = x2m.GetModVersion();
    mod->guid = Utils::StdStringToQString(x2m.GetModGuid());
    mod->entry_name = Utils::StdStringToQString(x2m.GetEntryName());
    mod->type = x2m.GetType();
    mod->package_path = dummy_path;
    mod->depends.clear();
    mod->skill_sets.clear();
    mod->skill_entry.clear();

    if (!Utils::GetFileDate(dummy_path, &mod->last_modified))
        mod->last_modified = 0;

    AddModToQuestCompiler(x2m, qc, dummy_path);

    if (x2m.GetType() == X2mType::NEW_SKILL)
    {
        PostProcessSkill(&x2m); // Process char-->skill links

        if (temp_skill_entry.size() > 0)
        {
            mod->depends.push_back(x2m.GetSkillCostumeDepend());
            mod->skill_entry.push_back(std::pair<CusSkill, X2mSkillType>(temp_skill_entry.front(), x2m.GetSkillType()));
        }
    }
    else if (x2m.GetType() == X2mType::NEW_CHARACTER)
    {
        if (temp_skill_sets.size() > 0)
        {
            mod->depends = x2m.GetAllCharaSkillDepends();
            mod->skill_sets = temp_skill_sets;
        }

        if (temp_psc_entries.size() > 0)
        {
            std::vector<X2mDepends> &ss_depends = x2m.GetAllCharaSsDepends();
            mod->depends.insert(mod->depends.end(), ss_depends.begin(), ss_depends.end());
            mod->psc_entries = temp_psc_entries;
        }

        PostProcessCharacter(&x2m); // Add character to map, and process skill->character links
    }
    else if (x2m.GetType() == X2mType::NEW_COSTUME)
    {
        PostProcessCostume(&x2m); // process skill-->costume links
    }
    else if (x2m.GetType() == X2mType::NEW_SUPERSOUL)
    {
        PostProcessSuperSoul(&x2m); // Process char->ss links

        if (x2m.HasSSSkillDepend())
        {
            mod->depends.push_back(x2m.GetSSSkillDepend());
        }
    }

    if (!XV2Patcher::SetConfigIfNeeded())
    {
        DPRINTF("Failed in setting data for xv2patcher. Mod itself is installed but it won't work.\n");
    }

    return mod;
}

void MainWindow::PostProcessCostumeUninstall(X2mFile *x2m, const X2mCostumeEntry &costume)
{
    if (costume.partsets.size() == 0)
        return;

    bool commit = false;
    uint16_t partset = costume.partsets.front();

    uint8_t costume_guid[16];
    Utils::String2GUID(costume_guid, x2m->GetModGuid());

    for (const ModEntry &mod : installed_mods)
    {
        if (mod.depends.size() > 0 && mod.type == X2mType::NEW_SKILL)
        {
            uint16_t cost_id = 0xFFFF;

            for (const X2mDepends &dep : mod.depends)
            {
                if (dep.type == X2mDependsType::COSTUME && memcmp(dep.guid, costume_guid, 16) == 0)
                {
                    cost_id = (uint16_t)dep.id;
                    break;
                }
            }

            if (cost_id == 0xFFFF || mod.skill_entry.size() == 0)
                continue;

            uint8_t skill_guid[16];
            Utils::String2GUID(skill_guid, Utils::QStringToStdString(mod.guid));

            CusSkill *skill = X2mFile::FindInstalledSkill(skill_guid,  mod.skill_entry.front().second);
            if (!skill)
                continue;

            if (skill->partset == partset)
            {
                skill->partset = 0xFFFF;
                commit = true;
            }
        }
    }

    if (commit && !Xenoverse2::CommitSystemFiles(false, false, false))
    {
        DPRINTF("CommitSystemFiles failed in PostProcessCostumeUninstall.\n");
    }
}

void MainWindow::PostProcessSkillUninstall(X2mFile *x2m)
{
    if (x2m->GetSkillType() != X2mSkillType::BLAST || !x2m->BlastSkillSsIntended())
        return;

    bool commit = false;

    uint8_t skill_guid[16];
    Utils::String2GUID(skill_guid, x2m->GetModGuid());

    for (const ModEntry &mod : installed_mods)
    {
        if (mod.depends.size() > 0 && mod.type == X2mType::NEW_SUPERSOUL && memcmp(skill_guid, mod.depends.front().guid, 16) == 0)
        {
            std::string ss_guid_str = Utils::QStringToStdString(mod.guid);
            uint8_t ss_guid[16];
            Utils::String2GUID(ss_guid, ss_guid_str);

            X2mSuperSoul *ss = x2m->FindInstalledSS(ss_guid);
            if (ss)
            {
               if (!game_talisman_idb && !Xenoverse2::InitIdb(false, false, true, false, false, false, false, false))
               {
                   DPRINTF("InitIdb failed in PostProcessSkillUninstall");
                   return;
               }

               IdbEntry *entry = game_talisman_idb->FindEntryByID(ss->idb_id);
               if (entry)
               {
                   entry->model = 0;
                   commit = true;
               }
            }
        }
    }

    if (commit && !Xenoverse2::CommitIdb(false, false, true, false))
    {
        DPRINTF("CommitIdb failed in PostProcessSkillUninstall.\n");
    }
}

void MainWindow::PostProcessCharacterUninstall(X2mFile *x2m)
{
    uint16_t model = 0;
    auto it = character_mod_map.find(x2m->GetModGuid());
    if (it == character_mod_map.end())
        return;

    model = (uint16_t)it->second;
    character_mod_map.erase(it);

    bool commit = false;

    uint8_t char_guid[16];
    Utils::String2GUID(char_guid, x2m->GetModGuid());

    for (const ModEntry &mod : installed_mods)
    {
        if (mod.depends.size() > 0 && mod.type == X2mType::NEW_SKILL)
        {
            uint16_t id = 0xFFFF;

            for (const X2mDepends &dep : mod.depends)
            {
                if (dep.type == X2mDependsType::CHARACTER && memcmp(dep.guid, char_guid, 16) == 0)
                {
                    id = (uint16_t)dep.id;
                    break;
                }
            }

            if (id == 0xFFFF || mod.skill_entry.size() == 0)
                continue;

            uint8_t skill_guid[16];
            Utils::String2GUID(skill_guid, Utils::QStringToStdString(mod.guid));

            CusSkill *skill = X2mFile::FindInstalledSkill(skill_guid,  mod.skill_entry.front().second);
            if (!skill)
                continue;

            if (skill->model == model)
            {
                skill->model = 0xFFFF;
                commit = true;
            }
        }
    }

    if (commit && !Xenoverse2::CommitSystemFiles(false, false, false))
    {
        DPRINTF("CommitSystemFiles failed in PostProcessCharacterUninstall.\n");
    }
}

bool MainWindow::UninstallMod(const MainWindow::ModEntry &mod, bool remove_empty_dir, bool restore_mode)
{
    X2mFile x2d;
    X2mType type;
    X2mCostumeEntry uninstall_cost_entry;

    if (!x2d.LoadFromFile(mod.package_path))
        return false;

    type = x2d.GetType();

    if (type == X2mType::NEW_CHARACTER)
    {
        if (!x2d.UninstallCharaName())
        {
            DPRINTF("UninstallCharaName failed.\n");
            return false;
        }

        if (!x2d.UninstallCostumeNames())
        {
            DPRINTF("UninstallCostumeNames failed.\n");
            return false;
        }

        if (!x2d.UninstallSlots())
        {
            DPRINTF("UninstallSlots failed.\n");
            return false;
        }


        if (!x2d.UninstallCms())
        {
            DPRINTF("UninstallCms failed.\n");
            return false;
        }

        if (!x2d.UninstallCus())
        {
            DPRINTF("UninstallCus failed.\n");
            return false;
        }

        if (!x2d.UninstallCso())
        {
            DPRINTF("UninstallCso failed.\n");
            return false;
        }

        if (!x2d.UninstallPsc())
        {
            DPRINTF("UninstallPsc failed.\n");
            return false;
        }

        if (!x2d.UninstallAur())
        {
            DPRINTF("UninstallAur failed.\n");
            return false;
        }

        if (!x2d.UninstallSevAudio())
        {
            DPRINTF("UninstallSevAudio failed.\n");
            return false;
        }

        if (!x2d.UninstallSev(sev_hl_table, sev_ll_table))
        {
            DPRINTF("UninstallSev failed.\n");
            return false;
        }

        if (!x2d.UninstallCml())
        {
            DPRINTF("UninstallCml failed.\n");
            return false;
        }

        if (!x2d.UninstallHci())
        {
            DPRINTF("UninstallHci failed.\n");
            return false;
        }

        if (!x2d.UninstallTtbAudio())
        {
            DPRINTF("UninstallTtbAudio failed.\n");
            return false;
        }

        if (!x2d.UninstallTtbSubtitles())
        {
            DPRINTF("UninstallTtbSubtitles failed.\n");
            return false;
        }

        if (!x2d.UninstallTtb(ttb_hl_table, ttb_ll_table))
        {
            DPRINTF("UninstallTtb failed.\n");
            return false;
        }

        if (!x2d.UninstallTtc())
        {
            DPRINTF("UninstallTtc failed.\n");
            return false;
        }

        if (!x2d.UninstallCnc())
        {
            DPRINTF("UninstallCnc failed.\n");
            return false;
        }

        if (!x2d.UninstallCharVfx())
        {
            DPRINTF("UninstallCharVfx failed.\n");
            return false;
        }

        if (!x2d.UninstallIkd())
        {
            DPRINTF("UninstallIkd failed.\n");
            return false;
        }

        if (!x2d.UninstallVlc())
        {
            DPRINTF("UninstallVlc failed.\n");
            return false;
        }

        if (!x2d.UninstallSelPortrait())
        {
            DPRINTF("UninstallSelPortrait failed.\n");
            return false;
        }

        if (!x2d.UninstallPreBaked())
        {
            DPRINTF("UninstallPreBaked failed.\n");
            return false;
        }

        /*if (!Xenoverse2::CompileCharaSel(Utils::QStringToStdString(config.flex_path)))
        {
            DPRINTF("CompileCharaSel failed (in uninstall).\n");
            return false;
        }*/

        if (!x2d.UninstallBtlPortrait())
        {
            DPRINTF("UninstallBtlPortrait failed.\n");
            return false;
        }

        if (!x2d.UninstallCharaFiles(remove_empty_dir))
        {
            DPRINTF("UninstallCharaFiles failed.\n");
            return false;
        }
    }
    else if (type == X2mType::NEW_SKILL)
    {
        if (!x2d.FindInstalledSkill())
        {
            DPRINTF("Cannot find installed skill in the system.\nDid you delete some X2M_SKILL.ini file?");
            return false;
        }        

        // Uninstall pup must be called before uninstall cus
        if (!x2d.UninstallPupSkill())
        {
            DPRINTF("UninstallPupSkill failed.\n");
            return false;
        }

        // Uninstall aura must go before cus
        if (!x2d.UninstallAuraSkill())
        {
            DPRINTF("UninstallAuraSkill failed.\n");
            return false;
        }

        if (!x2d.UninstallCusSkill())
        {
            DPRINTF("UninstallCusSkill failed.\n");
            return false;
        }

        if (!x2d.UninstallSkillName())
        {
            DPRINTF("UninstallSkillName failed.\n");
            return false;
        }

        if (!x2d.UninstallSkillDesc())
        {
            DPRINTF("UninstallSkillDesc failed.\n");
            return false;
        }

        if (!x2d.UninstallSkillHow())
        {
            DPRINTF("UninstallSkillHow failed.\n");
            return false;
        }

        if (!x2d.UninstallIdbSkill())
        {
            DPRINTF("UninstallIdbSkill failed.\n");
            return false;
        }

        if (!x2d.UninstallSkillBodies())
        {
            DPRINTF("UninstallSkillBodies failed.\n");
            return false;
        }

        if (!x2d.UninstallSkillFiles(remove_empty_dir))
        {
            DPRINTF("UninstallSkillFiles failed.\n");
            return false;
        }
    }
    else if (type == X2mType::NEW_COSTUME)
    {
        X2mCostumeEntry *temp = x2d.FindInstalledCostume();
        if (temp)
            uninstall_cost_entry = *temp; // Copy

        if (!x2d.UninstallCostumePartSets())
        {
            DPRINTF("UninstallCostumePartSets failed.\n");
            return false;
        }

        if (!x2d.UninstallCostumeCostumeNames())
        {
            DPRINTF("UninstallCostumeCostumeNames failed.\n");
            return false;
        }

        if (!x2d.UninstallCostumeAccessoryNames())
        {
            DPRINTF("UninstallCostumeAccessoryNames failed.\n");
            return false;
        }

        if (!x2d.UninstallCostumeCostumeDescs())
        {
            DPRINTF("UninstallCostumeCostumeDescs failed.\n");
            return false;
        }

        if (!x2d.UninstallCostumeAccessoryDescs())
        {
            DPRINTF("UninstallCostumeAccessoryDescs failed.\n");
            return false;
        }

        if (!x2d.UninstallCostumeIdb())
        {
            DPRINTF("UninstallCostumeIdb failed.\n");
            return false;
        }

        if (!x2d.UninstallCostumeFile())
        {
            DPRINTF("UninstallCostumeFile failed.\n");
            return false;
        }

        if (!x2d.UninstallCostumeFiles())
        {
            DPRINTF("UninstallCostumeFiles failed.\n");
            return false;
        }
    }
    else if (type == X2mType::NEW_STAGE)
    {
        if (!x2d.UninstallStageBgm())
        {
            DPRINTF("UninstallStageBgm failed.\n");
            return false;
        }

        // This must be called before UninstallStageDef

        if (!x2d.UninstallStageVfx())
        {
            DPRINTF("UninstallStageVfx failed.\n");
            return false;
        }

        if (!x2d.UninstallStageDef())
        {
            DPRINTF("UninstallStageDef failed.\n");
            return false;
        }

        if (!x2d.UninstallStageName())
        {
            DPRINTF("UninstallStageName failed.\n");
            return false;
        }

        if (!x2d.UninstallStageSelImages())
        {
            DPRINTF("UninstallStageSelImages failed.\n");
            return false;
        }

        if (!x2d.UninstallStageQstPortrait())
        {
            DPRINTF("UninstallStageQstPortrait failed.\n");
            return false;
        }

        if (!x2d.UninstallStageSlot())
        {
            DPRINTF("UninstallStageSlot failed.\n");
            return false;
        }

        if (!x2d.UninstallStageFiles())
        {
            DPRINTF("UninstallStageFiles failed.\n");
            return false;
        }

        if (!x2d.UninstallStageLighting())
        {
            DPRINTF("UninstallStageLighting failed.\n");
            return false;
        }
    }
    else if (type == X2mType::NEW_QUEST)
    {
        std::string qs, cs, ds, ps;
        std::vector<std::string> svec, sfvec;

        if (!ExtractQuestFiles(x2d, qs, cs, ds, ps, svec, sfvec))
            return false;

        qc.SetResourceLoad("", &x2d);
        qc.SetTestMode(true);

        compiler_output.clear();

        if (!restore_mode)
            redirect_dprintf(CompilerOutput);

        bool ret = qc.CompileQuest(qs, "quest.x2qs", cs, "chars.x2qs", ds, "dialogue.x2qs", ps, "positions.x2qs", svec, sfvec);

        if (!restore_mode)
            redirect_dprintf((RedirectFunc)nullptr);

        if (!ret)
        {
            if (compiler_output.length() > 0)
            {
                DPRINTF("%s", compiler_output.c_str());
            }

            DPRINTF("Compilation of quest failed!\n");
            return false;
        }

        std::string quest_name = qc.GetCompiledQuestName();
        if (!Utils::BeginsWith(quest_name, "TMQ_", false) && !Utils::BeginsWith(quest_name, "HLQ_", false))
        {
            DPRINTF("The quest is not a parallel (TMQ) or expert (HLQ) one. X2m can only support these types.\n");
            return false;
        }

        if (qc.IsVanilla())
        {
            DPRINTF("\"%s\" is the name of a vanilla quest. You must specify a Quest name that doesn't match any vanilla quest.\n", quest_name.c_str());
            return false;
        }

        if (quest_name.length() > MAX_NEW_QUEST_NAME)
        {
            DPRINTF("The name of the quest object is too big (%Id characters). Installer only allows a max of 12 characters for the quest object name.\n", quest_name.length());
            return false;
        }

        qc.SetTestMode(false);
    }
    else if (type == X2mType::NEW_SUPERSOUL)
    {
        if (!x2d.UninstallSSName())
        {
            DPRINTF("UninstallSSName failed.\n");
            return false;
        }

        if (!x2d.UninstallSSDesc())
        {
            DPRINTF("UninstallSSDesc failed.\n");
            return false;
        }

        if (!x2d.UninstallSSHow())
        {
            DPRINTF("UninstallSSHow failed.\n");
            return false;
        }

        if (!x2d.UninstallSSIdb())
        {
            DPRINTF("UninstallSSIdb failed.\n");
            return false;
        }

        if (!x2d.UninstallSSFile())
        {
            DPRINTF("UninstallSSFile failed.\n");
            return false;
        }
    }

    if (x2d.JungleExists())
    {
        if (!x2d.UninstallJungle())
        {
            DPRINTF("UninstallJungle failed.\n");
            return false;
        }
    }

    if (type == X2mType::NEW_CHARACTER)
    {
        if (!Xenoverse2::CommitDualSkill(true, false))
        {
            DPRINTF("CommitDualSkill failed (in uninstall).\n");
            return false;
        }

        if (!Xenoverse2::CommitSelPort())
        {
            DPRINTF("CommitSelPort failed (in uninstall).\n");
            return false;
        }

        if (!Xenoverse2::CommitPreBaked())
        {
            DPRINTF("CommitPreBaked failed (in uninstall).\n");
            return false;
        }

        if (!Xenoverse2::CommitCharaNames())
        {
            DPRINTF("CommitCharaNames failed (in uninstall).\n");
            return false;
        }

        if (!Xenoverse2::CommitCharaCostumeNames())
        {
            DPRINTF("CommitCharaCostumeNames failed (in uninstall).\n");
            return false;
        }

        if (!Xenoverse2::CommitSystemFiles(false, true, true))
        {
            DPRINTF("CommitSystemFiles failed (in uninstall).\n");
            return false;
        }

        if (!Xenoverse2::CommitSound(true, true, false, true))
        {
            DPRINTF("CommitSound failed (in uninstall).\n");
            return false;
        }

        if (!Xenoverse2::CommitCharaList(true, false))
        {
            DPRINTF("CommitCharaList failed!!!!!!!!!!!!!!!!!!! (in uninstall)\n");
            return false;
        }

        if (!Xenoverse2::CommitVfx())
        {
            DPRINTF("CommitVfx failed (in uninstall)\n");
            return false;
        }
    }
    else if (type == X2mType::NEW_SKILL)
    {
        if (x2d.GetSkillType() != X2mSkillType::BLAST)
        {
            if (!Xenoverse2::CommitSkillNames())
            {
                DPRINTF("CommitSkillNames failed (in uninstall).\n");
                return false;
            }

            if (!Xenoverse2::CommitSkillDescs())
            {
                DPRINTF("CommitSkillDescs failed (in uninstall).\n");
                return false;
            }

            if (!Xenoverse2::CommitSkillHows())
            {
                DPRINTF("CommitSkillHows failed (in uninstall).\n");
                return false;
            }
        }
        else
        {
            if (x2d.BlastSkillSsIntended() && !Xenoverse2::CommitShopText())
            {
                DPRINTF("CommitShopText failed (in uninstall).\n");
                return false;
            }
        }

        if (!Xenoverse2::CommitBtlHudText())
        {
            DPRINTF("CommitBtlHudText failed (in uninstall).\n");
            return false;
        }

        if (!Xenoverse2::CommitPreBaked())
        {
            DPRINTF("CommitPreBaked failed (in uninstall).\n");
            return false;
        }

        if (!Xenoverse2::CommitIdb(false, false, false, true))
        {
            DPRINTF("CommitIdb failed (in uninstall).\n");
            return false;
        }

        if (!Xenoverse2::CommitCac())
        {
            DPRINTF("CommitCac failed (in uninstall).\n");
            return false;
        }

        if (!Xenoverse2::CommitCostumeFile())
        {
            DPRINTF("CommitCostumeFile failed (in uninstall).\n");
            return false;
        }

        if (!Xenoverse2::CommitSystemFiles(true, false, false))
        {
            DPRINTF("CommitSystemFiles failed (in uninstall).\n");
            return false;
        }
    }
    else if (type == X2mType::NEW_COSTUME)
    {
        if (!Xenoverse2::CommitCacCostumeNames())
        {
            DPRINTF("CommitCacCostumeNames failed (in uninstall).\n");
            return false;
        }

        if (!Xenoverse2::CommitCacCostumeDescs())
        {
            DPRINTF("CommitCacCostumeDescs failed (in uninstall).\n");
            return false;
        }

        if (!Xenoverse2::CommitCac())
        {
            DPRINTF("CommitCac failed (in uninstall).\n");
            return false;
        }

        if (!Xenoverse2::CommitIdb(true, true, false, false))
        {
            DPRINTF("CommitIdb failed (in uninstall).\n");
            return false;
        }

        if (!Xenoverse2::CommitCostumeFile())
        {
            DPRINTF("CommitCostumeFile failed (in uninstall).\n");
            return false;
        }
    }
    else if (type == X2mType::NEW_STAGE)
    {
        // Stage names go in def, no need to commit this
        /*if (!Xenoverse2::CommitStageNames())
        {
            DPRINTF("CommitStageNames failed (in uninstall).\n");
            return false;
        }*/

        if (!Xenoverse2::CommitStageEmb())
        {
            DPRINTF("CommitStageEmb failed (in uninstall).\n");
            return false;
        }

        if (!Xenoverse2::CommitStageSlots())
        {
            DPRINTF("CommitStageSlots failed (in uninstall).\n");
            return false;
        }

        if (!Xenoverse2::CommitSystemFiles(false, false, false))
        {
            DPRINTF("CommitSystemFiles failed (in uninstall)\n");
            return false;
        }

        if (!Xenoverse2::CommitVfx())
        {
            DPRINTF("CommitVfx failed (in uninstall)\n");
            return false;
        }

        if (!Xenoverse2::CommitBgm())
        {
            DPRINTF("CommitBgm failed (in uninstall)\n");
            return false;
        }
    }
    else if (type == X2mType::NEW_QUEST)
    {
        if (!qc.UninstallCompiledQuest(remove_empty_dir))
        {
            DPRINTF("Remove of quest failed.\n");
            return false;
        }

        if (!qc.Maintenance())
        {
            DPRINTF("Quest uninstallation failed (at maintenance).\n");
            return false;
        }

        if (!qc.CommitQsf())
        {
            DPRINTF("Quest uninstallation failed (at commit qsf).\n");
            return false;
        }

        if (!qc.CommitActiveQxd())
        {
            DPRINTF("Quest uninstallation failed (at commit qxd).\n");
            return false;
        }

        if (!qc.CommitTitle())
        {
            DPRINTF("Quest uninstallation failed (at commit title).\n");
            return false;
        }

        if (!qc.CommitDialogue())
        {
            DPRINTF("Quest uninstallation failed (at commit dialogue).\n");
            return false;
        }

        if (!qc.CommitDialogueAudio())
        {
            DPRINTF("Quest uninstallation failed (at commit audio).\n");
            return false;
        }
    }
    else if (type == X2mType::NEW_SUPERSOUL)
    {
        if (!Xenoverse2::CommitTalismanNames())
        {
            DPRINTF("CommitTalismanNames failed (in uninstall).\n");
            return false;
        }

        if (!Xenoverse2::CommitTalismanDescs())
        {
            DPRINTF("CommitTalismanDescs failed (in uninstall).\n");
            return false;
        }

        if (!Xenoverse2::CommitIdb(false, false, true, false))
        {
            DPRINTF("CommitIdb failed (in uninstall).\n");
            return false;
        }

        if (!Xenoverse2::CommitCostumeFile())
        {
            DPRINTF("CommitCostumeFile failed (in uninstall).\n");
            return false;
        }

        // Psc may change (talisman references removed) so we need this
        if (!Xenoverse2::CommitSystemFiles(false, false, false))
        {
            DPRINTF("CommitSystemFiles failed (in uninstall).\n");
            return false;
        }
    }

    if (x2d.GetType() == X2mType::NEW_SKILL)
    {
        PostProcessSkillUninstall(&x2d);
    }
    else if (x2d.GetType() == X2mType::NEW_COSTUME)
    {
        PostProcessCostumeUninstall(&x2d, uninstall_cost_entry);
    }
    else if (x2d.GetType() == X2mType::NEW_CHARACTER)
    {
        PostProcessCharacterUninstall(&x2d);
    }

    qc.RemoveMod(Utils::ToLowerCase(x2d.GetModGuid()));

    if (restore_mode)
        return true;

    return Utils::RemoveFile(mod.package_path);
}

void MainWindow::RestartProgram(const QStringList &args)
{
    HANDLE handle = CreateMutexA(nullptr, FALSE, XV2INSTALLER_INSTANCE);
    if (handle)
        CloseHandle(handle);

    QProcess::startDetached(QCoreApplication::applicationFilePath(), args);
    qApp->exit();
}

void MainWindow::ToggleDarkTheme(bool update_config)
{
    if (update_config)
    {
        config.dark_theme = !config.dark_theme;
        config.Save();
    }

    static bool dark_theme = false;
    static QPalette saved_palette;

    if (!dark_theme)
    {
        saved_palette = qApp->palette();
        //DPRINTF("%s\n", qApp->style()->metaObject()->className());

        qApp->setStyle(QStyleFactory::create("Fusion"));
        QPalette palette;
        palette.setColor(QPalette::Window, QColor(53,53,53));
        palette.setColor(QPalette::WindowText, Qt::white);
        palette.setColor(QPalette::Base, QColor(15,15,15));
        palette.setColor(QPalette::AlternateBase, QColor(53,53,53));
        palette.setColor(QPalette::ToolTipBase, Qt::white);
        palette.setColor(QPalette::ToolTipText, Qt::white);
        palette.setColor(QPalette::Text, Qt::white);
        palette.setColor(QPalette::Button, QColor(53,53,53));
        palette.setColor(QPalette::ButtonText, Qt::white);
        palette.setColor(QPalette::BrightText, Qt::red);

        //palette.setColor(QPalette::Highlight, QColor(142,45,197).lighter());
        palette.setColor(QPalette::HighlightedText, Qt::black);
        palette.setColor(QPalette::Disabled, QPalette::Text, Qt::darkGray);
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::darkGray);
        qApp->setPalette(palette);

        dark_theme =true;
    }
    else
    {
        qApp->setStyle(QStyleFactory::create("windowsvista"));
        qApp->setPalette(saved_palette);
        dark_theme = false;
    }
}

static bool RenameFileToBak(const std::string &file)
{
    for (size_t i = 0; ; i++)
    {
        std::string new_file = file + ".bak";
        if (i > 0)
            new_file += Utils::ToString(i+1);

        if (!Utils::FileExists(new_file))
            return Utils::RenameFile(file, new_file);
    }

    // Unreachable code
    return false;
}

void MainWindow::ClearEpatches()
{
    static std::unordered_set<std::string> xv2patcher_xml =
    {
        "aiextend.xml",
        "aura_limit_remove.xml",
        "autogenportrait_dumper.xml",
        "bacbcm_protection.xml",
        "battle_timer.xml",
        "debug.xml",
        "freezer_event_offline.xml",
        "iggy.xml",
        "jungle.xml",
        "loose_files.xml",
        "misc.xml",
        "mob_control.xml",
        "new_chara.xml",
        "new_stages.xml",
        "prebaked_patches.xml",
        "psc_destroy_limit.xml",
        "pseudocacs.xml",
        "quests.xml",
        "ui.xml",
        "unlock_chara.xml",
        "unlock_stages.xml",
    };

    const std::string epatches_dir = Utils::MakePathString(Utils::QStringToStdString(config.game_directory), "XV2PATCHER/Epatches");
    std::vector<std::string> files;

    Utils::ListFiles(epatches_dir, true, false, false, files);
    for (const std::string &file : files)
    {
        if (Utils::EndsWith(file, ".xml", false) && xv2patcher_xml.find(Utils::ToLowerCase(Utils::GetFileNameString(file))) == xv2patcher_xml.end())
        {
            //DPRINTF("Renaming %s\n", file.c_str());
            RenameFileToBak(file);
        }
    }
}


bool MainWindow::ClearInstallation(bool special_mode)
{
    const std::string data_dir = Utils::MakePathString(Utils::QStringToStdString(config.game_directory), "data");
    const std::string xv2ins_cfg_dir = Utils::GetAppDataPath("XV2INS");

    QMessageBox box;
    QCheckBox *check = new QCheckBox("Send folders to recycle bin instead of deleting them.");
    check->setChecked(true);
    QString text;

    text = "This process will clear the mods installation by deleting the data folder and the internal XV2INS configuration folder.\n";
    text += "\nThe following folders and all its contents will be deleted:\n";
    text += QString("- %1\n").arg(Utils::StdStringToQString(data_dir));
    text += QString("- %1\n").arg(Utils::StdStringToQString(xv2ins_cfg_dir));
    text += "\nDo you want to proceed?";

    box.setText(text);
    box.setWindowTitle("Clear installation?");
    box.setIcon(QMessageBox::Icon::Warning);
    box.addButton(QMessageBox::Yes);
    box.addButton(QMessageBox::No);
    box.setDefaultButton(QMessageBox::No);

    if (!special_mode)
        box.setCheckBox(check);

    if (box.exec() != QMessageBox::Yes)
        return false;

    UPRINTF("Before proceeding, it is recommended that you don't' have the game running, and that you close any explorer window opened in the data directory.\n");

    bool success = true;

    if (Utils::RemoveDirFull(data_dir, true, !special_mode && check->isChecked(), false))
    {
        if (!Utils::RemoveDirFull(xv2ins_cfg_dir, true,  !special_mode && check->isChecked(), false))
        {
            DPRINTF("Data directory was removed but removal of XV2INS config folder failed.\n");
        }
        else
        {
            config.position = 0;
        }
    }
    else
    {
        DPRINTF("Removal of data directory failed.\n");
        success = false;
    }

    ClearEpatches();

    if (success)
    {
        UPRINTF("The mod installation has been cleared succesfully. Program will now restart automatically.\n");        
    }

    if (success)
        RestartProgram();

    return success;
}

void MainWindow::RestorePart1()
{
    const std::string data_dir = Utils::MakePathString(Utils::QStringToStdString(config.game_directory), "data");
    const std::string data_dir_old = Utils::MakePathString(Utils::QStringToStdString(config.game_directory), std::string("data") + Utils::ToString(Utils::RandomInt(100000, 999999)));

    QMessageBox box;
    QString text;

    text = "<b><font color='red'>Warning: this is an experimental/alpha festure.<br>Make sure to read everything before confirming.<br>An in case of instability in game, use Tools->Clear Installation and install from scratch.</font></b><br>";
    text += "<br>This process will attempt to port over current installation.<br>";
    text += "The process will consist in the following:<br><br>";
    text += "\t-Firstly, the data folder will be renamed to " + Utils::StdStringToQString(Utils::GetFileNameString(data_dir_old), true) + "<br><br>";
    text += "\t-Then the program will self-restart, creating a basic new data folder (clean).<br><br>";
    text += "\t-The program will try to restore all x2m mods by using a combination of the semi-dummy x2d files in InstalledMods and the old data folder.<br><br>";
    text += "\t-After that, it will port old slots (characters and stages) to new installation.<br><br>";
    text += "\t-Finally, the program will ask you if you want to delete, keep or send to recycle bin the old data folder.<br><br>";
    text += "<b>Known limitations:</b><br><br>";
    text += "\t-The tool can only restore x2m mods, anything installed outside cannot be ported over.<br>";
    text += "\t-The ID assignment will be different, which means that you may need to buy custom clothes/skills again, and new quests may appear in game in a different order.<br>";
    text += "\t-Any X2M mod with a 'JUNGLE' directory that has files outside data will fail to install.<br>";
    text += "\t-This tool cannot account for mods that install system/global files that may break new installation.<br>";
    text += "<br>Before proceeding, these warnings:<br>";
    text += "<b><font color='red'>-DO NOT CLOSE THE LOG WINDOW UNTIL THE PROCESS IS FINISHED.</font></b><br>";
    text += "<b>-Make sure to not have any programs, using the current data folder or the rename will fail.</b><br>";
    text += "<b>-If after several seconds, you cannot see a log window after clicking Yes, please try to minimize all windows.</b><br>";
    text += "<b>-You need at least the same space in disk that current data folder has.</b><br>";
    text += "<br><br>Do you want to proceed?";

    box.setText(text);
    box.setIcon(QMessageBox::Icon::Warning);
    box.addButton(QMessageBox::Yes);
    box.addButton(QMessageBox::No);
    box.setDefaultButton(QMessageBox::No);

    if (box.exec() != QMessageBox::Yes)
        return;

    if (!Utils::RenameFile(data_dir, data_dir_old))
    {
        DPRINTF("Installation restoration failed: cannot rename data directory.\n"
                "Make sure it's not in use by some program.");
        return;
    }

    ClearEpatches();
    RestartProgram( { "--restore", Utils::StdStringToQString(data_dir_old, true) });
}

#ifdef DEBUG_MEASURE_TIME
static std::map<uint32_t, std::string> itime_map;
#endif

void MainWindow::RestoreMods(const std::vector<std::string> &paths, X2mType type, const std::string &label, int &global_success, int &global_errors, std::vector<std::string> &failures)
{
    int success = 0;
    int errors = 0;

    UPRINTF("<b>Restoring %s mods...</b><br>", label.c_str());

    for (const std::string &file : paths)
    {
        if (!Utils::EndsWith(file, ".x2d", false))
            continue;

        X2mFile x2d;
        if (!x2d.LoadFromFile(file) || x2d.GetType() != type)
            continue;

        UPRINTF("Attempting to restore mod \"%s\" from file \"%s\"...", x2d.GetModName().c_str(), Utils::GetFileNameString(file).c_str());
        QApplication::processEvents();

#ifdef DEBUG_MEASURE_TIME
        uint64_t start_time = GetTickCount64();
#endif

        if (InstallMod(Utils::StdStringToQString(file, true), nullptr, true, true))
        {
#ifdef DEBUG_MEASURE_TIME
            uint32_t itime = (uint32_t)(GetTickCount64()- start_time);
            UPRINTF("Mod installed in %d milisecs", itime);

            auto it = itime_map.find(itime);
            if (it == itime_map.end())
            {
                itime_map[itime] = x2d.GetModName();
            }
            else
            {
                itime_map[itime] = itime_map[itime] + ";;;" + x2d.GetModName();
            }
#endif
            UPRINTF("<b>Success!</b><br>");
            success++;
        }
        else
        {
            failures.push_back(file);
            DPRINTF("Failure!\n");
            errors++;
        }

        QApplication::processEvents();
    }

    if (success == 0 && errors == 0)
    {
        UPRINTF("There were no %s mods.", label.c_str());
    }
    else
    {
       UPRINTF("<b>%d %s mods were restored with success%s.</b>", success, label.c_str(), (errors==0) ? " (all of them!)" : "");
       if (errors > 0)
           DPRINTF("%d %s mods failed to be restored.\n", errors, label.c_str());
    }
    UPRINTF("\n");

    QApplication::processEvents();
    global_success += success;
    global_errors += errors;
}

static size_t FindLastVanillaChar(const Xv2PatcherSlotsFile &sf, bool exclude_prb)
{
    size_t latest = (size_t)-1;

    for (size_t i = 0; i < sf.GetNumSlots(); i++)
    {
        const CharaListSlot &slot = sf.GetSlots()[i];
        if (slot.entries.size() == 0)
            continue;

        const CharaListSlotEntry &entry = slot.entries[0];
        if (entry.code.length() != 5)
            continue;

        std::string cms_entry = entry.code.substr(1, 3); // remove quotes
        if (Xenoverse2::IsOriginalChara(cms_entry))
        {
            if (!exclude_prb || entry.dlc != "PRB")
                latest = i;
        }
    }

    return latest;
}

bool MainWindow::RestoreCharaSlots(const std::string &recover_path)
{
    Xv2PatcherSlotsFile os;

    if (!os.LoadFromFile(Utils::MakePathString(recover_path, "XV2P_SLOTS.x2s")))
        return false;

    // The base will be the old slots file. And from there, we will introduce the new characters/costumes of latest update
    // First: introduce full original characters that exist in new slot, but not in old slots (e.g.: probably DLC chars)
    for (size_t i = 0; i < chara_list->GetNumSlots(); i++)
    {
        const CharaListSlot &nslot = chara_list->GetSlots()[i];
        if (nslot.entries.size() == 0)
            continue;

        if (nslot.entries[0].code.length() != 5)
            continue;

        std::string cms_entry = nslot.entries[0].code.substr(1, 3); // remove quotes
        size_t idx;

        if (Xenoverse2::IsOriginalChara(cms_entry) && !os.FindFirstMatch(nslot.entries[0].code, &idx))
        {
            size_t lv = FindLastVanillaChar(os, true);
            if (lv == (size_t)-1)
                continue;

            os.PlaceAtPos(lv+1, nslot);
        }
    }

    // Second: introduce any new costume that may have been introduced to vanilla characters
    for (size_t i = 0; i < chara_list->GetNumSlots(); i++)
    {
        const CharaListSlot &nslot = chara_list->GetSlots()[i];
        if (nslot.entries.size() == 0)
            continue;

        if (nslot.entries[0].code.length() != 5)
            continue;

        std::string cms_entry = nslot.entries[0].code.substr(1, 3); // remove quotes
        if (!Xenoverse2::IsOriginalChara(cms_entry))
            continue;

        for (size_t j = 0; j < nslot.entries.size(); j++)
        {
            const CharaListSlotEntry &nentry = nslot.entries[j];
            size_t idx, jdx;

            if (!os.FindFirstMatch(nentry.code, nentry.costume_index, nentry.model_preset, &idx, &jdx))
            {
                if (os.FindFirstMatch(nentry.code, &idx))
                {
                    os[idx].entries.push_back(nentry);
                }
            }
        }
    }

    // Now, remove any characters that don't exist (eg, those that may have failed to install)
    bool done = false;
    while (!done)
    {
        done = true;
        for (const CharaListSlot &slot : os)
        {
            if (slot.entries.size() == 0)
                continue;

            for (const CharaListSlotEntry &entry : slot.entries)
            {
                if (entry.code.length() != 5)
                    continue;

                std::string cms_entry = entry.code.substr(1, 3); // remove quotes
                if (!game_cms->FindEntryByName(cms_entry))
                {
                    //DPRINTF("Deleting %s", entry.code.c_str());
                    os.RemoveSlots(entry.code); // slot && entry won't longer be valid after this call, so we have to start over the loops
                    done = false;
                    break;
                }
            }

            if (!done)
                break;
        }
    }

    // Voice id of mod characters are messed at this point, restore them from new installation
    for (CharaListSlot &slot : os)
    {
        for (CharaListSlotEntry &entry : slot.entries)
        {
            if (entry.code.length() != 5)
                continue;

            std::string cms_entry = entry.code.substr(1, 3); // remove quotes
            if (Xenoverse2::IsOriginalChara(cms_entry))
                continue;

            size_t idx, jdx;
            if (chara_list->FindFirstMatch(entry.code, entry.costume_index, entry.model_preset, &idx, &jdx))
            {
                const CharaListSlotEntry &another = (*chara_list)[idx].entries[jdx];
                entry.voices_id_list[0] = another.voices_id_list[0];
                entry.voices_id_list[1] = another.voices_id_list[1];
            }
        }
    }

    // Commit file
    return xv2fs->SaveFile(&os, "data/XV2P_SLOTS.x2s");
}

static size_t FindLastVanillaStage(const Xv2PatcherSlotsFileStage &sf)
{
    size_t latest = (size_t)-1;

    for (size_t i = 0; i < sf.GetNumSlots(); i++)
    {
        const Xv2StageSlot &slot = sf.GetSlots()[i];
        Xv2Stage *s = game_stage_def->GetStageBySsid(slot.stage);
        if (s)
        {
            if (Xenoverse2::IsForbiddenNewStageName(s->code))
                latest = i;
        }
    }

    return latest;
}

bool MainWindow::RestoreStageSlots(const std::string &recover_path, bool local)
{
    const std::string fn = (local) ? "XV2P_SLOTS_STAGE_LOCAL.x2s" : "XV2P_SLOTS_STAGE.x2s";
    Xv2PatcherSlotsFileStage os;

    if (!os.LoadFromFile(Utils::MakePathString(recover_path, fn)))
        return false;

    static Xv2StageDefFile *old_stage_def = nullptr;
    if (old_stage_def == nullptr)
    {
        old_stage_def = new Xv2StageDefFile();
        if (!old_stage_def->CompileFromFile(Utils::MakePathString(recover_path, "xv2_stage_def.xml")))
        {
            delete old_stage_def; old_stage_def = nullptr;
            return false;
        }
    }

    // First buiild translation map, and remove invalid entries
    bool done = false;
    std::unordered_map<uint32_t, uint32_t> tr_map; // Old-> new
    while (!done)
    {
        done = true;

        for (const Xv2StageSlot &slot : os)
        {
            Xv2Stage *olds = old_stage_def->GetStageBySsid(slot.stage);
            if (!olds)
            {
                os.RemoveSlots(slot.stage);
                done = false;
                break;
            }

            Xv2Stage *news = game_stage_def->GetStageByCode(olds->code);
            if (!news || news->ssid < 0)
            {
                os.RemoveSlots(slot.stage);
                done = false;
                break;
            }

            tr_map[slot.stage] = news->ssid;
        }
    }

    // Do actual id translation
    for (Xv2StageSlot &slot : os)
    {
        auto it = tr_map.find(slot.stage);
        if (it != tr_map.end())
        {
            slot.stage = it->second;
        }
    }

    // Now insert any new vanilla stage (if any)
    for (const Xv2StageSlot &slot : *game_stage_slots_file)
    {
        Xv2Stage *s = game_stage_def->GetStageBySsid(slot.stage);
        if (!s || !Xenoverse2::IsForbiddenNewStageName(s->code))
            continue;

        std::vector<Xv2StageSlot *> ss;
        if (os.FindSlots(slot.stage, ss) == 0)
        {
            size_t lv = FindLastVanillaStage(os);
            if (lv == (size_t)-1)
                continue;

            os.PlaceAtPos(lv+1, slot);
        }
    }

    // Commit file
    return xv2fs->SaveFile(&os, Utils::MakePathString("data", fn));
}

static void CollectModsCms(const std::vector<std::string> &paths, std::unordered_set<std::string> &cms_set)
{
    // The purpose of this function is to collect cms of all mods in advance so that the x2m skill assign id cms fake entries know in advance which names not to use.
    for (const std::string &file : paths)
    {
        if (!Utils::EndsWith(file, ".x2d", false))
            continue;

        X2mFile x2d;
        if (!x2d.LoadFromFile(file) || x2d.GetType() != X2mType::NEW_CHARACTER)
            continue;

        cms_set.insert(x2d.GetEntryName());
    }
}

void MainWindow::RestorePart2(const QString &recover_path)
{    
    // Clear variables modified by the LoadVisitor
    installed_mods.clear();
    qc.RemoveAllMods();
    sev_hl_table.clear();
    sev_ll_table.clear();
    ttb_hl_table.clear();
    ttb_ll_table.clear();

    std::string rp_std = Utils::QStringToStdString(recover_path, true);

    log_dlg = new LogDialog(this);
    log_dlg->setWindowTitle("X2M Install Restoration");
    log_dlg->Resize(400, 400);
    log_dlg->show();

    redirect_uprintf(log_window);
    redirect_dprintf(log_window_error);
    UPRINTF("<b>Will try to restore installation from %s</b>", rp_std.c_str());
    DPRINTF("DO NOT CLOSE THIS WINDOW UNTIL DONE<br><br>");
    QApplication::processEvents();

    std::vector<std::string> paths;
    std::unordered_set<std::string> cms_set;

    Utils::ListFiles(GetInstalledModsPath(), true, false, false, paths);
    CollectModsCms(paths, cms_set);

    int success = 0;
    int errors = 0;

    X2mFile::SetDummyMode(rp_std, cms_set);

    // This order is not arbitrary, and is based on the fact that x2d files have their attachment files dummyed (they get downgraded to "links")
    // Costumes don't have depends. Skills can have costumes (transformation). Super souls can have skills (blast). Characters can have skills and super souls.
    // Replace are their own thing and quests can have any arbitrary attachment
    // Even if the "delayed assigning" of the installer would allow for a rearrange, there are two things to consider:
    // 1) The skill assignment algorithm needs to create fake cms entries once in a while, and these cms id need to be <= 500 for technical reasons,
    // so skills definitely need to be installed before characters.
    // 2) Quests depends are not optional, so everything needs to be installed before quests.
    // TLDR, best order.
    std::vector<std::string> failures;
    RestoreMods(paths, X2mType::NEW_COSTUME, "costume", success, errors, failures);    
    RestoreMods(paths, X2mType::NEW_SKILL, "skill", success, errors, failures);
    RestoreMods(paths, X2mType::NEW_SUPERSOUL, "supersoul", success, errors, failures);
    RestoreMods(paths, X2mType::NEW_CHARACTER, "character", success, errors, failures);
    RestoreMods(paths, X2mType::NEW_STAGE, "stage", success, errors, failures);
    RestoreMods(paths, X2mType::REPLACER, "replacer", success, errors, failures);
    RestoreMods(paths, X2mType::NEW_QUEST, "quest", success, errors, failures);

    // Fail mods must be removed at the end
    for (const std::string &fail : failures)
        Utils::RemoveFile(fail);

    UPRINTF("--------------------------\n");

    if (success == 0 && errors == 0)
    {
        UPRINTF("There were no mods at all!");
    }
    else
    {
       UPRINTF("<b>%d mods were restored with success%s.</b><br>", success, (errors==0) ? " (all of them!)" : "");
       if (errors > 0)
           DPRINTF("%d mods failed to be restored.\n", errors);
    }

    UPRINTF("Restoring character slots...");
    bool restore_char_slots = RestoreCharaSlots(rp_std);
    if (restore_char_slots)
        UPRINTF("<b>Success!</b><br>");
    else
        DPRINTF("Failure!\n");

    UPRINTF("Restoring stage slots...");
    bool restore_stage_slots = RestoreStageSlots(rp_std, false);
    if (restore_stage_slots)
        UPRINTF("<b>Success!</b><br>");
    else
        DPRINTF("Failure!\n");

    UPRINTF("Restoring stage slots (local)...");
    bool restore_stage_slots_local = RestoreStageSlots(rp_std, true);
    if (restore_stage_slots_local)
        UPRINTF("<b>Success!</b><br>");
    else
        DPRINTF("Failure!\n");

    UPRINTF("\n");
    UPRINTF("<b>Program will quit once you close this window.</b>");

#ifdef DEBUG_MEASURE_TIME
    UPRINTF("********Priting install times ordered from lower to bigger.");
    for (const auto &it: itime_map)
    {
        UPRINTF("%d - %s", it.first, it.second.c_str());
    }
#endif

    redirect_uprintf((RedirectFunc)nullptr);
    redirect_dprintf((RedirectFunc)nullptr);

    QMessageBox box;
    QString text;

    text = "The process has finished!<br><br>";
    text += QString("<b>Summary: %1/%2 mods were installed with success.</b><br>").arg(success).arg(success+errors);
    text += QString("<b>Character slots: %1; Stage slots: %2; Stage slots(local): %3.</b><br><br>").
            arg((restore_char_slots) ? "Success" : "Error").arg((restore_stage_slots) ? "Success" : "Error").arg((restore_stage_slots_local) ? "Success" : "Error");
    text += "You can now close the log window at any time, or review it.<br>";
    text += QString("<br><br>What do you want to do with the old data folder (%1)?").arg(recover_path);
    box.setText(text);

    QPushButton *recycle = box.addButton("Send to recycle bin", QMessageBox::ActionRole);
    QPushButton *remove = box.addButton("Delete it", QMessageBox::ActionRole);
    box.addButton("Keep it", QMessageBox::ActionRole);
    box.setDefaultButton(recycle);

    box.exec();

    // UGLY HACK to close the awb files that make impossible to remove the old directory
    X2mFile::RecoverCloseAwb();

    if (box.clickedButton() == recycle)
    {
        if (Utils::RemoveDirFull(rp_std, true, true, false))
        {
            UPRINTF("Done!");
        }
        else
        {
            DPRINTF("Failed to send the foler to recycle bin.");
        }
    }
    else if (box.clickedButton() == remove)
    {
        if (Utils::RemoveDirFull(rp_std, true, false, false))
        {
            UPRINTF("Done!");
        }
        else
        {
            DPRINTF("Failed to delete the folder.");
        }
    }

    while (log_dlg->isVisible())
    {
         QApplication::processEvents();
    }
}

bool MainWindow::MultipleModsQuestion()
{
    QString message = "You are gonna install several mods at same time.\n"
                      "Do you want to activate silent mode?\n\n"
                      "In silent mod, no questions will be asked, and new mods will default to new slots, and updates/reinstall will always be performed.\n";

    QWidget *parent = (invisible_mode) ? nullptr : this;

    if (QMessageBox::question(parent, "Activate silent mode?", message,
                              QMessageBox::StandardButtons(QMessageBox::Yes|QMessageBox::No),
                              QMessageBox::Yes) == QMessageBox::Yes)
    {
        return true;
    }

    return false;

}

void MainWindow::on_actionOpen_triggered()
{
    QStringList files = QFileDialog::getOpenFileNames(this, "Install mod", config.lf_installer_open, "X2M Files (*.x2m)");

    if (files.isEmpty())
        return;

    bool silent = false;

    config.lf_installer_open = files[0];

    if (files.size() > 1)
    {
        silent = MultipleModsQuestion();
    }

    int total = files.size();
    int installed = 0;

    for (QString &file : files)
    {
        ModEntry *mod = InstallMod(file, nullptr, silent);

        if (mod)
        {
            if (!mod->item)
            {
                mod->index = installed_mods.size();
                mod->item = new QTreeWidgetItem();
                mod->item->setData(0, Qt::UserRole, QVariant(mod->index));
                ui->modsList->addTopLevelItem(mod->item);
                installed_mods.push_back(*mod);

                if (total == 1)
                    UPRINTF("Mod succesfully installed.\n");
            }
            else
            {
                if (total == 1)
                    UPRINTF("Mod succesfully updated.\n");
            }

            ModToGui(*mod);
            UpdateStatus();
            installed++;
        }
    }

    if (total != 1)
    {
        if (installed == total)
        {
            UPRINTF("All mods were installed or updated succesfully.\n");
        }
        else
        {
            UPRINTF("%d of %d mods were installed or updated.\n", installed, total);
        }
    }
}

void MainWindow::on_actionExit_triggered()
{
    if (ProcessShutdown())
    {
        qApp->exit();
    }
}

#include "Xv2StageDefFile.h"

void MainWindow::on_actionAbout_triggered()
{    
    //DPRINTF("%d\n", Utils::IsUtf8());
    /*size_t size;
    uint8_t *buf = Utils::ReadFile("C:/Users/MUU/DBXV2_1.21.1_dirty.exe", &size);

    if (!buf)
    {
        DPRINTF("Failed to read file.\n");
        return;
    }

    static const char *eve_dump[] = { "template", "BFlbb", "template", "BFpln", "BFrck", "BFrcn", "BFnmc", "BFnms", "BFnmb", "BFtwc", "BFtwn", "BFtwh", "BFcel", "BFceb", "BFkoh", "BFsin", "BFsky", "BFsei", "BFkai", "BFspe", "BFspv", "BFice", "BFtfl", "BFgen", "BFten", "BFund", "BFtok", "BFnmt", "template", "BFnmc", "BFnms", "DMtwh", "template", "template", "template", "template", "template", "template", "BFlnd", "BFlnc", "BFhel", "template", "template", "template", "template", "template", "template", "template", "template", "template", "DMtwh2", "BFtre", "BFbrw", "FLBY03IN", "FLBY04IN", "FLBY05IN", "BFsmt", "template", "BFtwf", "BFwis", "BFpot", "BFvol", "BFrrg", "Err", "Err", "Err", "Err", "Err", "TRN", "template", "SANDBOX", "NMC", "Random" };

    Xv2StageDefFile stadef;

    //if (!stadef.LoadFromDump(XV2_ORIGINAL_NUM_STAGES, 0x13FAE0000, buf, buf+0xBC8BA0,
    //                         XV2_ORIGINAL_NUM_SS_STAGES, buf+0xBC9380,
    //                         buf+0xBC9410,
    //                         buf+0xBA74E0))
    //if (!stadef.LoadFromDump(XV2_ORIGINAL_NUM_STAGES, 0x140000000, buf, buf+0xCCB7B0,
    //                         XV2_ORIGINAL_NUM_SS_STAGES, buf+0xCCBFB0,
    //                         buf+0xCCC040,
    //                         buf+0xCA7E30,
    //                         buf+0x1136790,
    //                         buf+0xCF23C0,
    //                         eve_dump))
    // 1.09
    //if (!stadef.LoadFromDump(XV2_ORIGINAL_NUM_STAGES, 0x140000000, buf, buf+0xD47340,
    //                         XV2_ORIGINAL_NUM_SS_STAGES, buf+0xD47B40,
    //                         buf+0xD47BD0,
    //                         buf+0xD1EE70,
    //                         buf+0x11CF980,
    //                         buf+0xD6FC80,
    //                         eve_dump))
    // 1.10v2
    //if (!stadef.LoadFromDump(XV2_ORIGINAL_NUM_STAGES, 0x140000000, buf, buf+0xD5AEB0,
    //                         XV2_ORIGINAL_NUM_SS_STAGES, buf+0xD5B6D0,
    //                         buf+0xD5B760,
    //                         buf+0xD32140,
    //                         buf+0x11E7990,
    //                         buf+0xD840E0,
    //                         eve_dump))
    // 1.11
    //if (!stadef.LoadFromDump(XV2_ORIGINAL_NUM_STAGES, 0x140000000, buf, buf+0xD7A7F0,
    //                         XV2_ORIGINAL_NUM_SS_STAGES, buf+0xD7B220,
    //                         buf+0xD7B2B0,
    //                         buf+0xD503D0,
    //                         buf+0x120E9F0,
    //                         buf+0xDA4A30,
    //                         eve_dump))
    // 1.14
    //if (!stadef.LoadFromDump(XV2_ORIGINAL_NUM_STAGES, 0x140000000, buf, buf+0xDBEE50,
    //                         XV2_ORIGINAL_NUM_SS_STAGES, buf+0xDBF8A0,
    //                         buf+0xDBF930,
    //                         buf+0xD936F0,
    //                         buf+0x1264A20,
    //                         buf+0xDEADF0,
    //                         eve_dump))
    // 1.17
    //if (!stadef.LoadFromDump(XV2_ORIGINAL_NUM_STAGES, 0x140000000, buf, buf+0xE9AAF0,
    //                         XV2_ORIGINAL_NUM_SS_STAGES, buf+0xE9B570,
    //                         buf+0xE9B600,
    //                         buf+0xE6E1F0,
    //                         buf+0x1388A10,
    //                         buf+0xECA140,
    //                         eve_dump))
    // 1.19
    //if (!stadef.LoadFromDump(XV2_ORIGINAL_NUM_STAGES, 0x13FA70000, buf, buf+0xEC4A00,
    //                         XV2_ORIGINAL_NUM_SS_STAGES, buf+0xEC54A0,
    //                         buf+0xEC5530,
    //                         buf+0xE97560,
    //                         buf+0x13BBA80,
    //                         buf+0xEF4660,
    //                         eve_dump))
    // 1.20
    //if (!stadef.LoadFromDump(XV2_ORIGINAL_NUM_STAGES, 0x7FF6C2100000, buf, buf+0xECEC30,
    //                         XV2_ORIGINAL_NUM_SS_STAGES, buf+0xECF7A0,
    //                         buf+0xECF830,
    //                         buf+0xEA1350,
    //                         buf+0x13C7CC0,
    //                         buf+0xEFEA30,
    //                         eve_dump))
    // 1.20.1
    //if (!stadef.LoadFromDump(XV2_ORIGINAL_NUM_STAGES, 0x7FF7047C0000, buf, buf+0xED2BC0,
    //                             XV2_ORIGINAL_NUM_SS_STAGES, buf+0xED3730,
    //                             buf+0xED37D0,
    //                             buf+0xEA74E0,
    //                             buf+0x13C7B70,
    //                             buf+0xF022D0,
    //                             eve_dump))
    // 1.21.1
    if (!stadef.LoadFromDump(XV2_ORIGINAL_NUM_STAGES, 0x7FF7DD7C0000, buf, buf+0xFB52E0,
                                 XV2_ORIGINAL_NUM_SS_STAGES, buf+0xFB5E50,
                                 buf+0xFB5EF0,
                                 buf+0xF7CD40,
                                 buf+0xF81F60,
                                 buf+0x14EDBD0,
                                 buf+0xFE5AC0,
                                 eve_dump))
        return;

    if (!xv2fs->DecompileFile(&stadef, "data/xv2_stage_def.xml"))
        return;

    UPRINTF("Oki.\n");*/

    QMessageBox box;

    box.setWindowTitle(PROGRAM_NAME_INSTALLER);
    box.setTextFormat(Qt::RichText);
    box.setIcon(QMessageBox::Icon::Information);
    box.setText(QString("%1 v%2 by Eternity<br>Built on %3 %4<br><br>Detected xv2patcher version: %5<br><br>If you liked the program, you can support its development at<br><a href='https://www.patreon.com/eternity_tools'>https://www.patreon.com/eternity_tools</a>").arg(PROGRAM_NAME_INSTALLER).arg(PROGRAM_VERSION).arg(__DATE__).arg(__TIME__).arg(config.patcher_version_new));

    //box.setText(QString("%1 v%2 by Eternity<br>Built on %3 %4<br><br>Detected xv2patcher version: %5").arg(PROGRAM_NAME_INSTALLER).arg(PROGRAM_VERSION).arg(__DATE__).arg(__TIME__).arg(config.patcher_version_new));

    box.exec();

    /*UPRINTF("%s v%s by Eternity\n\nBuilt on %s %s\n"
            "Detected xv2patcher version: %f\n", PROGRAM_NAME_INSTALLER, PROGRAM_VERSION, __DATE__, __TIME__, config.patcher_version_new);*/
}

void MainWindow::on_modsList_itemSelectionChanged()
{
    if (!process_selection_change)
        return;

    //std::vector<ModEntry *> mods;
    //ui->actionUninstall->setEnabled(GetSelectedMods(mods) > 0);

    ModEntry *mod = GetSelectedMod();
    ui->actionUninstall->setEnabled(mod != nullptr);
}

void MainWindow::RemoveDeadItem()
{
    for (int i = 0; i < ui->modsList->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *item = ui->modsList->topLevelItem(i);

        if (item)
        {
            int index = item->data(0, Qt::UserRole).toInt();

            if (index == -1)
            {
                ui->modsList->takeTopLevelItem(i);
                //return;
            }
        }
    }
}

void MainWindow::on_actionUninstall_triggered()
{    
    /*ModEntry *mod = GetSelectedMod();
    if (!mod)
        return;*/
    std::vector<int> mods_idx;
    if (GetSelectedMods(mods_idx) == 0)
        return;

    for (size_t i = 0; i < mods_idx.size(); i++)
    {
        ModEntry &mod = installed_mods[mods_idx[i]];
        //UPRINTF("Uninstall %s", Utils::QStringToStdString(mod.name).c_str());

        if (!UninstallMod(mod, true))
            return;

        if (mods_idx.size() == 1)
            UPRINTF("Mod succesfully uninstalled.\n");

        int index = mod.index;
        mod.item->setData(0, Qt::UserRole, QVariant(-1));
        installed_mods.remove(index);

        for (size_t j = i+1; j < mods_idx.size(); j++)
        {
            if (mods_idx[j] > index)
                mods_idx[j]--;
        }

        for (int i = index; i < installed_mods.size(); i++)
        {
            ModEntry &mod = installed_mods[i];

            mod.index--;
            mod.item->setData(0, Qt::UserRole, QVariant(mod.index));
        }
    }

    if (mods_idx.size() > 1)
    {
        UPRINTF("Mods succesfully uninstalled.\n");
    }

    if (installed_mods.size() == 0)
    {
        ui->actionUninstall->setDisabled(true);
    }    

    RemoveDeadItem();
    UpdateStatus();
}

void MainWindow::on_actionAssociate_x2m_extension_triggered()
{
    std::string reg_data;
    std::string program = Utils::QStringToStdString(qApp->applicationFilePath().replace('/', '\\').replace('\\', "\\\\"));
    std::string editor;

    QString editor_q = qApp->applicationDirPath();

    if (!editor_q.endsWith('/') && !editor_q.endsWith('\\'))
        editor_q += '/';

    editor_q += "xv2characreat.exe";
    if (Utils::FileExists(Utils::QStringToStdString(editor_q)))
    {
        editor = Utils::QStringToStdString(editor_q.replace('/', '\\').replace('\\', "\\\\"));
    }

    reg_data = "Windows Registry Editor Version 5.00\r\n\r\n";
    reg_data += "[HKEY_CURRENT_USER\\Software\\Classes\\X2MMod\\shell\\open\\command]\r\n";
    reg_data += "@=\"" + program +  " \\\"%1\\\"\"\r\n";

    if (editor.length() != 0)
    {
        reg_data += "[HKEY_CURRENT_USER\\Software\\Classes\\X2MMod\\shell\\Edit\\command]\r\n";
        reg_data += reg_data += "@=\"" + editor +  " \\\"%1\\\"\"\r\n";
    }

    reg_data += "[HKEY_CURRENT_USER\\Software\\Classes\\.x2m]\r\n";
    reg_data += "@=\"X2MMod\"\r\n";

    std::string temp_file = Utils::GetTempFile("x2i", ".reg");

    if (!Utils::WriteFileBool(temp_file, (const uint8_t *)reg_data.c_str(), reg_data.length()))
        return;

    Utils::RunCmd("regedit", { temp_file });
}

void MainWindow::on_actionSlot_editor_triggered()
{
    CssDialog dialog(CssMode::EDIT, this);

    if (dialog.exec())
    {
        bool commit_emb = false, commit_slots = false;
        std::vector<CharaListSlot> &edit_slots = dialog.GetEditedSlots();
        EmbFile *edit_emb = dialog.GetEditedEmb();

        if (*edit_emb != *game_sel_port)
            commit_emb = true;

        if (edit_slots != chara_list->GetSlots())
            commit_slots = true;

        if (commit_emb)
        {
            *game_sel_port = *edit_emb;

            if (!Xenoverse2::CommitSelPort())
            {
                DPRINTF("Commit SelPort failed.\n");
                return;
            }
        }

        if (commit_slots)
        {
            std::vector<CharaListSlot> &c_slots = chara_list->GetSlots();
            c_slots = edit_slots;

            /*if (!Xenoverse2::CompileCharaSel(Utils::QStringToStdString(config.flex_path)))
            {
                DPRINTF("Compilation failed.\n");
                return;
            }*/

            if (!Xenoverse2::CommitCharaList(true, false))
            {
                DPRINTF("CommitCharaList failed.\n");
                return;
            }

            //UPRINTF("Done");
        }
    }
}

void MainWindow::on_actionFind_and_delete_dead_ids_triggered()
{
    int found = 0;
    int deleted = 0;

    std::vector<CusSkillSet> &skill_sets = game_cus->GetSkillSets();

    for (size_t i = 0; i < skill_sets.size(); i++)
    {
        const CusSkillSet &set = skill_sets[i];

        if (set.char_id < XV2_FREE_ID_SEARCH_START)
            continue;

        if (!game_cms->FindEntryByID(set.char_id))
        {
            found++;

            QString message = QString("The ID 0x%1 referenced in the cus doesn't exist in the cms.\n"
                                      "Do you want to delete it?\n").arg(set.char_id, 0, 16);

            if (QMessageBox::question(this, "Delete it?", message,
                                      QMessageBox::StandardButtons(QMessageBox::Yes|QMessageBox::No),
                                      QMessageBox::Yes) == QMessageBox::Yes)
            {
                skill_sets.erase(skill_sets.begin()+i);
                i--;
                deleted++;
            }
        }
    }

    std::vector<CsoEntry> &cso_entries = game_cso->GetEntries();

    for (size_t i = 0; i < cso_entries.size(); i++)
    {
        const CsoEntry &entry = cso_entries[i];

        if (entry.char_id < XV2_FREE_ID_SEARCH_START)
            continue;

        if (!game_cms->FindEntryByID(entry.char_id))
        {
            found++;

            QString message = QString("The ID 0x%1 referenced in the cso doesn't exist in the cms.\n"
                                      "Do you want to delete it?\n").arg(entry.char_id, 0, 16);

            if (QMessageBox::question(this, "Delete it?", message,
                                      QMessageBox::StandardButtons(QMessageBox::Yes|QMessageBox::No),
                                      QMessageBox::Yes) == QMessageBox::Yes)
            {
                cso_entries.erase(cso_entries.begin()+i);
                i--;
                deleted++;
            }
        }
    }

    for (size_t c = 0; c < game_psc->GetNumConfigs(); c++)
    {
        std::vector<PscEntry> &psc_entries = game_psc->GetEntries(c);

        for (size_t i = 0; i < psc_entries.size(); i++)
        {
            const PscEntry &entry = psc_entries[i];

            if (entry.char_id < XV2_FREE_ID_SEARCH_START)
                continue;

            if (!game_cms->FindEntryByID(entry.char_id))
            {
                found++;

                QString message = QString("The ID 0x%1 referenced in the psc doesn't exist in the cms.\n"
                                          "Do you want to delete it?\n").arg(entry.char_id, 0, 16);

                if (QMessageBox::question(this, "Delete it?", message,
                                          QMessageBox::StandardButtons(QMessageBox::Yes|QMessageBox::No),
                                          QMessageBox::Yes) == QMessageBox::Yes)
                {
                    psc_entries.erase(psc_entries.begin()+i);
                    i--;
                    deleted++;
                }
            }
        }
    }

    std::vector<AurCharaLink> &aur_entries = game_aur->GetCharaLinks();

    for (size_t i = 0; i < aur_entries.size(); i++)
    {
        const AurCharaLink &entry = aur_entries[i];

        if (entry.char_id < XV2_FREE_ID_SEARCH_START)
            continue;

        if (!game_cms->FindEntryByID(entry.char_id))
        {
            found++;

            QString message = QString("The ID 0x%1 referenced in the aur doesn't exist in the cms.\n"
                                      "Do you want to delete it?\n").arg(entry.char_id, 0, 16);

            if (QMessageBox::question(this, "Delete it?", message,
                                      QMessageBox::StandardButtons(QMessageBox::Yes|QMessageBox::No),
                                      QMessageBox::Yes) == QMessageBox::Yes)
            {
                aur_entries.erase(aur_entries.begin()+i);
                i--;
                deleted++;
            }
        }
    }

    std::vector<SevEntry> &sev_entries = game_sev->GetEntries();

    for (size_t i = 0; i < sev_entries.size(); i++)
    {
        SevEntry &entry = sev_entries[i];

        if (!game_cms->FindEntryByID(entry.char_id))
        {
            found++;

            QString message = QString("The ID 0x%1 referenced in the sev (as source) doesn't exist in the cms.\n"
                                      "Do you want to delete it?\n").arg(entry.char_id, 0, 16);

            if (QMessageBox::question(this, "Delete it?", message,
                                      QMessageBox::StandardButtons(QMessageBox::Yes|QMessageBox::No),
                                      QMessageBox::Yes) == QMessageBox::Yes)
            {
                sev_entries.erase(sev_entries.begin()+i);
                i--;
                deleted++;
                continue;
            }
        }

        for (size_t j = 0; j < entry.chars_events.size(); j++)
        {
            const SevCharEvents &cevents = entry.chars_events[j];

            if (cevents.char_id == 0xFFFFFFFF)
                continue;

            if (!game_cms->FindEntryByID(cevents.char_id))
            {
                found++;

                QString message = QString("The ID 0x%1 referenced in the sev (as destination) doesn't exist in the cms.\n"
                                          "Do you want to delete it?\n").arg(entry.char_id, 0, 16);

                if (QMessageBox::question(this, "Delete it?", message,
                                          QMessageBox::StandardButtons(QMessageBox::Yes|QMessageBox::No),
                                          QMessageBox::Yes) == QMessageBox::Yes)
                {
                    entry.chars_events.erase(entry.chars_events.begin()+j);
                    j--;
                    deleted++;
                }
            }
        }

        if (entry.chars_events.size() == 0) // If became empty because of above (very rare, but it can happen)
        {
            sev_entries.erase(sev_entries.begin()+i);
            i--;
        }
    }

    std::vector<CmlEntry> &cml_entries = game_cml->GetEntries();

    for (size_t i = 0; i < cml_entries.size(); i++)
    {
        const CmlEntry &entry = cml_entries[i];

        if (entry.char_id < XV2_FREE_ID_SEARCH_START || entry.char_id == 0xFFFF)
            continue;

        if (!game_cms->FindEntryByID(entry.char_id))
        {
            found++;

            QString message = QString("The ID 0x%1 referenced in the cml doesn't exist in the cms.\n"
                                      "Do you want to delete it?\n").arg(entry.char_id, 0, 16);

            if (QMessageBox::question(this, "Delete it?", message,
                                      QMessageBox::StandardButtons(QMessageBox::Yes|QMessageBox::No),
                                      QMessageBox::Yes) == QMessageBox::Yes)
            {
                cml_entries.erase(cml_entries.begin()+i);
                i--;
                deleted++;
            }
        }
    }

    std::vector<HciEntry> &hci_entries = game_hci->GetEntries();

    for (size_t i = 0; i < hci_entries.size(); i++)
    {
        const HciEntry &entry = hci_entries[i];

        if (entry.char_id < XV2_FREE_ID_SEARCH_START)
            continue;

        if (!game_cms->FindEntryByID(entry.char_id))
        {
            found++;

            QString message = QString("The ID 0x%1 referenced in the hci doesn't exist in the cms.\n"
                                      "Do you want to delete it?\n").arg(entry.char_id, 0, 16);

            if (QMessageBox::question(this, "Delete it?", message,
                                      QMessageBox::StandardButtons(QMessageBox::Yes|QMessageBox::No),
                                      QMessageBox::Yes) == QMessageBox::Yes)
            {
                hci_entries.erase(hci_entries.begin()+i);
                i--;
                deleted++;
            }
        }
    }

    std::vector<std::string> &ozarus = game_prebaked->GetOzarus();

    for (size_t i = 0; i < ozarus.size(); i++)
    {
        const std::string &ozaru = ozarus[i];

        if (!game_cms->FindEntryByName(ozaru))
        {
            found++;

            QString message = QString("Character %1 referenced in the ozarus list doesn't exist in the cms.\n"
                                      "Do you want to delete it?\n").arg(Utils::StdStringToQString(ozaru));

            if (QMessageBox::question(this, "Delete it?", message,
                                      QMessageBox::StandardButtons(QMessageBox::Yes|QMessageBox::No),
                                      QMessageBox::Yes) == QMessageBox::Yes)
            {
                ozarus.erase(ozarus.begin()+i);
                i--;
                deleted++;
            }
        }
    }

    std::vector<std::string> &cell_maxes = game_prebaked->GetCellMaxes();

    for (size_t i = 0; i < cell_maxes.size(); i++)
    {
        const std::string &cm = cell_maxes[i];

        if (!game_cms->FindEntryByName(cm))
        {
            found++;

            QString message = QString("Character %1 referenced in the Cell Maxes list doesn't exist in the cms.\n"
                                      "Do you want to delete it?\n").arg(Utils::StdStringToQString(cm));

            if (QMessageBox::question(this, "Delete it?", message,
                                      QMessageBox::StandardButtons(QMessageBox::Yes|QMessageBox::No),
                                      QMessageBox::Yes) == QMessageBox::Yes)
            {
                cell_maxes.erase(cell_maxes.begin()+i);
                i--;
                deleted++;
            }
        }
    }

    std::vector<uint32_t> &auto_btl_portrait_list = game_prebaked->GetAutoBtlPortraitList();

    for (size_t i = 0; i < auto_btl_portrait_list.size(); i++)
    {
        uint32_t cms_entry = auto_btl_portrait_list[i];

        if (!game_cms->FindEntryByID(cms_entry))
        {
            found++;

            QString message = QString("Character %1 referenced in the auto battle portrait list doesn't exist in the cms.\n"
                                      "Do you want to delete it?\n").arg(cms_entry, 0, 16);

            if (QMessageBox::question(this, "Delete it?", message,
                                      QMessageBox::StandardButtons(QMessageBox::Yes|QMessageBox::No),
                                      QMessageBox::Yes) == QMessageBox::Yes)
            {
                auto_btl_portrait_list.erase(auto_btl_portrait_list.begin()+i);
                i--;
                deleted++;
            }
        }
    }

    std::vector<BodyShape> &body_shapes = game_prebaked->GetBodyShapes();

    for (size_t i = 0; i < body_shapes.size(); i++)
    {
        const BodyShape &shape = body_shapes[i];

        if (!game_cms->FindEntryByID(shape.cms_entry))
        {
            found++;

            QString message = QString("Character %1 referenced in the body_shapes (pre-baked.xml) doesn't exist in the cms.\n"
                                      "Do you want to delete it?\n").arg(shape.cms_entry, 0, 16);

            if (QMessageBox::question(this, "Delete it?", message,
                                      QMessageBox::StandardButtons(QMessageBox::Yes|QMessageBox::No),
                                      QMessageBox::Yes) == QMessageBox::Yes)
            {
                body_shapes.erase(body_shapes.begin()+i);
                i--;
                deleted++;
            }
        }
    }

    if (deleted != 0)
    {
        if (!Xenoverse2::CommitSystemFiles(false, true, true))
        {
            DPRINTF("CommitSystemFiles failed.\n");
            return;
        }

        if (!Xenoverse2::CommitPreBaked())
        {
            DPRINTF("CommitPreBaked failed.\n");
            return;
        }
    }

    if (found == 0)
    {
        UPRINTF("No dead ids were found (that's good).\n");
    }
    else if (deleted == found)
    {
        DPRINTF("%d ids were found and deleted.\n", deleted);
    }
    else
    {
        DPRINTF("%d ids were found, and %d were deleted.\n", found, deleted);
    }
}

void MainWindow::on_actionClear_installation_triggered()
{
    ClearInstallation(false);
}

bool MainWindow::DeleteEmptyDirVisitor(const std::string &path, bool, void *param)
{
    MainWindow *pthis = (MainWindow *)param;

    if (!Utils::IsDirectoryEmpty(path, true))
        return true;

    pthis->temp_empty_found++;

    if (!pthis->temp_empty_yesall)
    {
        int result = QMessageBox::question(pthis, "Delete?",
                                           "Do you want to delete the following empty directory: \"" + Utils::StdStringToQString(path) + "\"?\n",
                                           QMessageBox::StandardButtons(QMessageBox::Yes|QMessageBox::YesToAll|QMessageBox::No), QMessageBox::Yes);

        if (result != QMessageBox::Yes && result != QMessageBox::YesToAll)
            return true;

        if (result == QMessageBox::YesToAll)
            pthis->temp_empty_yesall = true;
    }

    for (int i = 0; i < 5; i++)
    {
        if (Utils::RemoveDirFull(path, false, false, false))
        {
            pthis->temp_empty_deleted++;
            break;
        }
    }

    return true;
}

void MainWindow::on_actionRemoveEmptyDirs_triggered()
{
    std::string path = Utils::MakePathString(Utils::QStringToStdString(config.game_directory), "data/");

    temp_empty_deleted = temp_empty_found = 0;
    temp_empty_yesall = false;
    Utils::VisitDirectory(path, false, true, true, DeleteEmptyDirVisitor, this);

    if (temp_empty_found == 0)
    {
        UPRINTF("No empty directories were found.\n");
    }
    else if (temp_empty_deleted == temp_empty_found)
    {
        DPRINTF("%d empty directories were found and deleted.\n", temp_empty_deleted);
    }
    else
    {
        DPRINTF("%d empty directories were found, and %d were deleted.\n", temp_empty_found, temp_empty_deleted);
    }
}

static bool convert_visitor(const std::string &path, void *param)
{
    std::vector<std::string> *list_files = (std::vector<std::string> *)param;
    list_files->push_back(path);
    return true;
}

bool MainWindow::ConvertModEntry(X2mFile *x2m, const std::string &new_entry)
{
    if (x2m->GetType() != X2mType::NEW_CHARACTER)
    {
        DPRINTF("That x2m is not of character type!\n");
        return false;
    }

    if (new_entry.length() != 3)
    {
        DPRINTF("New entry must be 3-letter.\n");
        return false;
    }

    const std::string old_entry = x2m->GetEntryName();

    if (old_entry == new_entry)
    {
        DPRINTF("That x2m already has this entry name (%s)\n", new_entry.c_str());
        return false;
    }

    x2m->SetEntryName(new_entry);

    if (!x2m->RenamePath(old_entry, new_entry))
    {
        DPRINTF("Failed to rename character folder.\n");
        return false;
    }

    std::vector<std::string> list_files;

    if (!x2m->VisitDirectory(new_entry, convert_visitor, &list_files))
    {
        DPRINTF("Failed to list x2m character folder.\n");
        return false;
    }

    UPRINTF("X2M character folder renamed.");

    size_t num = 0;
    for (const std::string &path : list_files)
    {
        const std::string name = Utils::GetFileNameString(path);
        const std::string dir_name = Utils::GetDirNameString(path);

        if (Utils::ToUpperCase(dir_name) != Utils::ToUpperCase(new_entry))
        {
            continue;
        }

        if (Utils::BeginsWith(name, old_entry, false))
        {
            std::string new_name = new_entry + name.substr(3);

            if (!x2m->RenameFile(path, new_entry+"/"+new_name))
            {
                DPRINTF("Failed to rename file %s\n", path.c_str());
                return false;
            }

            num++;
        }
    }

    UPRINTF("%Id files were renamed in the character folder.", num);

    num = 0;
    size_t bcs_size;
    const std::string bcs_path = new_entry + "/" + new_entry + ".bcs";
    uint8_t *bcs_buf = x2m->ReadFile(bcs_path, &bcs_size);

    if (bcs_buf)
    {
        BcsFile bcs;

        bool ret = bcs.Load(bcs_buf, bcs_size);
        delete[] bcs_buf;

        if (ret)
        {
            for (BcsPartSet &set : bcs.GetPartSets())
            {
                if (!set.valid)
                    continue;

                for (BcsPart &part : set.parts)
                {
                    if (!part.valid)
                        continue;

                    if (strlen(part.name) == 3 && Utils::BeginsWith(old_entry, part.name, false))
                    {
                        strcpy(part.name, new_entry.c_str());
                        num++;
                    }

                    for (std::string &file : part.files)
                    {
                        if (file.length() == 0)
                            continue;

                        if (Utils::BeginsWith(file, old_entry, false))
                        {
                            file = new_entry + file.substr(3);
                            num++;
                        }
                    }

                    for (BcsPhysics &ph : part.physics)
                    {
                        if (strlen(ph.name) == 3 && Utils::BeginsWith(old_entry, ph.name, false))
                        {
                            strcpy(ph.name, new_entry.c_str());
                            num++;
                        }

                        for (std::string &s28 : ph.unk_28)
                        {
                            if (s28.length() == 0)
                                continue;

                            if (Utils::BeginsWith(s28, old_entry, false))
                            {
                                s28 = new_entry + s28.substr(3);
                                num++;
                            }
                        }
                    }
                }
            } // end top for

            bcs_buf = bcs.Save(&bcs_size);
            if (!bcs_buf)
            {
                DPRINTF("Bcs save failed (1)\n");
                return false;
            }

            bool ret = x2m->WriteFile(bcs_path, bcs_buf, bcs_size);
            delete[] bcs_buf;

            if (!ret)
            {
                DPRINTF("Bcs save failed (2)\n");
                return false;
            }
        }
        else
        {
            DPRINTF("Bcs file exists, but load failed.\n");
            return false;
        }
    }

    if (num != 0)
    {
        UPRINTF("Bcs processed, %Id strings changed.\n", num);
    }

    CmsEntryXV2 &cms = x2m->GetCmsEntry();
    UPRINTF("Processing CMS...");

    if (Utils::BeginsWith(cms.character, old_entry, false))
    {
        std::string new_character = new_entry + cms.character.substr(3);
        UPRINTF("Character changed (%s->%s)", cms.character.c_str(), new_character.c_str());
        cms.character = new_character;
    }

    if (Utils::BeginsWith(cms.ean, old_entry, false))
    {
        std::string new_ean = new_entry + cms.ean.substr(3);
        UPRINTF("ean changed (%s->%s)", cms.ean.c_str(), new_ean.c_str());
        cms.ean = new_ean;
    }

    if (Utils::BeginsWith(cms.fce_ean, old_entry, false))
    {
        std::string new_fce_ean = new_entry + cms.fce_ean.substr(3);
        UPRINTF("fce.ean changed (%s->%s)", cms.fce_ean.c_str(), new_fce_ean.c_str());
        cms.fce_ean = new_fce_ean;
    }

    if (Utils::BeginsWith(cms.fce, old_entry, false))
    {
        std::string new_fce = new_entry + cms.fce.substr(3);
        UPRINTF("fce changed (%s->%s)", cms.fce.c_str(), new_fce.c_str());
        cms.fce = new_fce;
    }

    if (Utils::BeginsWith(cms.cam_ean, old_entry, false))
    {
        std::string new_cam_ean = new_entry + cms.cam_ean.substr(3);
        UPRINTF("cam.ean changed (%s->%s)", cms.cam_ean.c_str(), new_cam_ean.c_str());
        cms.cam_ean = new_cam_ean;
    }

    if (Utils::BeginsWith(cms.bac, old_entry, false))
    {
        std::string new_bac = new_entry + cms.bac.substr(3);
        UPRINTF("bac changed (%s->%s)", cms.bac.c_str(), new_bac.c_str());
        cms.bac = new_bac;
    }

    if (Utils::BeginsWith(cms.bcm, old_entry, false))
    {
        std::string new_bcm = new_entry + cms.bcm.substr(3);
        UPRINTF("bcm changed (%s->%s)", cms.bcm.c_str(), new_bcm.c_str());
        cms.bcm = new_bcm;
    }

    if (Utils::BeginsWith(cms.bdm, old_entry, false))
    {
        std::string new_bdm = new_entry + cms.bdm.substr(3);
        UPRINTF("bdm changed (%s->%s)", cms.bdm.c_str(), new_bdm.c_str());
        cms.bdm = new_bdm;
    }

    UPRINTF("CMS processed\n");

    for (size_t i = 0; i < x2m->GetNumCsoEntries(); i++)
    {
        CsoEntry &cso = x2m->GetCsoEntry(i);
        std::string amk = old_entry + "/" + old_entry;

        if (Utils::ToUpperCase(cso.amk) == amk)
        {
            cso.amk = new_entry + "/" + new_entry;
            DPRINTF("CSO amk changed.");
        }

        std::string se = "CAR_BTL_" + old_entry + "_SE";

        if (Utils::ToUpperCase(cso.se) == se)
        {
            cso.se = "CAR_BTL_" + new_entry + "_SE";
            std::string base_path_jp = std::string(X2M_JUNGLE) + "data/sound/SE/Battle/Chara/";
            std::string base_path_en = std::string(X2M_JUNGLE) + "data/sound/SE/Battle/Chara/en/";

            UPRINTF("CSO SE changed (entry %Id)", i);

            if (x2m->FileExists(base_path_jp + se + ".acb"))
            {
                if (!x2m->RenameFile(base_path_jp + se + ".acb", base_path_jp + cso.se + ".acb"))
                {
                    DPRINTF("Failed to rename SE acb file.\n");
                    return false;
                }

                UPRINTF("Renamed JP SE acb file in JUNGLE");
            }

            if (x2m->FileExists(base_path_jp + se + ".awb"))
            {
                if (!x2m->RenameFile(base_path_jp + se + ".awb", base_path_jp + cso.se + ".awb"))
                {
                    DPRINTF("Failed to rename SE awb file.\n");
                    return false;
                }

                UPRINTF("Renamed JP SE awb file in JUNGLE");
            }

            if (x2m->FileExists(base_path_en + se + ".acb"))
            {
                if (!x2m->RenameFile(base_path_en + se + ".acb", base_path_en + cso.se + ".acb"))
                {
                    DPRINTF("Failed to rename SE acb file.\n");
                    return false;
                }

                UPRINTF("Renamed EN SE acb file in JUNGLE");
            }

            if (x2m->FileExists(base_path_en + se + ".awb"))
            {
                if (!x2m->RenameFile(base_path_en + se + ".awb", base_path_en + cso.se + ".awb"))
                {
                    DPRINTF("Failed to rename SE awbfile.\n");
                    return false;
                }

                UPRINTF("Renamed EN SE awb file in JUNGLE");
            }
        }

        std::string vox = "CAR_BTL_" + old_entry + "_VOX";

        if (Utils::ToUpperCase(cso.vox) == vox)
        {
            cso.vox = "CAR_BTL_" + new_entry + "_VOX";
            std::string base_path_jp = std::string(X2M_JUNGLE) + "data/sound/VOX/Battle/Chara/";
            std::string base_path_en = std::string(X2M_JUNGLE) + "data/sound/VOX/Battle/Chara/en/";

            UPRINTF("CSO VOX changed (entry %Id)", i);

            if (x2m->FileExists(base_path_jp + vox + ".acb"))
            {
                if (!x2m->RenameFile(base_path_jp + vox + ".acb", base_path_jp + cso.vox + ".acb"))
                {
                    DPRINTF("Failed to rename VOX acb file.\n");
                    return false;
                }

                UPRINTF("Renamed JP VOX acb file in JUNGLE");
            }

            if (x2m->FileExists(base_path_jp + vox + ".awb"))
            {
                if (!x2m->RenameFile(base_path_jp + vox + ".awb", base_path_jp + cso.vox + ".awb"))
                {
                    DPRINTF("Failed to rename VOX awb file.\n");
                    return false;
                }

                UPRINTF("Renamed JP VOX awb file in JUNGLE");
            }

            if (x2m->FileExists(base_path_en + vox + ".acb"))
            {
                if (!x2m->RenameFile(base_path_en + vox + ".acb", base_path_en + cso.vox + ".acb"))
                {
                    DPRINTF("Failed to rename VOX acb file.\n");
                    return false;
                }

                UPRINTF("Renamed EN VOX acb file in JUNGLE");
            }

            if (x2m->FileExists(base_path_en + vox + ".awb"))
            {
                if (!x2m->RenameFile(base_path_en + vox + ".awb", base_path_en + cso.vox + ".awb"))
                {
                    DPRINTF("Failed to rename VOX awbfile.\n");
                    return false;
                }

                UPRINTF("Renamed EN VOX awb file in JUNGLE");
            }
        }

        if (cso.skills.length() == 0 || Utils::ToUpperCase(cso.skills) == old_entry)
        {
            if (cso.skills.length() != 0)
            {
                cso.skills = new_entry;
                UPRINTF("CSO Skills changed (entry %Id)", i);
            }

            std::string base_path = std::string(X2M_JUNGLE) + "data/sound/VOX/Battle/Skill/";
            list_files.clear();

            if (x2m->VisitDirectory(base_path, convert_visitor, &list_files))
            {
                for (const std::string &file : list_files)
                {
                    std::string fn = Utils::GetFileNameString(file);

                    if (Utils::BeginsWith(fn, "CAR_BTL_", false))
                    {
                        size_t pos = fn.substr(8).find('_');

                        if (pos != std::string::npos)
                        {
                            size_t pos2 = fn.substr(8+pos+1).find('_');
                            if (pos2 != std::string::npos)
                            {
                                std::string last_part = fn.substr(8+pos+1+pos2+1);
                                std::string expected = old_entry + "_VOX";

                                if (Utils::BeginsWith(last_part, expected, false))
                                {
                                    std::string nfn = fn;
                                    nfn.replace(8+pos+1+pos2+1, 3, new_entry);

                                    std::string new_file = Utils::MakePathString(Utils::GetDirNameString(file), nfn);

                                    if (!x2m->RenameFile(file ,new_file))
                                    {
                                        DPRINTF("Failed to rename skill audio file.\n");
                                        return false;
                                    }

                                    UPRINTF("File %s renamed to %s\n", fn.c_str(), nfn.c_str());
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if (x2m->HasTtcLL())
    {
        // TTC audio
        for (int i = 0; i < 2; i++)
        {
            std::string path = std::string(X2M_JUNGLE) + "data/sound/VOX/Quest/Common/";

            if (i == 1)
                path += "en/";

            path += "CAQ_";

            const std::string old_path = path + old_entry + "_VOX";
            const std::string new_path = path + new_entry + "_VOX";

            const std::string old_acb = old_path + ".acb";
            const std::string new_acb = new_path + ".acb";

            const std::string old_awb = old_path + ".awb";
            const std::string new_awb = new_path + ".awb";

            if (x2m->FileExists(old_acb))
            {
                if (!x2m->RenameFile(old_acb, new_acb))
                {
                    DPRINTF("Failed to rename %s\n", old_acb.c_str());
                    return false;
                }

                UPRINTF("File %s renamed to %s", Utils::GetFileNameString(old_acb).c_str(), Utils::GetFileNameString(new_acb).c_str());
            }

            if (x2m->FileExists(old_awb))
            {
                if (!x2m->RenameFile(old_awb, new_awb))
                {
                    DPRINTF("Failed to rename %s\n", old_awb.c_str());
                    return false;
                }

                UPRINTF("File %s renamed to %s", Utils::GetFileNameString(old_awb).c_str(), Utils::GetFileNameString(new_awb).c_str());
            }
        }

        {
            // TTC voice
            const std::string voice_path = std::string(X2M_JUNGLE) + "data/msg/qc_";
            const std::string old_voice_path = voice_path + old_entry + "_voice.msg";
            const std::string new_voice_path = voice_path + new_entry + "_voice.msg";

            if (x2m->FileExists(old_voice_path))
            {
                if (!x2m->RenameFile(old_voice_path, new_voice_path))
                {
                    DPRINTF("Failed to rename %s\n", old_voice_path.c_str());
                    return false;
                }

                UPRINTF("File %s renamed to %s", Utils::GetFileNameString(old_voice_path).c_str(), Utils::GetFileNameString(new_voice_path).c_str());
            }
        }

        // TTC subs
        for (int i = 0; i < XV2_LANG_NUM; i++)
        {
            const std::string sub_path = std::string(X2M_JUNGLE) + "data/msg/qc_";
            const std::string old_sub_path = sub_path + old_entry + "_" + xv2_lang_codes[i] + ".msg";
            const std::string new_sub_path = sub_path + new_entry + "_" + xv2_lang_codes[i] + ".msg";

            if (x2m->FileExists(old_sub_path))
            {
                if (!x2m->RenameFile(old_sub_path, new_sub_path))
                {
                    DPRINTF("Failed to rename %s\n", old_sub_path.c_str());
                    return false;
                }

                UPRINTF("File %s renamed to %s", Utils::GetFileNameString(old_sub_path).c_str(), Utils::GetFileNameString(new_sub_path).c_str());
            }
        }
    }

    return true;
}

void MainWindow::on_actionChange_3_letter_code_of_x2m_triggered()
{
    QString file = QFileDialog::getOpenFileName(this, "Open x2m", config.lf_installer_open, "X2M Files (*.x2m)");

    if (file.isNull())
        return;

    config.lf_installer_open = file;

    X2mFile x2m;

    if (!x2m.LoadFromFile(Utils::QStringToStdString(file)))
    {
        DPRINTF("Cannot open x2m.\n");
        return;
    }

    if (x2m.GetType() != X2mType::NEW_CHARACTER)
    {
        DPRINTF("That x2m is not a new character one.\n");
        return;
    }

    CmsEntryChooseDialog choose(x2m.GetEntryName(), false, this);

    if (!choose.exec())
        return;

    file = file.left(file.size()-4) + "_" + Utils::StdStringToQString(choose.GetResult()) + ".x2m";
    file = QFileDialog::getSaveFileName(this, "Output file", file, "X2M Files (*.x2m)");

    if (file.isNull())
        return;

    config.lf_installer_open = file;

    log_dlg = new LogDialog(this);
    log_dlg->setWindowTitle("3-letter code change");
    log_dlg->show();

    redirect_dprintf(log_window_error);
    redirect_uprintf(log_window);

    bool ret = ConvertModEntry(&x2m, choose.GetResult());

    redirect_dprintf((RedirectFunc)nullptr);
    redirect_uprintf((RedirectFunc)nullptr);

    if (!ret)
    {
        DPRINTF("Change failed. Look at log window.\n");
        return;
    }

    if (!x2m.SaveToFile(Utils::QStringToStdString(file)))
    {
        DPRINTF("Save failed.\n");
        return;
    }

    UPRINTF("Succesfully changed 3-letter code.\n");
}

void MainWindow::on_actionConvert_charalist_triggered()
{
    QString dir = Utils::StdStringToQString(Utils::GetDirNameString(Utils::QStringToStdString(config.lf_installer_open)));

    QString file = QFileDialog::getOpenFileName(this, "Open CharaList.as", dir, "ActionScript Source (*.as)");

    if (file.isNull())
        return;

    CharaListFile chl;

    if (!chl.LoadFromFile(Utils::QStringToStdString(file)))
    {
        DPRINTF("Failed to load CharaList file.\n");
        return;
    }

    Xv2PatcherSlotsFile x2s(chl);

    file = QFileDialog::getSaveFileName(this, "Save x2s", dir, "XV2Patcher Slot (*.x2s)");

    if (file.isNull())
        return;

    if (!x2s.SaveToFile(Utils::QStringToStdString(file)))
    {
        DPRINTF("Failed to save slots file file.\n");
        return;
    }

    UPRINTF("Slot files succesfully saved.\n");
}

void MainWindow::on_actionCompile_INTERNAL_folder_triggered()
{
    CompileDialog dialog(this);

    if (dialog.exec())
    {
        config.Save();

        if (!Xenoverse2::CompileCharaSel(CHASEL_PATH, Utils::QStringToStdString(config.flex_path)))
        {
            DPRINTF("Compilation failed.\n");
            return;
        }

        if (!Xenoverse2::CommitCharaList(false, true))
        {
            DPRINTF("Failed to commit iggy.\n");
            return;
        }

        UPRINTF("Compilation succesful.\n");
    }
}

void MainWindow::on_actionSlot_editor_stages_triggered()
{
    std::vector<Xv2StageSlot> &stage_slots = game_stage_slots_file->GetSlots(); // Reference

    StageSeleDialog dialog(stage_slots,  this);

    if (dialog.exec())
    {
        stage_slots = dialog.GetEditedSlots();

        if (!Xenoverse2::CommitStageSlots())
        {
            DPRINTF("Failed to commit stage slots.\n");
        }
    }
}

void MainWindow::on_actionSlot_editor_stages_local_mode_triggered()
{
    std::vector<Xv2StageSlot> &stage_slots = game_stage_slots_file_local->GetSlots(); // Reference

    StageSeleDialog dialog(stage_slots,  this);

    if (dialog.exec())
    {
        stage_slots = dialog.GetEditedSlots();

        if (!Xenoverse2::CommitStageSlots())
        {
            DPRINTF("Failed to commit stage slots.\n");
        }
    }
}

void MainWindow::on_actionTriggerPortOover_triggered()
{
    RestorePart1();
}

void MainWindow::on_actionSet_dark_theme_triggered()
{
    ToggleDarkTheme(true);
}

