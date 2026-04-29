#include "editedgedialog.hpp"
#include "ui_editedgedialog.h"

EditEdgeDialog::EditEdgeDialog(const Edge &edge, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::EditEdgeDialog)
{
    ui->setupUi(this);
    ui->spinId->setValue(edge.id);
    ui->spinFrom->setValue(edge.from);
    ui->spinTo->setValue(edge.to);
    ui->spinLength->setValue(edge.length);
    ui->checkEnabled->setChecked(edge.isEnabled);
    ui->checkManualLength->setChecked(edge.isLengthManual);
}

EditEdgeDialog::~EditEdgeDialog()
{
    delete ui;
}

Edge EditEdgeDialog::editedEdge() const
{
    Edge edge(ui->spinId->value(),
              ui->spinFrom->value(),
              ui->spinTo->value(),
              ui->spinLength->value());
    edge.isEnabled = ui->checkEnabled->isChecked();
    edge.isLengthManual = ui->checkManualLength->isChecked();
    return edge;
}