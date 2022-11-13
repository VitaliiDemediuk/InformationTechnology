#ifndef LOADFROMMONGODBDIALOG_H
#define LOADFROMMONGODBDIALOG_H

// Qt
#include <QDialog>

// Core
#include "VirtualDatabase.h"

namespace core {
    class VirtualColumnNameValidator;
}

namespace Ui {
    class LoadFromMongoDbDialog;
}

namespace desktop
{

class LoadFromMongoDbDialog: public QDialog
{
    Q_OBJECT

    using Super = QDialog;
public:
    explicit LoadFromMongoDbDialog(const core::VirtualDatabaseNameValidator& validator, QWidget* parent = nullptr);
    ~LoadFromMongoDbDialog() override;

    using Super::exec;
    int exec(core::save_load::MongoDbInfo& saveInfo);

private slots:
    void reenable();

private:
    const core::VirtualDatabaseNameValidator& fValidator;
    std::unique_ptr<Ui::LoadFromMongoDbDialog> ui;
};

} // desktop

#endif // LOADFROMMONGODBDIALOG_H
