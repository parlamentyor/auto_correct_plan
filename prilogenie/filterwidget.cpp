#include "filterwidget.h"

FilterWidget::FilterWidget(QWidget *parent)
    : QWidget{parent}
{
    // Инициализация дат
    date_start_.day_ = 1;
    date_start_.month_ = 1;
    date_start_.year_ = QDate::currentDate().year();

    date_end_.day_ = QDate::currentDate().day();
    date_end_.month_ = QDate::currentDate().month();
    date_end_.year_ = QDate::currentDate().year();

    QHBoxLayout* main_layout = new QHBoxLayout(this);

    // Группа статуса
    QGroupBox* status_group = new QGroupBox("Статус");
    QHBoxLayout* status_layout = new QHBoxLayout(status_group);

    QVBoxLayout* completed_layout = new QVBoxLayout();
    QVBoxLayout* actual_layout = new QVBoxLayout();
    QVBoxLayout* paid_layout = new QVBoxLayout();

    check_completed_ = new QCheckBox("Выполненные");
    check_not_completed_ = new QCheckBox("Не выполненные");
    check_paid_ = new QCheckBox("Оплаченные");
    check_not_paid_ = new QCheckBox("Не оплаченные");
    check_actual_ = new QCheckBox("Актуальные");
    check_not_actual_ = new QCheckBox("Не Актуальные");

    // Устанавливаем галочки сразу
    check_completed_->setChecked(true);
    check_not_completed_->setChecked(true);
    check_paid_->setChecked(true);
    check_not_paid_->setChecked(true);
    check_actual_->setChecked(true);
    check_not_actual_->setChecked(true);

    // Заполняем вертикальные лейауты
    completed_layout->addWidget(check_completed_);
    completed_layout->addWidget(check_not_completed_);

    actual_layout->addWidget(check_actual_);
    actual_layout->addWidget(check_not_actual_);

    paid_layout->addWidget(check_paid_);
    paid_layout->addWidget(check_not_paid_);

    // Добавляем вертикальные лейауты в горизонтальный status_layout
    status_layout->addLayout(completed_layout);
    status_layout->addLayout(actual_layout);
    status_layout->addLayout(paid_layout);

    // Группа типа договора
    QGroupBox* type_group = new QGroupBox("Тип договора");
    QHBoxLayout* type_layout = new QHBoxLayout(type_group);

    check_goz_ = new QCheckBox("ГОЗ");
    check_att_ = new QCheckBox("АТТ");
    check_bek_ = new QCheckBox("БЭК");
    check_si_ = new QCheckBox("СИ");

    // Устанавливаем галочки сразу
    check_goz_->setChecked(true);
    check_att_->setChecked(true);
    check_bek_->setChecked(true);
    check_si_->setChecked(true);

    type_layout->addWidget(check_goz_);
    type_layout->addWidget(check_att_);
    type_layout->addWidget(check_bek_);
    type_layout->addWidget(check_si_);

    // Группа период
    QGroupBox* period_group = new QGroupBox("Период");
    QVBoxLayout* period_layout = new QVBoxLayout(period_group);

    // Первый ряд - дата начала
    QHBoxLayout* start_layout = new QHBoxLayout();
    QLabel* label_start = new QLabel("с");
    QDateEdit* date_start_edit = new QDateEdit();
    date_start_edit->setDate(QDate(date_start_.year_, date_start_.month_, date_start_.day_));
    date_start_edit->setCalendarPopup(true);
    // Устанавливаем формат отображения даты
    date_start_edit->setDisplayFormat("dd.MM.yyyy");
    // Устанавливаем минимальную ширину, чтобы дата помещалась полностью
    date_start_edit->setMinimumWidth(120);
    start_layout->addWidget(label_start);
    start_layout->addWidget(date_start_edit);

    // Второй ряд - дата окончания
    QHBoxLayout* end_layout = new QHBoxLayout();
    QLabel* label_end = new QLabel("по");
    QDateEdit* date_end_edit = new QDateEdit();
    date_end_edit->setDate(QDate(date_end_.year_, date_end_.month_, date_end_.day_));
    date_end_edit->setCalendarPopup(true);
    // Устанавливаем формат отображения даты
    date_end_edit->setDisplayFormat("dd.MM.yyyy");
    // Устанавливаем минимальную ширину, чтобы дата помещалась полностью
    date_end_edit->setMinimumWidth(120);
    end_layout->addWidget(label_end);
    end_layout->addWidget(date_end_edit);

    period_layout->addLayout(start_layout);
    period_layout->addLayout(end_layout);

    main_layout->addWidget(period_group);
    main_layout->addWidget(status_group);
    main_layout->addWidget(type_group);
    main_layout->addStretch();

    // Подключаем сигналы
    connect(check_completed_, &QCheckBox::clicked, this, &FilterWidget::onCompletedClicked);
    connect(check_not_completed_, &QCheckBox::clicked, this, &FilterWidget::onNotCompletedClicked);
    connect(check_paid_, &QCheckBox::clicked, this, &FilterWidget::onPaidClicked);
    connect(check_not_paid_, &QCheckBox::clicked, this, &FilterWidget::onNotPaidClicked);
    connect(check_actual_, &QCheckBox::clicked, this, &FilterWidget::onActualClicked);
    connect(check_not_actual_, &QCheckBox::clicked, this, &FilterWidget::onNotActualClicked);
    connect(check_goz_, &QCheckBox::clicked, this, &FilterWidget::onGozClicked);
    connect(check_att_, &QCheckBox::clicked, this, &FilterWidget::onAttClicked);
    connect(check_bek_, &QCheckBox::clicked, this, &FilterWidget::onBekClicked);
    connect(check_si_, &QCheckBox::clicked, this, &FilterWidget::onSiClicked);
}

void FilterWidget::onCompletedClicked(bool checked) {
    emit filterCompletedChanged(checked);
}

void FilterWidget::onNotCompletedClicked(bool checked) {
    emit filterNotCompletedChanged(checked);
}

void FilterWidget::onPaidClicked(bool checked) {
    emit filterPaidChanged(checked);
}

void FilterWidget::onNotPaidClicked(bool checked) {
    emit filterNotPaidChanged(checked);
}

void FilterWidget::onActualClicked(bool checked) {
    emit filterActualChanged(checked);
}

void FilterWidget::onNotActualClicked(bool checked) {
    emit filterNotActualChanged(checked);
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
