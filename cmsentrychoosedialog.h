#ifndef CMSENTRYCHOOSEDIALOG_H
#define CMSENTRYCHOOSEDIALOG_H

#include <QDialog>

namespace Ui {
class CmsEntryChooseDialog;
}

class CmsEntryChooseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CmsEntryChooseDialog(std::string original_code, bool check_exist_in_game, QWidget *parent = 0);
    ~CmsEntryChooseDialog();

    inline std::string GetResult()
    {
        return result;
    }

private slots:
    void on_buttonBox_accepted();

private:
    Ui::CmsEntryChooseDialog *ui;

    std::string original_code;
    bool check_exist_in_game;
    std::string result;

};

#endif // CMSENTRYCHOOSEDIALOG_H
