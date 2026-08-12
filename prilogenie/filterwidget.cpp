#include "filterwidget.h"

FilterWidget::FilterWidget(QWidget *parent)
    : QWidget{parent}
{
    QHBoxLayout* main_layout = new QHBoxLayout(this);

    // Группа статуса
    QGroupBox* status_group = new QGroupBox("Статус");
    QHBoxLayout* statusLayout = new QHBoxLayout(status_group);

    check_completed_ = new QCheckBox("Только выполненные");
    check_paid_ = new QCheckBox("Только оплаченные");

    statusLayout->addWidget(check_completed_);
    statusLayout->addWidget(check_paid_);

    // Группа типа договора
    QGroupBox* type_group = new QGroupBox("Тип договора");
    QHBoxLayout* type_layout = new QHBoxLayout(type_group);

    check_goz_ = new QCheckBox("ГОЗ");
    check_att_ = new QCheckBox("АТТ");
    check_bek_ = new QCheckBox("БЭК");
    check_si_ = new QCheckBox("СИ");

    type_layout->addWidget(check_goz_);
    type_layout->addWidget(check_att_);
    type_layout->addWidget(check_bek_);
    type_layout->addWidget(check_si_);

    main_layout->addWidget(status_group);
    main_layout->addWidget(type_group);
    main_layout->addStretch();

    // Подключаем сигналы
    connect(check_completed_, &QCheckBox::clicked, this, &FilterWidget::onCompletedClicked);
    connect(check_paid_, &QCheckBox::clicked, this, &FilterWidget::onPaidClicked);
    connect(check_goz_, &QCheckBox::clicked, this, &FilterWidget::onGozClicked);
    connect(check_att_, &QCheckBox::clicked, this, &FilterWidget::onAttClicked);
    connect(check_bek_, &QCheckBox::clicked, this, &FilterWidget::onBekClicked);
    connect(check_si_, &QCheckBox::clicked, this, &FilterWidget::onSiClicked);
}

void FilterWidget::onCompletedClicked(bool checked) {
    emit filterCompletedChanged(checked);
}

void FilterWidget::onPaidClicked(bool checked) {
    emit filterPaidChanged(checked);
}

void FilterWidget::onGozClicked(bool checked) {
    emit filterTypeChanged(model::TypeContract::GOZ, checked);
}

void FilterWidget::onAttClicked(bool checked) {
    emit filterTypeChanged(model::TypeContract::ATT, checked);
}

void FilterWidget::onBekClicked(bool checked) {
    emit filterTypeChanged(model::TypeContract::BEK, checked);
}

void FilterWidget::onSiClicked(bool checked) {
    emit filterTypeChanged(model::TypeContract::SI, checked);
}
