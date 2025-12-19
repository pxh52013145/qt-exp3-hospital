#include "patientdelegate.h"

#include "models/patientmodel.h"

#include <QComboBox>
#include <QDate>
#include <QDateEdit>
#include <QDoubleSpinBox>
#include <QStyleOptionViewItem>
#include <QSpinBox>

PatientDelegate::PatientDelegate(const PatientModel* model, QObject* parent)
    : QStyledItemDelegate(parent), m_patientModel(model)
{
}

QWidget* PatientDelegate::createEditor(QWidget* parent,
                                       const QStyleOptionViewItem& option,
                                       const QModelIndex& index) const
{
    Q_UNUSED(option);
    if (!m_patientModel) {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }

    const auto col = index.column();
    if (col == m_patientModel->sexColumn()) {
        auto* cb = new QComboBox(parent);
        cb->addItem(QStringLiteral("女"), 0);
        cb->addItem(QStringLiteral("男"), 1);
        return cb;
    }
    if (col == m_patientModel->dobColumn()) {
        auto* de = new QDateEdit(parent);
        de->setCalendarPopup(true);
        de->setDisplayFormat(QStringLiteral("yyyy/M/d"));
        de->setDate(QDate::currentDate());
        return de;
    }
    if (col == m_patientModel->heightColumn()) {
        auto* sb = new QDoubleSpinBox(parent);
        sb->setRange(0.0, 250.0);
        sb->setDecimals(1);
        return sb;
    }
    if (col == m_patientModel->weightColumn()) {
        auto* sb = new QDoubleSpinBox(parent);
        sb->setRange(0.0, 300.0);
        sb->setDecimals(1);
        return sb;
    }
    if (col == m_patientModel->ageColumn()) {
        auto* sb = new QSpinBox(parent);
        sb->setRange(0, 150);
        return sb;
    }
    return QStyledItemDelegate::createEditor(parent, option, index);
}

void PatientDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    if (!m_patientModel) {
        return QStyledItemDelegate::setEditorData(editor, index);
    }
    const auto col = index.column();

    if (col == m_patientModel->sexColumn()) {
        auto* cb = qobject_cast<QComboBox*>(editor);
        const int value = index.data(Qt::EditRole).toInt();
        cb->setCurrentIndex(value == 1 ? 1 : 0);
        return;
    }
    if (col == m_patientModel->dobColumn()) {
        auto* de = qobject_cast<QDateEdit*>(editor);
        const auto s = index.data(Qt::EditRole).toString();
        const auto d = QDate::fromString(s, Qt::ISODate);
        de->setDate(d.isValid() ? d : QDate::currentDate());
        return;
    }
    if (col == m_patientModel->heightColumn() || col == m_patientModel->weightColumn()) {
        auto* sb = qobject_cast<QDoubleSpinBox*>(editor);
        sb->setValue(index.data(Qt::EditRole).toDouble());
        return;
    }
    if (col == m_patientModel->ageColumn()) {
        auto* sb = qobject_cast<QSpinBox*>(editor);
        sb->setValue(index.data(Qt::EditRole).toInt());
        return;
    }

    QStyledItemDelegate::setEditorData(editor, index);
}

void PatientDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    if (!m_patientModel) {
        return QStyledItemDelegate::setModelData(editor, model, index);
    }
    const auto col = index.column();

    if (col == m_patientModel->sexColumn()) {
        auto* cb = qobject_cast<QComboBox*>(editor);
        model->setData(index, cb->currentData().toInt(), Qt::EditRole);
        return;
    }
    if (col == m_patientModel->dobColumn()) {
        auto* de = qobject_cast<QDateEdit*>(editor);
        model->setData(index, de->date().toString(Qt::ISODate), Qt::EditRole);
        return;
    }
    if (col == m_patientModel->heightColumn() || col == m_patientModel->weightColumn()) {
        auto* sb = qobject_cast<QDoubleSpinBox*>(editor);
        model->setData(index, sb->value(), Qt::EditRole);
        return;
    }
    if (col == m_patientModel->ageColumn()) {
        auto* sb = qobject_cast<QSpinBox*>(editor);
        model->setData(index, sb->value(), Qt::EditRole);
        return;
    }

    QStyledItemDelegate::setModelData(editor, model, index);
}

void PatientDelegate::initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const
{
    QStyledItemDelegate::initStyleOption(option, index);
    if (!m_patientModel || !option) {
        return;
    }
    if (index.column() == m_patientModel->sexColumn()) {
        const int v = index.data(Qt::DisplayRole).toInt();
        option->text = (v == 1) ? QStringLiteral("男") : QStringLiteral("女");
    }
}
