#ifndef EDITNODEDIALOG_HPP
#define EDITNODEDIALOG_HPP

#include <QDialog>
#include "graph.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {
class EditNodeDialog;
}
QT_END_NAMESPACE

class EditNodeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditNodeDialog(const Node &node, QWidget *parent = nullptr);
    ~EditNodeDialog() override;

    // Returns the modified node data
    [[nodiscard]] Node getUpdatedNode() const;

private:
    Ui::EditNodeDialog *ui;
    NodeId m_id; // Keep ID immutable
};

#endif // EDITNODEDIALOG_HPP