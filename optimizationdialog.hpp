#ifndef OPTIMIZATIONDIALOG_HPP
#define OPTIMIZATIONDIALOG_HPP

#include <QDialog>
#include "OptimizationParams.hpp"
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui {
class OptimizationDialog;
}
QT_END_NAMESPACE

class OptimizationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptimizationDialog(QWidget *parent = nullptr);
    ~OptimizationDialog() override;

    [[nodiscard]] OptimizationParams params() const;

public slots:
    void accept() override;

private:
    std::unique_ptr<Ui::OptimizationDialog> ui;
};

#endif // OPTIMIZATIONDIALOG_HPP
