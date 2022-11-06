#include "DbTableDelegate.h"

// Qt
#include <QLineEdit>
#include <QSpinBox>
#include <QFileDialog>

namespace detail
{

template <typename T>
class EmptyAllowedValidator: public T
{
    using Super = T;
public:
    using Super::Super;

    QValidator::State validate(QString& input, int& pos) const override
    {
        if (input.isEmpty()) {
            return QValidator::Acceptable;
        }
        const auto superRes = Super::validate(input, pos);
        return superRes == QValidator::Intermediate ? QValidator::Invalid : superRes;
    }

};

} // detail

desktop::DbTableDelegate::DbTableDelegate(DbCellEditor& cellEditor, QObject* parent)
    : Super{parent}, fCellEditor{cellEditor},
      intIntervalValidator{std::make_unique<detail::EmptyAllowedValidator<QIntValidator>>()} {}

void desktop::DbTableDelegate::setTable(const core::VirtualTable* table)
{
    fCoreTable = table;
}

QWidget* desktop::DbTableDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (!fCoreTable || !index.isValid()) {
        return nullptr;
    }

    const auto columnIdx = index.column();
    const auto rowIdx = index.row();
    const auto& columnInfo = fCoreTable->column(columnIdx);

    switch (columnInfo.dataType()) {
    case core::DataType::INTEGER: {
        static const detail::EmptyAllowedValidator<QIntValidator> intValidator;
        auto* lineEdit = new QLineEdit(parent);
        lineEdit->setValidator(&intValidator);
        return lineEdit;
    }
    case core::DataType::REAL: {
        static const detail::EmptyAllowedValidator<QDoubleValidator> doubleValidator;
        auto* lineEdit = new QLineEdit(parent);
        lineEdit->setValidator(&doubleValidator);
        return lineEdit;
    }
    case core::DataType::CHAR: {
        auto* lineEdit = new QLineEdit(parent);
        lineEdit->setMaxLength(1);
        return lineEdit;
    }
    case core::DataType::STRING: {
        return new QLineEdit(parent);
    }
    case core::DataType::INTERVAL_INTEGER: {
        const auto& intervalColumnInfo = static_cast<const core::IntervalIntColumnInfo&>(columnInfo);
        intIntervalValidator->setRange(intervalColumnInfo.lowerLimit(), intervalColumnInfo.upperLimit());
        auto* lineEdit = new QLineEdit(parent);
        lineEdit->setValidator(intIntervalValidator.get());
        return lineEdit;
    }
    case core::DataType::TEXT_FILE: {
        const auto setFileWorker = [this, rowIdx, columnIdx] (const QString& fileName, const QByteArray& fileContent) {
            if (!fileName.isEmpty()) {
                core::File file;
                std::filesystem::path path(fileName.toStdWString());
                file.name = path.filename().wstring();
                file.data = fileContent.toStdString();
                fCellEditor.editCell(rowIdx, columnIdx, std::move(file));
            }
        };
        QFileDialog::getOpenFileContent("Select file", setFileWorker);
        return nullptr;
    }
    case core::DataType::NN: {
        throw std::logic_error("Invalid data type!");
    }
    }

    return nullptr;
}

void desktop::DbTableDelegate::setModelData(QWidget* editor, QAbstractItemModel*, const QModelIndex& index) const
{
    if (!fCoreTable || !index.isValid()) {
        return;
    }

    const auto rowIdx = index.row();
    const auto columnIdx = index.column();
    core::CellData data{};

    switch (fCoreTable->column(columnIdx).dataType()) {
        case core::DataType::INTEGER: {
            auto* lineEdit = static_cast<QLineEdit*>(editor);
            if (const auto text = lineEdit->text(); !text.isEmpty()) {
                data = core::CellData{core::column_t<core::DataType::INTEGER>(text.toInt())};
            }
        } break;
        case core::DataType::REAL: {
            auto* lineEdit = static_cast<QLineEdit*>(editor);
            if (const auto text = lineEdit->text().replace(',', '.'); !text.isEmpty()) {
                data = core::CellData{core::column_t<core::DataType::REAL>(text.toDouble())};
            }
        } break;
        case core::DataType::CHAR: {
            auto* lineEdit = static_cast<QLineEdit*>(editor);
            if (const auto text = lineEdit->text().replace(',', '.'); !text.isEmpty()) {
                static_assert(std::is_same_v<core::column_t<core::DataType::CHAR>, wchar_t>);
                data = core::CellData{(text.toStdWString()[0])};
            }
        } break;
        case core::DataType::STRING: {
            auto* lineEdit = static_cast<QLineEdit*>(editor);
            if (const auto text = lineEdit->text().replace(',', '.'); !text.isEmpty()) {
                static_assert(std::is_same_v<core::column_t<core::DataType::STRING>, std::wstring>);
                data = core::CellData{(text.toStdWString())};
            }
        } break;
        case core::DataType::INTERVAL_INTEGER: {
            auto* lineEdit = static_cast<QLineEdit*>(editor);
            if (const auto text = lineEdit->text(); !text.isEmpty()) {
                data = core::CellData{core::column_t<core::DataType::INTERVAL_INTEGER>{.data = text.toInt()}};
            }
        } break;
        case core::DataType::TEXT_FILE: break;
        case core::DataType::NN: {
            throw std::logic_error("Invalid data type!");
        }
    }

    fCellEditor.editCell(rowIdx, columnIdx, std::move(data));
}