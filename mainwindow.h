#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidget>
#include <QLabel>

#include "X2mFile.h"
#include "Xv2QuestCompiler.h"
#include "logdialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool Initialize();
    void ClearEpatches();
    bool ClearInstallation(bool special_mode);
    void RestorePart1();    
    void RestoreMods(const std::vector<std::string> &paths, X2mType type, const std::string &label, int &global_success, int &global_errors, std::vector<std::string> &failures);
    bool RestoreCharaSlots(const std::string &recover_path);
    bool RestoreStageSlots(const std::string &recover_path, bool local);
    void RestorePart2(const QString &recover_path);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void on_actionOpen_triggered();

    void on_actionExit_triggered();

    void on_actionAbout_triggered();

    void on_modsList_itemSelectionChanged();

    void on_actionUninstall_triggered();

    void on_actionAssociate_x2m_extension_triggered();

    void on_actionSlot_editor_triggered();

    void on_actionFind_and_delete_dead_ids_triggered();

    void on_actionClear_installation_triggered();

    void on_actionRemoveEmptyDirs_triggered();

    void on_actionChange_3_letter_code_of_x2m_triggered();

    void on_actionConvert_charalist_triggered();

    void on_actionCompile_INTERNAL_folder_triggered();

    void on_actionSlot_editor_stages_triggered();

    void on_actionSlot_editor_stages_local_mode_triggered();

    void on_actionTriggerPortOover_triggered();

    void on_actionSet_dark_theme_triggered();

private:
    Ui::MainWindow *ui;
    QLabel *statusLabel;
    bool invisible_mode;
    QMutex ui_mutex;
    bool process_selection_change = false;

    std::vector<SevTableEntryHL> sev_hl_table;
    std::vector<SevTableEntryLL> sev_ll_table;

    std::vector<TtbTableEntryHL> ttb_hl_table;
    std::vector<TtbTableEntryLL> ttb_ll_table;
    std::unordered_map<std::string, uint32_t> character_mod_map; // GUID 2 cms map

    struct ModEntry
    {
        std::string package_path;
        QString name;
        QString author;
        float version;
        QString guid;
        QString entry_name;
        X2mType type;
        time_t last_modified;

        // /// Depends section
        std::vector<X2mDepends> depends; // For skill or super soul, this vector will either be 0 or 1 sized.

        // This needed to update skills of characters "after"
        std::vector<CusSkillSet> skill_sets;  // Only for chara mods with skill depends
        // This needed to update supersoul of characters "after"
        std::vector<PscSpecEntry> psc_entries;  // Only for chara mods with supersoul depends

        // This one needed along depends to update costume of skills "after"
        std::vector<std::pair<CusSkill, X2mSkillType>> skill_entry; // Only for skill mods with costume or character depends. This should be a 0 or 1 sized vector.
        // ///

        int index;
        QTreeWidgetItem *item;
    };

    QVector<ModEntry> installed_mods;
    Xv2QuestCompiler qc;

    size_t temp_empty_found; // For delete empty dir
    size_t temp_empty_deleted; // For delete empty dir
    bool temp_empty_yesall; // For delete empty dir

    std::string GetInstalledModsPath();

    bool ProcessShutdown();
    void UpdateStatus();

    bool CheckCharaSele();
    void CheckRegistryAssociation();

    void ModToGui(ModEntry &entry);
    ModEntry *GetSelectedMod();
    size_t GetSelectedMods(std::vector<int> &mods);

    static void AddModToQuestCompiler(X2mFile &x2m, Xv2QuestCompiler &qc, const std::string &path);
    static bool LoadVisitor(const std::string &path, bool, void *param);
    uint8_t *GetAudio(AcbFile *acb, AwbFile *awb, uint32_t cue_id, size_t *psize);   
    void LoadInstalledMods(const QString &restore_path="");

    bool ExtractQuestFiles(X2mFile &x2m, std::string &qs, std::string &cs, std::string &ds, std::string &ps, std::vector<std::string> &svec, std::vector<std::string> &sfvec);

    ModEntry *FindModByGuid(const QString &guid);
    ModEntry *FindModByEntry(const QString &entry);

    bool CanUpdateSlots(const X2mFile &new_mod, const X2mFile &old_mod) const;
    bool CanUpdateCostume(const X2mFile &new_mod, const X2mCostumeEntry *entry) const;
    void ClearAttachments(std::vector<X2mFile *> &attachments);
    void PostProcessSkill(X2mFile *x2m);
    void PostProcessCostume(X2mFile *x2m);
    void PostProcessSuperSoul(X2mFile *x2m);
    void PostProcessCharacter(X2mFile *x2m);
    ModEntry *InstallMod(const QString &path, ModEntry *reinstall_mod, bool silent_mode, bool restore_mode=false);

    void RemoveDeadItem();
    void PostProcessCostumeUninstall(X2mFile *x2m, const X2mCostumeEntry &costume);
    void PostProcessSkillUninstall(X2mFile *x2m);
    void PostProcessCharacterUninstall(X2mFile *x2m);
    bool UninstallMod(const ModEntry &mod, bool remove_empty_dir, bool restore_mode=false);

    bool MultipleModsQuestion();

    static bool DeleteEmptyDirVisitor(const std::string &path, bool, void *param);

    bool ConvertModEntry(X2mFile *x2m, const std::string &new_entry);

    void RestartProgram(const QStringList &args = {});
    void ToggleDarkTheme(bool update_config);
};

#endif // MAINWINDOW_H
