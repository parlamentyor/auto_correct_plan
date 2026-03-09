#ifndef AVTORIZATION_H
#define AVTORIZATION_H

#include <QDialog>

namespace Ui {
class avtorization;
}

class avtorization : public QDialog
{
    Q_OBJECT

public:
    explicit avtorization(QWidget *parent = nullptr);
    ~avtorization();

private:
    Ui::avtorization *ui;
};

#endif // AVTORIZATION_H
