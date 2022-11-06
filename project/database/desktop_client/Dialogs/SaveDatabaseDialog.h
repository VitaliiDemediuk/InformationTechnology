#ifndef SAVEDATABASEDIALOG_H
#define SAVEDATABASEDIALOG_H

// Qt
#include <QDialog>

// Core
#include "VirtualDatabase.h"

namespace core {
    class VirtualColumnNameValidator;
    class VirtualColumnInfo;
}

namespace Ui {
    class SaveDatabaseDialog;
}

namespace desktop
{

class SaveDatabaseDialog: public QDialog
{
    Q_OBJECT

    using Super = QDialog;
public:
    explicit SaveDatabaseDialog(QWidget* parent = nullptr);
    ~SaveDatabaseDialog() override;

    using Super::exec;
    int exec(core::SaveInformation& saveInfo);

private slots:
    void refresh();

private:
    void setInfo(const core::SaveInformation& info);
    core::SaveInformation getInfo();

    std::unique_ptr<Ui::SaveDatabaseDialog> ui;

    std::optional<std::filesystem::path> customFilePath;
};

} // desktop

#endif // SAVEDATABASEDIALOG_H
