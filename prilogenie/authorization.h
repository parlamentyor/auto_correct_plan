#ifndef AUTHORIZATION_H
#define AUTHORIZATION_H

#include <QMainWindow>

#include"app.h"

namespace Ui {
class Authorization;
}

class Authorization : public QMainWindow
{
    Q_OBJECT

public:
    explicit Authorization(std::shared_ptr<app::App> app, QWidget *parent = nullptr);
    ~Authorization();

signals:
    void loginSuccess();

private slots:
    void on_pb_authorization_clicked();

    void on_pb_create_user_clicked();

private:
    Ui::Authorization *ui;
    std::shared_ptr<app::App> app_;
};

#endif // AUTHORIZATION_H
