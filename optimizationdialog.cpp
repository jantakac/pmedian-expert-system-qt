#include "OptimizationDialog.hpp"
#include <QMessageBox>
#include "ui_OptimizationDialog.h"

OptimizationDialog::OptimizationDialog(QWidget *parent)
    : QDialog{parent}
    , ui{std::make_unique<Ui::OptimizationDialog>()}
{
    ui->setupUi(this);
}

OptimizationDialog::~OptimizationDialog() = default;

OptimizationParams OptimizationDialog::params() const
{
    return OptimizationParams{.pMedians = ui->spinP->value(),
                              .costPerKm = ui->doubleSpinCost->value(),
                              .tMax = ui->doubleSpinTMax->value(),
                              .tMin = ui->doubleSpinTMin->value(),
                              .coolingFactor = ui->doubleSpinCooling->value(),
                              .iterationsPerStep = ui->spinIterations->value()};
}

void OptimizationDialog::accept()
{
    const auto p = params();

    if (p.tMax <= p.tMin) {
        QMessageBox::critical(this,
                              "Validation Error",
                              "Max Temperature must be greater than Min Temperature.");
        return;
    }

    QDialog::accept();
}