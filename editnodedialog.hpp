#ifndef EDITNODEDIALOG_HPP
#define EDITNODEDIALOG_HPP

#include "node.hpp"

#include <QDialog>

namespace Ui {
class EditNodeDialog;
}

class EditNodeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditNodeDialog(const Node &nodeToEdit, QWidget *parent = nullptr);
    ~EditNodeDialog();

    Node editedNode() const;

private:
    Ui::EditNodeDialog *ui;
};

#endif // EDITNODEDIALOG_HPP
