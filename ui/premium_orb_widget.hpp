#pragma once

#include <QWidget>

class QTimer;

namespace pingram::ui {

class PremiumOrbWidget final : public QWidget {
    Q_OBJECT

public:
    explicit PremiumOrbWidget(QWidget* parent = nullptr);

    void set_speed_ms(int ms);
    void set_anim_enabled(bool enabled);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QTimer* timer_{nullptr};
    double phase_{0.0};
};

} // namespace pingram::ui
