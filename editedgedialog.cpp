#include "editedgedialog.hpp"
#include "ui_editedgedialog.h"

EditEdgeDialog::EditEdgeDialog(const Edge &edge, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::EditEdgeDialog)
    , m_id(edge.id)
    , m_from(edge.from)
    , m_to(edge.to)
{
    ui->setupUi(this);

    // Set Initial Values
    ui->checkEnabled->setChecked(edge.isEnabled);
    ui->checkManual->setChecked(edge.isLengthManual);
    ui->doubleSpinLength->setValue(static_cast<double>(edge.length));

    // Logic: If length is not manual, the spinbox should be read-only
    ui->doubleSpinLength->setEnabled(edge.isLengthManual);

    setWindowTitle(QString("Edit Edge #%1").arg(static_cast<uint32_t>(m_id)));
}

EditEdgeDialog::~EditEdgeDialog()
{
    delete ui;
}

void EditEdgeDialog::on_checkManual_toggled(bool checked)
{
    ui->doubleSpinLength->setEnabled(checked);
}

Edge EditEdgeDialog::getUpdatedEdge() const
{
    return Edge{.id = m_id,
                .from = m_from,
                .to = m_to,
                .length = static_cast<float>(ui->doubleSpinLength->value()),
                .isEnabled = ui->checkEnabled->isChecked(),
                .isLengthManual = ui->checkManual->isChecked()};
}