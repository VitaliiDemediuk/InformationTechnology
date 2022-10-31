#ifndef COLUMNINFODIALOG_H
#define COLUMNINFODIALOG_H

// Qt
#include <QDialog>



namespace core {
    class VirtualColumnNameValidator;
    class VirtualColumnInfo;
}

namespace Ui {
    class ColumnInfoDialog;
}

namespace desktop
{

class ColumnInfoDialog: public QDialog
{
    Q_OBJECT

    using Super = QDialog;
public:
    explicit ColumnInfoDialog(const core::VirtualColumnNameValidator& validator, QWidget* parent = nullptr);
    ~ColumnInfoDialog() override;

    using Super::exec;
    int exec(std::unique_ptr<core::VirtualColumnInfo>& columnInfo);
    int exec(const core::VirtualColumnInfo& columnInfo, std::wstring& name);

private slots:
    void reenableButtonBox();

private:
    void reenable(bool isEnable);

    std::unique_ptr<Ui::ColumnInfoDialog> ui;
    const core::VirtualColumnNameValidator& fValidator;
};

} // desktop

#endif // COLUMNINFODIALOG_H
