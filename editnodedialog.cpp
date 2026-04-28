#include "editnodedialog.hpp"
#include "ui_editnodedialog.h"

EditNodeDialog::EditNodeDialog(const Node &nodeToEdit, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::EditNodeDialog)
{
    ui->setupUi(this);

    ui->comboType->addItem("Customer", static_cast<int>(NodeType::Customer));
    ui->comboType->addItem("P-Median Candidate", static_cast<int>(NodeType::PMedianCandidate));

    ui->spinId->setValue(nodeToEdit.id);
    ui->spinX->setValue(nodeToEdit.pos.x());
    ui->spinY->setValue(nodeToEdit.pos.y());

    int typeIndex = ui->comboType->findData(static_cast<int>(nodeToEdit.type));
    ui->comboType->setCurrentIndex(typeIndex);
}

EditNodeDialog::~EditNodeDialog()
{
    delete ui;
}

Node EditNodeDialog::editedNode() const
{
    Node node({ui->spinX->value(), ui->spinY->value()}, static_cast<uint32_t>(ui->spinId->value()));
    node.type = static_cast<NodeType>(ui->comboType->currentData().toInt());

    return node;
}
