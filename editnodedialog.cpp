#include "editnodedialog.hpp"
#include "ui_editnodedialog.h"

EditNodeDialog::EditNodeDialog(const Node &node, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::EditNodeDialog)
    , m_id(node.id)
{
    ui->setupUi(this);

    ui->comboType->addItem("Customer", static_cast<int>(NodeType::Customer));
    ui->comboType->addItem("P-Median Candidate", static_cast<int>(NodeType::PMedianCandidate));

    ui->doubleSpinX->setValue(node.pos.x());
    ui->doubleSpinY->setValue(node.pos.y());

    int typeIndex = ui->comboType->findData(static_cast<int>(node.type));
    ui->comboType->setCurrentIndex(typeIndex != -1 ? typeIndex : 0);

    setWindowTitle(QString("Edit Node #%1").arg(static_cast<uint32_t>(m_id)));
}

EditNodeDialog::~EditNodeDialog()
{
    delete ui;
}

Node EditNodeDialog::getUpdatedNode() const
{
    return Node{.id = m_id,
                .pos = QPointF(ui->doubleSpinX->value(), ui->doubleSpinY->value()),
                .type = static_cast<NodeType>(ui->comboType->currentData().toInt())};
}