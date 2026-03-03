#include "ui/premium_orb_widget.hpp"

#include <QPainter>
#include <QPaintEvent>
#include <QTimer>

namespace pingram::ui {

PremiumOrbWidget::PremiumOrbWidget(QWidget* parent) : QWidget(parent) {
    setMinimumSize(96, 96);
    setMaximumSize(120, 120);

    timer_ = new QTimer(this);
    timer_->setInterval(30);
    connect(timer_, &QTimer::timeout, this, [this]() {
        phase_ += 3.6;
        if (phase_ >= 360.0) {
            phase_ -= 360.0;
        }
        update();
    });
    timer_->start();
}

void PremiumOrbWidget::set_speed_ms(int ms) {
    timer_->setInterval(ms < 16 ? 16 : ms / 8);
}

void PremiumOrbWidget::set_anim_enabled(bool enabled) {
    if (enabled) {
        timer_->start();
    } else {
        timer_->stop();
        update();
    }
}

void PremiumOrbWidget::paintEvent(QPaintEvent* event) {
    QWidget::paintEvent(event);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const QRectF r = rect().adjusted(8, 8, -8, -8);
    const QPointF c = r.center();
    const qreal rad = qMin(r.width(), r.height()) * 0.48;

    p.translate(c);
    p.rotate(phase_);

    QRadialGradient orb_grad(QPointF(-rad * 0.2, -rad * 0.25), rad);
    orb_grad.setColorAt(0.0, QColor("#ede9fe"));
    orb_grad.setColorAt(0.45, QColor("#a855f7"));
    orb_grad.setColorAt(1.0, QColor("#5b21b6"));

    p.setPen(QPen(QColor("#4c1d95"), 1.3));
    p.setBrush(orb_grad);
    p.drawEllipse(QPointF(0, 0), rad, rad);

    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(QColor(255, 255, 255, 135), 2.0));
    p.drawEllipse(QRectF(-rad * 0.75, -rad * 0.35, rad * 1.5, rad * 0.7));
    p.drawEllipse(QRectF(-rad * 0.35, -rad * 0.75, rad * 0.7, rad * 1.5));

    p.setPen(Qt::NoPen);
    p.setBrush(QColor(255, 255, 255, 90));
    p.drawEllipse(QPointF(-rad * 0.25, -rad * 0.28), rad * 0.22, rad * 0.18);
}

} // namespace pingram::ui
