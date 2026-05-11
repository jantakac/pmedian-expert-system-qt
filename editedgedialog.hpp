#ifndef EDITEDGEDIALOG_HPP
#define EDITEDGEDIALOG_HPP

#include <QDialog>
#include "graph.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {
class EditEdgeDialog;
}
QT_END_NAMESPACE

class EditEdgeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditEdgeDialog(const Edge &edge, QWidget *parent = nullptr);
    ~EditEdgeDialog() override;

    [[nodiscard]] Edge getUpdatedEdge() const;

private slots:
    void on_checkManual_toggled(bool checked);

private:
    Ui::EditEdgeDialog *ui;

    // Metadata for the Edge that shouldn't change via dialog
    EdgeId m_id;
    NodeId m_from;
    NodeId m_to;
};

#endif // EDITEDGEDIALOG_HPP