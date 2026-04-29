#ifndef EDITEDGEDIALOG_HPP
#define EDITEDGEDIALOG_HPP

#include <QDialog>
#include "edge.hpp"

namespace Ui {
class EditEdgeDialog;
}

class EditEdgeDialog : public QDialog
{
    Q_OBJECT
public:
    explicit EditEdgeDialog(const Edge &edge, QWidget *parent = nullptr);
    ~EditEdgeDialog();
    Edge editedEdge() const;

private:
    Ui::EditEdgeDialog *ui;
};
#endif
