#include "editedgedialog.hpp"
#include "ui_editedgedialog.h"

EditEdgeDialog::EditEdgeDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::EditEdgeDialog)
{
    ui->setupUi(this);
}

EditEdgeDialog::~EditEdgeDialog()
{
    delete ui;
}
