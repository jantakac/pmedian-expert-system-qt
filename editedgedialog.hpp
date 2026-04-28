#ifndef EDITEDGEDIALOG_HPP
#define EDITEDGEDIALOG_HPP

#include <QDialog>

namespace Ui {
class EditEdgeDialog;
}

class EditEdgeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditEdgeDialog(QWidget *parent = nullptr);
    ~EditEdgeDialog();

private:
    Ui::EditEdgeDialog *ui;
};

#endif // EDITEDGEDIALOG_HPP
