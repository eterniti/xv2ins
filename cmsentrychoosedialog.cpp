#include "cmsentrychoosedialog.h"
#include "ui_cmsentrychoosedialog.h"

#include <Xenoverse2.h>

CmsEntryChooseDialog::CmsEntryChooseDialog(std::string original_code, bool check_exist_in_game, QWidget *parent) :
    QDialog(parent), original_code(original_code), check_exist_in_game(check_exist_in_game),
    ui(new Ui::CmsEntryChooseDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

CmsEntryChooseDialog::~CmsEntryChooseDialog()
{
    delete ui;
}

void CmsEntryChooseDialog::on_buttonBox_accepted()
{
    QString code = ui->cmsEntryEdit->text().trimmed().toUpper();

    if (code.isEmpty())
    {
        result.clear();

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
                        if (result.size() == 0 && b == 'X')
                            continue;

                        result += b;
                        break;
                    }
                }
            } // end for

            if (result != original_code && !Xenoverse2::IsOriginalChara(result) && !game_cms->FindEntryByName(result))
                break;
        }
    }
    else
    {
        if (code.length() != 3)
        {
            DPRINTF("The code must be 3-letter. Leave blank to autogenerate.\n");
            return;
        }

        result = Utils::QStringToStdString(code);

        if (result == original_code)
        {
            DPRINTF("You specified the same 3-letter that the mod already has.\n");
            return;
        }

        if (Xenoverse2::IsOriginalChara(result))
        {
            DPRINTF("The code you specified is from an original character and cannot be used.\n");
            return;
        }

        if (check_exist_in_game && game_cms->FindEntryByName(result))
        {
            DPRINTF("The code you specified already exists in your mods installation.\n");
            return;
        }

        for (int i = 0; i < (int)result.length(); i++)
        {
            if (!( (result[i] >= '0' && result[i] <= '9') || (result[i] >= 'A' && result[i] <= 'Z') ))
            {
                DPRINTF("The 3-letter code can only use A-Z and 0-9 chars. (no spaces, no symbols, no tildes or non-ascii chars)\n");
                return;
            }
        }
    }

    accept();
}
