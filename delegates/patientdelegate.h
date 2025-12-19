#pragma once

#include <QStyledItemDelegate>

class PatientModel;

class PatientDelegate final : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit PatientDelegate(const PatientModel* model, QObject* parent = nullptr);

    QWidget* createEditor(QWidget* parent,
                          const QStyleOptionViewItem& option,
                          const QModelIndex& index) const override;
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
    void initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const override;

private:
    const PatientModel* m_patientModel = nullptr;
};
