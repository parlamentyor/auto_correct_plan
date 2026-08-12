#ifndef FILTERWIDGET_H
#define FILTERWIDGET_H

#include <QWidget>
#include <QCheckBox>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

#include "model.h"

class FilterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FilterWidget(QWidget *parent = nullptr);

signals:
    void filterCompletedChanged(bool checked);
    void filterPaidChanged(bool checked);
    void filterTypeChanged(model::TypeContract type, bool checked);

private slots:
    void onCompletedClicked(bool checked);
    void onPaidClicked(bool checked);
    void onGozClicked(bool checked);
    void onAttClicked(bool checked);
    void onBekClicked(bool checked);
    void onSiClicked(bool checked);

private:
    QCheckBox* check_completed_;
    QCheckBox* check_paid_;
    QCheckBox* check_goz_;
    QCheckBox* check_att_;
    QCheckBox* check_bek_;
    QCheckBox* check_si_;
};

#endif // FILTERWIDGET_H
