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
    int exec(core::save_load::Information& saveInfo);

private slots:
    void refresh();
    void getMongoDbInfoFromUi();

private:
    void setInfo(const core::save_load::Information& info);
    core::save_load::Information getInfo();

    std::unique_ptr<Ui::SaveDatabaseDialog> ui;

    std::optional<core::save_load::CustomFileInfo> customFileInfo;
    std::optional<core::save_load::SQLiteInfo> sqliteDbInfo;
    std::optional<core::save_load::MongoDbInfo> mongoDbInfo;
};

} // desktop

#endif // SAVEDATABASEDIALOG_H
