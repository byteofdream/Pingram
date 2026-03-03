#pragma once

#include "api/auth/auth_requests.hpp"
#include "api/messages/messages_requests.hpp"
#include "transport/http_client.hpp"

#include <QMainWindow>

class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QPushButton;
class QSlider;
class QStackedWidget;
class QTabWidget;
class QTextEdit;
class QWidget;

namespace pingram::ui {

class PremiumOrbWidget;

class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void on_login_clicked();
    void on_send_clicked();
    void on_chat_selected(QListWidgetItem* item);
    void on_chat_filter_changed(const QString& text);
    void on_open_chats();
    void on_open_settings();
    void on_subscribe_premium();
    void on_auth_mode_toggled(bool checked);
    void on_phone_input_changed(const QString& phone);
    void on_language_changed(int index);
    void on_language_cycle();

private:
    void build_login_page();
    void build_main_page();
    void build_chat_panel();
    void build_settings_panel();
    void seed_chat_list();
    void apply_theme();
    void animate_widget_fade(QWidget* widget);
    void animate_button_pulse(QPushButton* button);
    void update_account_badge();
    void update_phone_country_badge(const QString& phone);
    QString phone_flag_for(const QString& phone) const;
    QString phone_region_for(const QString& phone) const;
    void update_profile_preview();
    void finalize_auth_success(const QString& phone);
    void apply_language();
    QString tr_key(const QString& key) const;
    void set_active_chat(const QString& chat_name, const QString& recipient_id);
    void set_active_nav_button(QPushButton* active_button);
    void append_my_message(const QString& text);
    void append_peer_message(const QString& text);

    transport::HttpClient http_;
    api::auth::AuthRequests auth_api_;
    api::messages::MessageRequests msg_api_;
    std::string access_token_;

    QStackedWidget* pages_{nullptr};

    QWidget* login_page_{nullptr};
    QLabel* login_title_label_{nullptr};
    QLabel* login_subtitle_label_{nullptr};
    QLabel* login_hero_text_label_{nullptr};
    QLabel* login_hero_caption_label_{nullptr};
    QLineEdit* phone_edit_{nullptr};
    QLineEdit* password_edit_{nullptr};
    QLineEdit* register_username_edit_{nullptr};
    QCheckBox* register_mode_checkbox_{nullptr};
    QLabel* phone_country_label_{nullptr};
    QPushButton* language_switch_button_{nullptr};
    QPushButton* login_button_{nullptr};
    QLabel* login_status_{nullptr};

    QWidget* main_page_{nullptr};
    QLineEdit* search_edit_{nullptr};
    QLabel* account_badge_label_{nullptr};
    QListWidget* chats_list_{nullptr};
    QPushButton* chats_nav_button_{nullptr};
    QPushButton* settings_nav_button_{nullptr};

    QStackedWidget* content_stack_{nullptr};

    QWidget* chat_panel_{nullptr};
    QLabel* chat_name_label_{nullptr};
    QLabel* chat_status_label_{nullptr};
    QTextEdit* chat_log_{nullptr};
    QLineEdit* message_edit_{nullptr};
    QPushButton* send_button_{nullptr};

    QWidget* settings_panel_{nullptr};
    QTabWidget* settings_tabs_{nullptr};
    QLineEdit* profile_display_name_edit_{nullptr};
    QLineEdit* profile_username_edit_{nullptr};
    QLineEdit* profile_bio_edit_{nullptr};
    QLabel* profile_preview_name_{nullptr};
    QLabel* profile_preview_username_{nullptr};
    QLabel* profile_preview_bio_{nullptr};
    QLabel* profile_preview_phone_{nullptr};
    QCheckBox* notifications_checkbox_{nullptr};
    QCheckBox* enter_to_send_checkbox_{nullptr};
    QComboBox* settings_language_combo_{nullptr};
    QSlider* font_scale_slider_{nullptr};
    QCheckBox* read_receipts_checkbox_{nullptr};
    QCheckBox* save_media_checkbox_{nullptr};
    QCheckBox* lock_chats_checkbox_{nullptr};
    QCheckBox* hide_phone_checkbox_{nullptr};
    QLabel* premium_status_label_{nullptr};
    QPushButton* premium_subscribe_button_{nullptr};
    QCheckBox* premium_button_anim_checkbox_{nullptr};
    QCheckBox* premium_orb_anim_checkbox_{nullptr};
    QSlider* premium_anim_speed_slider_{nullptr};
    PremiumOrbWidget* premium_orb_widget_{nullptr};

    QString current_recipient_id_;
    QString current_phone_;
    QString current_language_{"ru"};
    bool premium_enabled_{false};
    int premium_anim_ms_{180};
};

} // namespace pingram::ui
