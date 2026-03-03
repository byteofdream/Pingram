#include "ui/main_window.hpp"
#include "ui/premium_orb_widget.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QDateTime>
#include <QFrame>
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPropertyAnimation>
#include <QPointer>
#include <QPushButton>
#include <QRegularExpression>
#include <QSlider>
#include <QStackedWidget>
#include <QTabWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>
#include <algorithm>
#include <vector>

namespace pingram::ui {

namespace {

struct DialRegion {
    const char* prefix;
    const char* region;
    const char* flag;
};

static const std::vector<DialRegion> kDialRegions{
    {"+888", "Custom Network", "🟣"},
    {"+998", "Uzbekistan", "🇺🇿"}, {"+996", "Kyrgyzstan", "🇰🇬"}, {"+995", "Georgia", "🇬🇪"},
    {"+994", "Azerbaijan", "🇦🇿"}, {"+993", "Turkmenistan", "🇹🇲"}, {"+992", "Tajikistan", "🇹🇯"},
    {"+977", "Nepal", "🇳🇵"}, {"+976", "Mongolia", "🇲🇳"}, {"+975", "Bhutan", "🇧🇹"},
    {"+974", "Qatar", "🇶🇦"}, {"+973", "Bahrain", "🇧🇭"}, {"+972", "Israel", "🇮🇱"},
    {"+971", "UAE", "🇦🇪"}, {"+970", "Palestine", "🇵🇸"}, {"+966", "Saudi Arabia", "🇸🇦"},
    {"+965", "Kuwait", "🇰🇼"}, {"+964", "Iraq", "🇮🇶"}, {"+963", "Syria", "🇸🇾"},
    {"+962", "Jordan", "🇯🇴"}, {"+961", "Lebanon", "🇱🇧"}, {"+960", "Maldives", "🇲🇻"},
    {"+886", "Taiwan", "🇹🇼"}, {"+880", "Bangladesh", "🇧🇩"},
    {"+856", "Laos", "🇱🇦"}, {"+855", "Cambodia", "🇰🇭"}, {"+853", "Macao", "🇲🇴"},
    {"+852", "Hong Kong", "🇭🇰"}, {"+850", "North Korea", "🇰🇵"},
    {"+995", "Georgia", "🇬🇪"}, {"+886", "Taiwan", "🇹🇼"},
    {"+420", "Czechia", "🇨🇿"}, {"+421", "Slovakia", "🇸🇰"}, {"+423", "Liechtenstein", "🇱🇮"},
    {"+380", "Ukraine", "🇺🇦"}, {"+377", "Monaco", "🇲🇨"}, {"+376", "Andorra", "🇦🇩"},
    {"+375", "Belarus", "🇧🇾"}, {"+374", "Armenia", "🇦🇲"}, {"+373", "Moldova", "🇲🇩"},
    {"+372", "Estonia", "🇪🇪"}, {"+371", "Latvia", "🇱🇻"}, {"+370", "Lithuania", "🇱🇹"},
    {"+358", "Finland", "🇫🇮"}, {"+357", "Cyprus", "🇨🇾"}, {"+356", "Malta", "🇲🇹"},
    {"+354", "Iceland", "🇮🇸"}, {"+353", "Ireland", "🇮🇪"}, {"+352", "Luxembourg", "🇱🇺"},
    {"+351", "Portugal", "🇵🇹"}, {"+389", "North Macedonia", "🇲🇰"},
    {"+86", "China", "🇨🇳"}, {"+81", "Japan", "🇯🇵"}, {"+49", "Germany", "🇩🇪"},
    {"+44", "United Kingdom", "🇬🇧"}, {"+39", "Italy", "🇮🇹"}, {"+34", "Spain", "🇪🇸"},
    {"+33", "France", "🇫🇷"}, {"+31", "Netherlands", "🇳🇱"}, {"+30", "Greece", "🇬🇷"},
    {"+27", "South Africa", "🇿🇦"}, {"+20", "Egypt", "🇪🇬"}, {"+7", "Russia/Kazakhstan", "🇷🇺"},
    {"+98", "Iran", "🇮🇷"}, {"+95", "Myanmar", "🇲🇲"}, {"+94", "Sri Lanka", "🇱🇰"},
    {"+93", "Afghanistan", "🇦🇫"}, {"+92", "Pakistan", "🇵🇰"}, {"+91", "India", "🇮🇳"},
    {"+90", "Turkey", "🇹🇷"}, {"+84", "Vietnam", "🇻🇳"}, {"+82", "South Korea", "🇰🇷"},
    {"+66", "Thailand", "🇹🇭"}, {"+65", "Singapore", "🇸🇬"}, {"+64", "New Zealand", "🇳🇿"},
    {"+63", "Philippines", "🇵🇭"}, {"+62", "Indonesia", "🇮🇩"}, {"+61", "Australia", "🇦🇺"},
    {"+60", "Malaysia", "🇲🇾"}, {"+55", "Brazil", "🇧🇷"}, {"+54", "Argentina", "🇦🇷"},
    {"+52", "Mexico", "🇲🇽"}, {"+51", "Peru", "🇵🇪"}, {"+48", "Poland", "🇵🇱"},
    {"+47", "Norway", "🇳🇴"}, {"+46", "Sweden", "🇸🇪"}, {"+45", "Denmark", "🇩🇰"},
    {"+43", "Austria", "🇦🇹"}, {"+41", "Switzerland", "🇨🇭"}, {"+40", "Romania", "🇷🇴"},
    {"+36", "Hungary", "🇭🇺"}, {"+32", "Belgium", "🇧🇪"}, {"+1", "USA/Canada", "🇺🇸"},
};

const DialRegion* match_region(const QString& phone) {
    QString normalized = phone.trimmed();
    if (!normalized.startsWith("+") && normalized.size() > 0 && normalized[0].isDigit()) {
        normalized = "+" + normalized;
    }
    const DialRegion* best = nullptr;
    int best_len = -1;
    for (const auto& item : kDialRegions) {
        const QString prefix = QString::fromLatin1(item.prefix);
        if (normalized.startsWith(prefix) && prefix.size() > best_len) {
            best = &item;
            best_len = prefix.size();
        }
    }
    return best;
}

} // namespace

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      auth_api_(http_, "http://127.0.0.1:8000"),
      msg_api_(http_, "http://127.0.0.1:8000") {
    setWindowTitle("Pingram Desktop");
    resize(1280, 800);

    pages_ = new QStackedWidget(this);
    setCentralWidget(pages_);

    build_login_page();
    build_main_page();
    apply_theme();

    pages_->setCurrentWidget(login_page_);
}

void MainWindow::build_login_page() {
    login_page_ = new QWidget(this);

    auto* shell = new QHBoxLayout(login_page_);
    shell->setContentsMargins(64, 64, 64, 64);
    shell->setSpacing(28);

    auto* hero = new QWidget(login_page_);
    hero->setObjectName("LoginHero");
    auto* hero_layout = new QVBoxLayout(hero);
    hero_layout->setContentsMargins(32, 32, 32, 32);
    hero_layout->setSpacing(14);

    auto* hero_title = new QLabel("Pingram", hero);
    hero_title->setObjectName("HeroTitle");
    login_hero_text_label_ = new QLabel(
        "Мессенджер для быстрых командных диалогов.\n"
        "Безопасная авторизация по номеру, чистый интерфейс,\n"
        "и премиум-возможности для тех, кто хочет больше.",
        hero
    );
    login_hero_text_label_->setObjectName("HeroText");
    login_hero_text_label_->setWordWrap(true);

    login_hero_caption_label_ = new QLabel("Вход или регистрация занимает меньше минуты.", hero);
    login_hero_caption_label_->setObjectName("HeroCaption");

    hero_layout->addWidget(hero_title);
    hero_layout->addWidget(login_hero_text_label_);
    hero_layout->addWidget(login_hero_caption_label_);
    hero_layout->addStretch();

    auto* card = new QWidget(login_page_);
    card->setObjectName("LoginCard");
    card->setMinimumWidth(470);

    auto* card_layout = new QVBoxLayout(card);
    card_layout->setContentsMargins(32, 28, 32, 28);
    card_layout->setSpacing(12);

    login_title_label_ = new QLabel("Pingram", card);
    login_title_label_->setObjectName("LoginTitle");

    login_subtitle_label_ = new QLabel("Вход или регистрация по номеру телефона", card);
    login_subtitle_label_->setObjectName("LoginSubTitle");

    language_switch_button_ = new QPushButton("Русский", card);
    language_switch_button_->setObjectName("LangSwitchButton");

    register_mode_checkbox_ = new QCheckBox("Создать новый аккаунт", card);

    phone_edit_ = new QLineEdit(card);
    phone_edit_->setPlaceholderText("+380501112233");
    phone_edit_->setText("+380501112233");

    phone_country_label_ = new QLabel(card);
    phone_country_label_->setObjectName("PhoneCountryLabel");

    register_username_edit_ = new QLineEdit(card);
    register_username_edit_->setPlaceholderText("Username для нового аккаунта");
    register_username_edit_->setText("new_user");
    register_username_edit_->setVisible(false);

    password_edit_ = new QLineEdit(card);
    password_edit_->setPlaceholderText("Пароль");
    password_edit_->setEchoMode(QLineEdit::Password);
    password_edit_->setText("password123");

    login_button_ = new QPushButton("Войти", card);
    login_button_->setObjectName("PrimaryButton");

    login_status_ = new QLabel(card);
    login_status_->setObjectName("LoginStatus");

    card_layout->addWidget(login_title_label_);
    card_layout->addWidget(login_subtitle_label_);
    card_layout->addWidget(language_switch_button_);
    card_layout->addWidget(register_mode_checkbox_);
    card_layout->addSpacing(6);
    card_layout->addWidget(phone_edit_);
    card_layout->addWidget(phone_country_label_);
    card_layout->addWidget(register_username_edit_);
    card_layout->addWidget(password_edit_);
    card_layout->addWidget(login_button_);
    card_layout->addWidget(login_status_);

    shell->addWidget(hero, 1);
    shell->addWidget(card, 0, Qt::AlignVCenter);

    pages_->addWidget(login_page_);

    connect(login_button_, &QPushButton::clicked, this, &MainWindow::on_login_clicked);
    connect(register_mode_checkbox_, &QCheckBox::toggled, this, &MainWindow::on_auth_mode_toggled);
    connect(phone_edit_, &QLineEdit::textChanged, this, &MainWindow::on_phone_input_changed);
    connect(language_switch_button_, &QPushButton::clicked, this, &MainWindow::on_language_cycle);
    update_phone_country_badge(phone_edit_->text());
    apply_language();
}

void MainWindow::build_main_page() {
    main_page_ = new QWidget(this);

    auto* root = new QHBoxLayout(main_page_);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    auto* sidebar = new QWidget(main_page_);
    sidebar->setObjectName("Sidebar");
    sidebar->setMinimumWidth(340);
    sidebar->setMaximumWidth(380);

    auto* sidebar_layout = new QVBoxLayout(sidebar);
    sidebar_layout->setContentsMargins(16, 16, 16, 16);
    sidebar_layout->setSpacing(10);

    auto* app_title = new QLabel("Pingram", sidebar);
    app_title->setObjectName("AppTitle");

    account_badge_label_ = new QLabel("Не авторизован", sidebar);
    account_badge_label_->setObjectName("AccountBadge");
    account_badge_label_->setTextFormat(Qt::RichText);

    search_edit_ = new QLineEdit(sidebar);
    search_edit_->setObjectName("SearchInput");
    search_edit_->setPlaceholderText("Поиск");

    chats_list_ = new QListWidget(sidebar);
    chats_list_->setObjectName("ChatsList");
    chats_list_->setSelectionMode(QAbstractItemView::SingleSelection);

    auto* nav_row = new QWidget(sidebar);
    auto* nav_row_layout = new QHBoxLayout(nav_row);
    nav_row_layout->setContentsMargins(0, 0, 0, 0);
    nav_row_layout->setSpacing(8);

    chats_nav_button_ = new QPushButton("Чаты", nav_row);
    settings_nav_button_ = new QPushButton("Настройки", nav_row);

    chats_nav_button_->setObjectName("NavButton");
    settings_nav_button_->setObjectName("NavButton");

    chats_nav_button_->setCheckable(true);
    settings_nav_button_->setCheckable(true);

    nav_row_layout->addWidget(chats_nav_button_);
    nav_row_layout->addWidget(settings_nav_button_);

    sidebar_layout->addWidget(app_title);
    sidebar_layout->addWidget(account_badge_label_);
    sidebar_layout->addWidget(search_edit_);
    sidebar_layout->addWidget(chats_list_, 1);
    sidebar_layout->addWidget(nav_row);

    content_stack_ = new QStackedWidget(main_page_);
    content_stack_->setObjectName("ContentStack");

    build_chat_panel();
    build_settings_panel();

    content_stack_->addWidget(chat_panel_);
    content_stack_->addWidget(settings_panel_);

    root->addWidget(sidebar);
    root->addWidget(content_stack_, 1);

    pages_->addWidget(main_page_);

    seed_chat_list();
    set_active_nav_button(chats_nav_button_);
    update_account_badge();

    connect(send_button_, &QPushButton::clicked, this, &MainWindow::on_send_clicked);
    connect(message_edit_, &QLineEdit::returnPressed, this, [this]() {
        if (enter_to_send_checkbox_ && enter_to_send_checkbox_->isChecked()) {
            on_send_clicked();
        }
    });
    connect(chats_list_, &QListWidget::itemClicked, this, &MainWindow::on_chat_selected);
    connect(search_edit_, &QLineEdit::textChanged, this, &MainWindow::on_chat_filter_changed);
    connect(chats_nav_button_, &QPushButton::clicked, this, &MainWindow::on_open_chats);
    connect(settings_nav_button_, &QPushButton::clicked, this, &MainWindow::on_open_settings);
    connect(premium_subscribe_button_, &QPushButton::clicked, this, &MainWindow::on_subscribe_premium);
}

void MainWindow::build_chat_panel() {
    chat_panel_ = new QWidget(main_page_);
    chat_panel_->setObjectName("ContentPanel");

    auto* content_layout = new QVBoxLayout(chat_panel_);
    content_layout->setContentsMargins(0, 0, 0, 0);
    content_layout->setSpacing(0);

    auto* topbar = new QWidget(chat_panel_);
    topbar->setObjectName("TopBar");

    auto* topbar_layout = new QVBoxLayout(topbar);
    topbar_layout->setContentsMargins(22, 14, 22, 14);
    topbar_layout->setSpacing(2);

    chat_name_label_ = new QLabel("Выберите чат", topbar);
    chat_name_label_->setObjectName("ChatName");

    chat_status_label_ = new QLabel("offline", topbar);
    chat_status_label_->setObjectName("ChatStatus");

    topbar_layout->addWidget(chat_name_label_);
    topbar_layout->addWidget(chat_status_label_);

    chat_log_ = new QTextEdit(chat_panel_);
    chat_log_->setReadOnly(true);
    chat_log_->setObjectName("ChatLog");

    auto* composer = new QWidget(chat_panel_);
    composer->setObjectName("Composer");

    auto* composer_layout = new QHBoxLayout(composer);
    composer_layout->setContentsMargins(16, 12, 16, 12);
    composer_layout->setSpacing(10);

    auto* attach_button = new QPushButton("+", composer);
    attach_button->setObjectName("GhostButton");
    attach_button->setEnabled(false);

    message_edit_ = new QLineEdit(composer);
    message_edit_->setPlaceholderText("Напишите сообщение...");

    send_button_ = new QPushButton("Отправить", composer);
    send_button_->setObjectName("PrimaryButton");

    composer_layout->addWidget(attach_button);
    composer_layout->addWidget(message_edit_, 1);
    composer_layout->addWidget(send_button_);

    content_layout->addWidget(topbar);
    content_layout->addWidget(chat_log_, 1);
    content_layout->addWidget(composer);
}

void MainWindow::build_settings_panel() {
    settings_panel_ = new QWidget(main_page_);
    settings_panel_->setObjectName("ContentPanel");

    auto* layout = new QVBoxLayout(settings_panel_);
    layout->setContentsMargins(34, 28, 34, 28);
    layout->setSpacing(16);

    auto* title = new QLabel("Настройки", settings_panel_);
    title->setObjectName("SectionTitle");

    settings_tabs_ = new QTabWidget(settings_panel_);
    settings_tabs_->setObjectName("SettingsTabs");

    auto* profile_tab = new QWidget(settings_tabs_);
    auto* profile_layout = new QVBoxLayout(profile_tab);
    profile_layout->setContentsMargins(12, 12, 12, 12);
    profile_layout->setSpacing(12);

    auto* profile_card = new QFrame(profile_tab);
    profile_card->setObjectName("SettingsCard");
    auto* profile_card_layout = new QVBoxLayout(profile_card);
    profile_card_layout->setContentsMargins(18, 18, 18, 18);
    profile_card_layout->setSpacing(10);

    profile_display_name_edit_ = new QLineEdit(profile_card);
    profile_display_name_edit_->setPlaceholderText("Отображаемое имя");
    profile_display_name_edit_->setText("Alice");

    profile_username_edit_ = new QLineEdit(profile_card);
    profile_username_edit_->setPlaceholderText("Username");
    profile_username_edit_->setText("@alice");

    profile_bio_edit_ = new QLineEdit(profile_card);
    profile_bio_edit_->setPlaceholderText("О себе");
    profile_bio_edit_->setText("Pingram early adopter");

    auto* profile_save_btn = new QPushButton("Сохранить профиль", profile_card);
    profile_save_btn->setObjectName("PrimaryButton");

    auto* preview_card = new QFrame(profile_tab);
    preview_card->setObjectName("ProfilePreviewCard");
    auto* preview_layout = new QVBoxLayout(preview_card);
    preview_layout->setContentsMargins(18, 18, 18, 18);
    preview_layout->setSpacing(8);
    preview_layout->addWidget(new QLabel("Предпросмотр профиля", preview_card));

    profile_preview_name_ = new QLabel(preview_card);
    profile_preview_username_ = new QLabel(preview_card);
    profile_preview_bio_ = new QLabel(preview_card);
    profile_preview_phone_ = new QLabel(preview_card);
    profile_preview_phone_->setTextFormat(Qt::RichText);

    preview_layout->addWidget(profile_preview_name_);
    preview_layout->addWidget(profile_preview_username_);
    preview_layout->addWidget(profile_preview_bio_);
    preview_layout->addWidget(profile_preview_phone_);

    profile_card_layout->addWidget(new QLabel("Профиль", profile_card));
    profile_card_layout->addWidget(profile_display_name_edit_);
    profile_card_layout->addWidget(profile_username_edit_);
    profile_card_layout->addWidget(profile_bio_edit_);
    profile_card_layout->addWidget(profile_save_btn);

    profile_layout->addWidget(profile_card);
    profile_layout->addWidget(preview_card);
    profile_layout->addStretch();

    auto* app_tab = new QWidget(settings_tabs_);
    auto* app_layout = new QVBoxLayout(app_tab);
    app_layout->setContentsMargins(12, 12, 12, 12);
    app_layout->setSpacing(12);

    auto* app_card = new QFrame(app_tab);
    app_card->setObjectName("SettingsCard");
    auto* app_card_layout = new QVBoxLayout(app_card);
    app_card_layout->setContentsMargins(18, 18, 18, 18);
    app_card_layout->setSpacing(14);

    notifications_checkbox_ = new QCheckBox("Включить push-уведомления", app_card);
    notifications_checkbox_->setChecked(true);

    enter_to_send_checkbox_ = new QCheckBox("Отправлять сообщение по Enter", app_card);
    enter_to_send_checkbox_->setChecked(true);

    settings_language_combo_ = new QComboBox(app_card);
    settings_language_combo_->addItem("Русский", "ru");
    settings_language_combo_->addItem("English", "en");
    settings_language_combo_->addItem("Українська", "uk");

    auto* slider_label = new QLabel("Размер шрифта чата", app_card);
    font_scale_slider_ = new QSlider(Qt::Horizontal, app_card);
    font_scale_slider_->setMinimum(12);
    font_scale_slider_->setMaximum(20);
    font_scale_slider_->setValue(14);

    app_card_layout->addWidget(notifications_checkbox_);
    app_card_layout->addWidget(enter_to_send_checkbox_);
    app_card_layout->addWidget(new QLabel("Язык интерфейса", app_card));
    app_card_layout->addWidget(settings_language_combo_);
    app_card_layout->addWidget(slider_label);
    app_card_layout->addWidget(font_scale_slider_);

    app_layout->addWidget(app_card);
    app_layout->addStretch();

    auto* chats_tab = new QWidget(settings_tabs_);
    auto* chats_layout = new QVBoxLayout(chats_tab);
    chats_layout->setContentsMargins(12, 12, 12, 12);
    chats_layout->setSpacing(12);

    auto* chats_card = new QFrame(chats_tab);
    chats_card->setObjectName("SettingsCard");
    auto* chats_card_layout = new QVBoxLayout(chats_card);
    chats_card_layout->setContentsMargins(18, 18, 18, 18);
    chats_card_layout->setSpacing(12);

    read_receipts_checkbox_ = new QCheckBox("Показывать галочки доставки", chats_card);
    read_receipts_checkbox_->setChecked(true);
    save_media_checkbox_ = new QCheckBox("Автосохранять медиа в галерее", chats_card);
    save_media_checkbox_->setChecked(false);

    chats_card_layout->addWidget(read_receipts_checkbox_);
    chats_card_layout->addWidget(save_media_checkbox_);
    chats_layout->addWidget(chats_card);
    chats_layout->addStretch();

    auto* privacy_tab = new QWidget(settings_tabs_);
    auto* privacy_layout = new QVBoxLayout(privacy_tab);
    privacy_layout->setContentsMargins(12, 12, 12, 12);
    privacy_layout->setSpacing(12);

    auto* privacy_card = new QFrame(privacy_tab);
    privacy_card->setObjectName("SettingsCard");
    auto* privacy_card_layout = new QVBoxLayout(privacy_card);
    privacy_card_layout->setContentsMargins(18, 18, 18, 18);
    privacy_card_layout->setSpacing(12);

    lock_chats_checkbox_ = new QCheckBox("Блокировать чаты по PIN (демо)", privacy_card);
    lock_chats_checkbox_->setChecked(false);
    hide_phone_checkbox_ = new QCheckBox("Скрывать номер в профиле", privacy_card);
    hide_phone_checkbox_->setChecked(false);

    privacy_card_layout->addWidget(lock_chats_checkbox_);
    privacy_card_layout->addWidget(hide_phone_checkbox_);
    privacy_layout->addWidget(privacy_card);
    privacy_layout->addStretch();

    auto* premium_tab = new QWidget(settings_tabs_);
    auto* premium_tab_layout = new QVBoxLayout(premium_tab);
    premium_tab_layout->setContentsMargins(12, 12, 12, 12);
    premium_tab_layout->setSpacing(12);

    auto* premium_card = new QFrame(premium_tab);
    premium_card->setObjectName("PremiumInlineCard");
    auto* premium_layout = new QVBoxLayout(premium_card);
    premium_layout->setContentsMargins(18, 18, 18, 18);
    premium_layout->setSpacing(10);

    auto* premium_title = new QLabel("Pingram Premium", premium_card);
    premium_title->setObjectName("PremiumInlineTitle");
    premium_orb_widget_ = new PremiumOrbWidget(premium_card);

    auto* premium_features = new QLabel(
        "- Быстрая загрузка медиа\n"
        "- Дополнительные темы\n"
        "- Расширенные лимиты облака",
        premium_card
    );
    premium_features->setObjectName("HintText");

    premium_status_label_ = new QLabel("Текущий статус: Free", premium_card);
    premium_status_label_->setObjectName("PremiumStatus");

    premium_button_anim_checkbox_ = new QCheckBox("Анимировать premium-кнопку", premium_card);
    premium_button_anim_checkbox_->setChecked(true);
    premium_orb_anim_checkbox_ = new QCheckBox("Анимировать premium-орбиту", premium_card);
    premium_orb_anim_checkbox_->setChecked(true);

    auto* premium_speed_label = new QLabel("Скорость premium-анимаций", premium_card);
    premium_anim_speed_slider_ = new QSlider(Qt::Horizontal, premium_card);
    premium_anim_speed_slider_->setMinimum(90);
    premium_anim_speed_slider_->setMaximum(420);
    premium_anim_speed_slider_->setValue(premium_anim_ms_);

    premium_subscribe_button_ = new QPushButton("Подключить Premium за $4.99/мес", premium_card);
    premium_subscribe_button_->setObjectName("PremiumButton");

    premium_layout->addWidget(premium_title);
    premium_layout->addWidget(premium_orb_widget_, 0, Qt::AlignHCenter);
    premium_layout->addWidget(premium_features);
    premium_layout->addWidget(premium_status_label_);
    premium_layout->addWidget(premium_button_anim_checkbox_);
    premium_layout->addWidget(premium_orb_anim_checkbox_);
    premium_layout->addWidget(premium_speed_label);
    premium_layout->addWidget(premium_anim_speed_slider_);
    premium_layout->addWidget(premium_subscribe_button_);

    premium_tab_layout->addWidget(premium_card);
    premium_tab_layout->addStretch();

    settings_tabs_->addTab(profile_tab, "Профиль");
    settings_tabs_->addTab(app_tab, "Приложение");
    settings_tabs_->addTab(chats_tab, "Чаты");
    settings_tabs_->addTab(privacy_tab, "Приватность");
    settings_tabs_->addTab(premium_tab, "Premium");

    auto* tips = new QLabel(
        "Совет: для production добавьте синхронизацию настроек на сервере и E2E шифрование.",
        settings_panel_
    );
    tips->setWordWrap(true);
    tips->setObjectName("HintText");

    layout->addWidget(title);
    layout->addWidget(settings_tabs_, 1);
    layout->addWidget(tips);

    connect(font_scale_slider_, &QSlider::valueChanged, this, [this](int value) {
        chat_log_->setStyleSheet(QString("font-size:%1px;").arg(value));
    });
    connect(profile_save_btn, &QPushButton::clicked, this, [this]() {
        chat_status_label_->setText("Профиль обновлен");
        animate_widget_fade(settings_panel_);
        update_profile_preview();
    });
    connect(profile_display_name_edit_, &QLineEdit::textChanged, this, [this]() { update_profile_preview(); });
    connect(profile_username_edit_, &QLineEdit::textChanged, this, [this]() { update_profile_preview(); });
    connect(profile_bio_edit_, &QLineEdit::textChanged, this, [this]() { update_profile_preview(); });
    connect(hide_phone_checkbox_, &QCheckBox::toggled, this, [this]() { update_profile_preview(); });
    connect(settings_language_combo_, &QComboBox::currentIndexChanged, this, &MainWindow::on_language_changed);
    connect(premium_orb_anim_checkbox_, &QCheckBox::toggled, this, [this](bool checked) {
        if (premium_orb_widget_) {
            premium_orb_widget_->set_anim_enabled(checked);
        }
    });
    connect(premium_anim_speed_slider_, &QSlider::valueChanged, this, [this](int value) {
        premium_anim_ms_ = value;
        if (premium_orb_widget_) {
            premium_orb_widget_->set_speed_ms(value);
        }
    });
    update_profile_preview();
}

void MainWindow::seed_chat_list() {
    struct SeedChat {
        QString title;
        QString subtitle;
        QString recipient_id;
    };

    const SeedChat seed[] = {
        {"Bob", "online", "user-bob"},
        {"Design Team", "Новый макет экрана профиля", "user-bob"},
        {"Backend Crew", "deploy в 21:00", "user-bob"},
    };

    for (const auto& chat : seed) {
        auto* item = new QListWidgetItem(chat.title + "\n" + chat.subtitle);
        item->setData(Qt::UserRole, chat.recipient_id);
        item->setData(Qt::UserRole + 1, chat.title);
        chats_list_->addItem(item);
    }

    if (chats_list_->count() > 0) {
        chats_list_->setCurrentRow(0);
        on_chat_selected(chats_list_->item(0));
    }
}

void MainWindow::apply_theme() {
    setStyleSheet(R"(
        QMainWindow {
            background: #cfdbe6;
        }

        QWidget {
            color: #1f2d3d;
            font-family: "Segoe UI", "Noto Sans", sans-serif;
            font-size: 14px;
        }

        QWidget#LoginCard {
            background: rgba(255, 255, 255, 0.95);
            border: 1px solid #d5deea;
            border-radius: 18px;
        }

        QWidget#LoginHero {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                        stop:0 #11325d, stop:1 #3f6fb5);
            border-radius: 24px;
        }

        QLabel#HeroTitle {
            font-size: 44px;
            font-weight: 800;
            color: #f4f8ff;
        }

        QLabel#HeroText {
            font-size: 16px;
            color: #d4e5ff;
            line-height: 1.4;
        }

        QLabel#HeroCaption {
            color: #b5cbef;
            font-size: 13px;
        }

        QLabel#LoginTitle {
            font-size: 32px;
            font-weight: 800;
        }

        QLabel#LoginSubTitle {
            color: #5f7287;
        }

        QLabel#LoginStatus {
            color: #c54848;
            min-height: 20px;
        }

        QPushButton#LangSwitchButton {
            border: 1px solid #c8d7e6;
            border-radius: 10px;
            background: #f4f8fd;
            color: #2f4f6b;
            font-weight: 600;
            padding: 8px 10px;
            text-align: left;
        }

        QPushButton#LangSwitchButton:hover {
            background: #e7f1fb;
        }

        QLabel#PhoneCountryLabel {
            color: #48627d;
            background: #eef4fb;
            border: 1px solid #d5e1ee;
            border-radius: 10px;
            padding: 6px 10px;
            font-weight: 600;
        }

        QWidget#Sidebar {
            background: #f4f8fc;
            border-right: 1px solid #d7e0ec;
        }

        QLabel#AppTitle {
            font-size: 24px;
            font-weight: 700;
            color: #23435f;
            padding-left: 4px;
        }

        QLabel#AccountBadge {
            background: #e7eef6;
            border: 1px solid #d1dce8;
            border-radius: 12px;
            padding: 6px 10px;
            color: #2f475f;
            font-weight: 600;
        }

        QLineEdit#SearchInput {
            background: #e9f0f7;
            border: 1px solid #d2dde8;
            border-radius: 14px;
            padding: 10px 12px;
        }

        QListWidget#ChatsList {
            border: none;
            background: transparent;
            outline: none;
        }

        QListWidget#ChatsList::item {
            background: transparent;
            border-radius: 11px;
            padding: 11px;
            margin: 2px 0;
        }

        QListWidget#ChatsList::item:selected {
            background: #d6e9fa;
            color: #102437;
        }

        QStackedWidget#ContentStack {
            background: #dce6f2;
        }

        QWidget#ContentPanel {
            background: #dce6f2;
        }

        QWidget#TopBar {
            background: #ffffff;
            border-bottom: 1px solid #d4deea;
        }

        QLabel#ChatName {
            font-size: 17px;
            font-weight: 600;
        }

        QLabel#ChatStatus {
            color: #6b7f94;
            font-size: 12px;
        }

        QTextEdit#ChatLog {
            border: none;
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                        stop:0 #dce7f3, stop:1 #d5e2f1);
            padding: 14px;
        }

        QWidget#Composer {
            background: #ffffff;
            border-top: 1px solid #d4deea;
        }

        QLineEdit {
            background: #ffffff;
            border: 1px solid #ced8e4;
            border-radius: 14px;
            padding: 10px 12px;
            selection-background-color: #2b9be7;
        }

        QPushButton#PrimaryButton, QPushButton#NavButton {
            border: none;
            border-radius: 14px;
            background: #2b9be7;
            color: #ffffff;
            font-weight: 700;
            padding: 10px 16px;
        }

        QPushButton#PrimaryButton:hover, QPushButton#NavButton:hover {
            background: #228fd8;
        }

        QPushButton#PrimaryButton:pressed, QPushButton#NavButton:pressed {
            background: #176fa9;
        }

        QPushButton#NavButton:checked {
            background: #1f78b8;
        }

        QPushButton#GhostButton {
            background: #e6edf5;
            color: #7b8d9e;
            border: none;
            border-radius: 14px;
            min-width: 36px;
            max-width: 36px;
            padding: 8px;
        }

        QLabel#SectionTitle {
            font-size: 26px;
            font-weight: 700;
            color: #2a3e53;
        }

        QTabWidget#SettingsTabs::pane {
            border: none;
        }

        QTabBar::tab {
            background: #e5edf6;
            border: 1px solid #ced9e5;
            border-bottom: none;
            border-top-left-radius: 10px;
            border-top-right-radius: 10px;
            padding: 8px 14px;
            margin-right: 6px;
            color: #35516b;
            font-weight: 600;
        }

        QTabBar::tab:selected {
            background: #ffffff;
            color: #1f3952;
        }

        QFrame#SettingsCard {
            background: #ffffff;
            border: 1px solid #d4deea;
            border-radius: 16px;
        }

        QFrame#PremiumInlineCard {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                        stop:0 #fff1bf, stop:1 #ffdf84);
            border: 1px solid #eac35c;
            border-radius: 16px;
        }

        QFrame#ProfilePreviewCard {
            background: #f4f7fb;
            border: 1px solid #d6e0eb;
            border-radius: 14px;
        }

        QLabel#PremiumInlineTitle {
            font-size: 22px;
            font-weight: 800;
            color: #3f2f07;
        }

        QLabel#PremiumStatus {
            font-size: 16px;
            font-weight: 600;
            color: #3f2f07;
        }

        QPushButton#PremiumButton {
            border: none;
            border-radius: 14px;
            background: #222;
            color: #ffd66d;
            font-weight: 800;
            padding: 12px 20px;
        }

        QLabel#HintText {
            color: #5e7083;
        }
    )");
}

void MainWindow::animate_widget_fade(QWidget* widget) {
    if (!widget) {
        return;
    }

    if (auto* previous = widget->graphicsEffect()) {
        previous->deleteLater();
    }

    auto* effect = new QGraphicsOpacityEffect(widget);
    widget->setGraphicsEffect(effect);

    QPointer<QWidget> widget_guard(widget);
    QPointer<QGraphicsOpacityEffect> effect_guard(effect);

    auto* animation = new QPropertyAnimation(effect, "opacity", effect);
    animation->setDuration(220);
    animation->setStartValue(0.2);
    animation->setEndValue(1.0);
    animation->start(QAbstractAnimation::DeleteWhenStopped);

    connect(animation, &QPropertyAnimation::finished, effect, [widget_guard, effect_guard]() {
        if (!widget_guard || !effect_guard) {
            return;
        }
        if (widget_guard->graphicsEffect() == effect_guard) {
            widget_guard->setGraphicsEffect(nullptr);
        }
    });
}

void MainWindow::animate_button_pulse(QPushButton* button) {
    if (!button) {
        return;
    }
    auto* animation = new QPropertyAnimation(button, "maximumWidth", button);
    const int original = button->maximumWidth() > 0 ? button->maximumWidth() : button->width();
    animation->setDuration(premium_anim_ms_);
    animation->setStartValue(original);
    animation->setKeyValueAt(0.5, original + 8);
    animation->setEndValue(original);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

QString MainWindow::phone_flag_for(const QString& phone) const {
    if (const DialRegion* region = match_region(phone)) {
        return QString::fromUtf8(region->flag);
    }
    return "🌐";
}

QString MainWindow::phone_region_for(const QString& phone) const {
    if (const DialRegion* region = match_region(phone)) {
        return QString::fromLatin1(region->region);
    }
    return "Unknown region";
}

void MainWindow::update_phone_country_badge(const QString& phone) {
    if (!phone_country_label_) {
        return;
    }
    const QString clean = phone.trimmed();
    if (clean.isEmpty()) {
        phone_country_label_->setText("🌐 " + tr_key("enter_phone_code"));
        return;
    }
    QString region = phone_region_for(clean);
    if (region == "Unknown region") {
        region = tr_key("unknown_region");
    }
    phone_country_label_->setText(phone_flag_for(clean) + " " + region);
}

void MainWindow::update_account_badge() {
    if (!account_badge_label_) {
        return;
    }

    if (current_phone_.isEmpty()) {
        account_badge_label_->setText("Не авторизован");
        return;
    }

    const QString flag = phone_flag_for(current_phone_);
    if (premium_enabled_) {
        account_badge_label_->setText(
            QString("%1 %2 <span style='color:#8b5cf6;font-weight:700;'>Premium</span>")
                .arg(flag, current_phone_.toHtmlEscaped())
        );
    } else {
        account_badge_label_->setText(flag + " " + current_phone_.toHtmlEscaped());
    }
}

void MainWindow::update_profile_preview() {
    if (!profile_preview_name_ || !profile_preview_username_ || !profile_preview_bio_ || !profile_preview_phone_) {
        return;
    }

    const QString display_name = profile_display_name_edit_ ? profile_display_name_edit_->text().trimmed() : "";
    const QString username = profile_username_edit_ ? profile_username_edit_->text().trimmed() : "";
    const QString bio = profile_bio_edit_ ? profile_bio_edit_->text().trimmed() : "";

    profile_preview_name_->setText("Имя: " + (display_name.isEmpty() ? QString("—") : display_name));
    profile_preview_username_->setText("Username: " + (username.isEmpty() ? QString("—") : username));
    profile_preview_bio_->setText("Bio: " + (bio.isEmpty() ? QString("—") : bio));

    const bool hide_phone = hide_phone_checkbox_ && hide_phone_checkbox_->isChecked();
    if (current_phone_.isEmpty() || hide_phone) {
        profile_preview_phone_->setText("Телефон: —");
    } else if (premium_enabled_) {
        profile_preview_phone_->setText(
            QString("Телефон: %1 %2 <span style='color:#8b5cf6;font-weight:700;'>Premium</span>")
                .arg(phone_flag_for(current_phone_), current_phone_.toHtmlEscaped())
        );
    } else {
        profile_preview_phone_->setText(
            QString("Телефон: %1 %2").arg(phone_flag_for(current_phone_), current_phone_.toHtmlEscaped())
        );
    }
}

void MainWindow::finalize_auth_success(const QString& phone) {
    current_phone_ = phone;
    update_account_badge();
    update_profile_preview();
    pages_->setCurrentWidget(main_page_);
    animate_widget_fade(main_page_);
    chat_status_label_->setText("online");
    animate_button_pulse(login_button_);
}

QString MainWindow::tr_key(const QString& key) const {
    const bool en = current_language_ == "en";
    const bool uk = current_language_ == "uk";

    if (key == "login_subtitle") return en ? "Sign in or register with your phone number" : uk ? "Вхід або реєстрація за номером телефону" : "Вход или регистрация по номеру телефона";
    if (key == "create_account") return en ? "Create new account" : uk ? "Створити новий акаунт" : "Создать новый аккаунт";
    if (key == "phone_placeholder") return en ? "+380501112233" : uk ? "+380501112233" : "+380501112233";
    if (key == "username_placeholder") return en ? "Username for a new account" : uk ? "Username для нового акаунта" : "Username для нового аккаунта";
    if (key == "password_placeholder") return en ? "Password" : uk ? "Пароль" : "Пароль";
    if (key == "login_button") return en ? "Sign in" : uk ? "Увійти" : "Войти";
    if (key == "create_button") return en ? "Create account" : uk ? "Створити акаунт" : "Создать аккаунт";
    if (key == "hero_text") return en ? "Messenger for fast team conversations.\nSecure phone auth, clean interface,\nand premium options for power users." : uk ? "Месенджер для швидких командних діалогів.\nБезпечна авторизація за номером, чистий інтерфейс,\nі premium-можливості для тих, хто хоче більше." : "Мессенджер для быстрых командных диалогов.\nБезопасная авторизация по номеру, чистый интерфейс,\nи премиум-возможности для тех, кто хочет больше.";
    if (key == "hero_caption") return en ? "Sign in or registration takes less than a minute." : uk ? "Вхід або реєстрація займає менше хвилини." : "Вход или регистрация занимает меньше минуты.";
    if (key == "settings_title") return en ? "Settings" : uk ? "Налаштування" : "Настройки";
    if (key == "tab_profile") return en ? "Profile" : uk ? "Профіль" : "Профиль";
    if (key == "tab_app") return en ? "App" : uk ? "Застосунок" : "Приложение";
    if (key == "tab_chats") return en ? "Chats" : uk ? "Чати" : "Чаты";
    if (key == "tab_privacy") return en ? "Privacy" : uk ? "Приватність" : "Приватность";
    if (key == "tab_premium") return "Premium";
    if (key == "enter_phone_code") return en ? "Enter phone number with country code" : uk ? "Введіть номер з кодом країни" : "Введите номер с кодом страны";
    if (key == "unknown_region") return en ? "Unknown region" : uk ? "Невідомий регіон" : "Неизвестный регион";
    if (key == "invalid_phone") return en ? "Enter a valid number (+code), including +888..." : uk ? "Введіть коректний номер (+код), включно з +888..." : "Введите корректный номер (+код), включая +888...";
    if (key == "enter_password") return en ? "Enter password" : uk ? "Введіть пароль" : "Введите пароль";
    if (key == "username_min") return en ? "Username must be at least 3 chars" : uk ? "Username має бути від 3 символів" : "Username должен быть от 3 символов";
    if (key == "logging_in") return en ? "Signing in..." : uk ? "Вхід..." : "Вход...";
    if (key == "creating") return en ? "Creating..." : uk ? "Створення..." : "Создание...";
    if (key == "lang_button_ru") return en ? "Language: Russian" : uk ? "Мова: Руська" : "Язык: Русский";
    if (key == "lang_button_en") return en ? "Language: English" : uk ? "Мова: English" : "Язык: English";
    if (key == "lang_button_uk") return en ? "Language: Ukrainian" : uk ? "Мова: Українська" : "Язык: Українська";
    return key;
}

void MainWindow::apply_language() {
    if (!login_subtitle_label_ || !register_mode_checkbox_ || !phone_edit_ || !password_edit_ || !register_username_edit_ || !login_button_) {
        return;
    }
    login_subtitle_label_->setText(tr_key("login_subtitle"));
    register_mode_checkbox_->setText(tr_key("create_account"));
    phone_edit_->setPlaceholderText(tr_key("phone_placeholder"));
    register_username_edit_->setPlaceholderText(tr_key("username_placeholder"));
    password_edit_->setPlaceholderText(tr_key("password_placeholder"));
    login_hero_text_label_->setText(tr_key("hero_text"));
    login_hero_caption_label_->setText(tr_key("hero_caption"));
    if (language_switch_button_) {
        language_switch_button_->setText(
            current_language_ == "en" ? tr_key("lang_button_en")
                : current_language_ == "uk" ? tr_key("lang_button_uk")
                : tr_key("lang_button_ru")
        );
    }

    const bool register_mode = register_mode_checkbox_->isChecked();
    login_button_->setText(register_mode ? tr_key("create_button") : tr_key("login_button"));

    if (settings_tabs_) {
        settings_tabs_->setTabText(0, tr_key("tab_profile"));
        settings_tabs_->setTabText(1, tr_key("tab_app"));
        settings_tabs_->setTabText(2, tr_key("tab_chats"));
        settings_tabs_->setTabText(3, tr_key("tab_privacy"));
        settings_tabs_->setTabText(4, tr_key("tab_premium"));
    }
    if (settings_language_combo_) {
        const int idx = settings_language_combo_->findData(current_language_);
        if (idx >= 0 && settings_language_combo_->currentIndex() != idx) {
            settings_language_combo_->setCurrentIndex(idx);
        }
    }
    update_phone_country_badge(phone_edit_->text());
}

void MainWindow::set_active_chat(const QString& chat_name, const QString& recipient_id) {
    current_recipient_id_ = recipient_id;
    chat_name_label_->setText(chat_name);
    chat_status_label_->setText("online");
}

void MainWindow::set_active_nav_button(QPushButton* active_button) {
    chats_nav_button_->setChecked(active_button == chats_nav_button_);
    settings_nav_button_->setChecked(active_button == settings_nav_button_);
}

void MainWindow::append_my_message(const QString& text) {
    const QString t = QDateTime::currentDateTime().toString("hh:mm");
    const QString checks = read_receipts_checkbox_ && read_receipts_checkbox_->isChecked() ? "✓✓" : "✓";

    chat_log_->append(
        QString("<div style='text-align:right;margin:10px 0;'>"
                "<span style='display:inline-block;background:#101316;color:#ffffff;"
                "padding:11px 13px;border-radius:18px 18px 5px 18px;max-width:70%%;"
                "border:1px solid #000000;box-shadow:0 2px 6px rgba(0,0,0,0.2);'>%1</span>"
                "<div style='color:#7f93a7;font-size:11px;margin-top:3px;'>%2 &nbsp; <span style='color:#7fdcff;'>%3</span></div></div>")
            .arg(text.toHtmlEscaped())
            .arg(t, checks)
    );
    animate_widget_fade(chat_log_);
}

void MainWindow::append_peer_message(const QString& text) {
    const QString t = QDateTime::currentDateTime().toString("hh:mm");
    chat_log_->append(
        QString("<div style='text-align:left;margin:10px 0;'>"
                "<span style='display:inline-block;background:#ffffff;color:#101316;"
                "padding:11px 13px;border-radius:18px 18px 18px 5px;max-width:70%%;"
                "border:1px solid #000000;box-shadow:0 1px 2px rgba(0,0,0,0.08);'>%1</span>"
                "<div style='color:#5f7489;font-size:11px;margin-top:3px;'>%2</div></div>")
            .arg(text.toHtmlEscaped())
            .arg(t)
    );
    animate_widget_fade(chat_log_);
}

void MainWindow::on_chat_selected(QListWidgetItem* item) {
    if (!item) {
        return;
    }

    const QString recipient_id = item->data(Qt::UserRole).toString();
    const QString chat_name = item->data(Qt::UserRole + 1).toString();
    set_active_chat(chat_name, recipient_id);
    on_open_chats();
}

void MainWindow::on_chat_filter_changed(const QString& text) {
    for (int i = 0; i < chats_list_->count(); ++i) {
        QListWidgetItem* item = chats_list_->item(i);
        const QString raw = item->text();
        item->setHidden(!raw.contains(text, Qt::CaseInsensitive));
    }
}

void MainWindow::on_open_chats() {
    content_stack_->setCurrentWidget(chat_panel_);
    set_active_nav_button(chats_nav_button_);
    animate_widget_fade(chat_panel_);
}

void MainWindow::on_open_settings() {
    content_stack_->setCurrentWidget(settings_panel_);
    set_active_nav_button(settings_nav_button_);
    animate_widget_fade(settings_panel_);
}

void MainWindow::on_auth_mode_toggled(bool checked) {
    if (!register_username_edit_ || !login_button_) {
        return;
    }
    register_username_edit_->setVisible(checked);
    login_button_->setText(checked ? tr_key("create_button") : tr_key("login_button"));
}

void MainWindow::on_phone_input_changed(const QString& phone) {
    update_phone_country_badge(phone);
}

void MainWindow::on_language_changed(int index) {
    if (!settings_language_combo_) {
        return;
    }
    current_language_ = settings_language_combo_->itemData(index).toString();
    apply_language();
}

void MainWindow::on_language_cycle() {
    static const QString codes[] = {"ru", "en", "uk"};
    int pos = 0;
    for (int i = 0; i < 3; ++i) {
        if (codes[i] == current_language_) {
            pos = i;
            break;
        }
    }
    current_language_ = codes[(pos + 1) % 3];
    apply_language();
}

void MainWindow::on_subscribe_premium() {
    premium_enabled_ = true;
    premium_status_label_->setText("Текущий статус: Premium Active");
    premium_subscribe_button_->setText("Premium активирован");
    premium_subscribe_button_->setEnabled(false);
    if (!premium_button_anim_checkbox_ || premium_button_anim_checkbox_->isChecked()) {
        animate_button_pulse(premium_subscribe_button_);
    }
    if (premium_orb_widget_) {
        premium_orb_widget_->set_anim_enabled(!premium_orb_anim_checkbox_ || premium_orb_anim_checkbox_->isChecked());
        premium_orb_widget_->set_speed_ms(premium_anim_ms_);
    }
    update_account_badge();
    update_profile_preview();
    animate_widget_fade(settings_panel_);
}

void MainWindow::on_login_clicked() {
    login_status_->clear();

    const QString phone = phone_edit_->text().trimmed();
    const QString password = password_edit_->text().trimmed();

    QRegularExpression phone_rx("^(\\+888[0-9]{3,12}|\\+?[0-9]{7,15})$");
    if (!phone_rx.match(phone).hasMatch()) {
        login_status_->setText(tr_key("invalid_phone"));
        return;
    }

    if (password.isEmpty()) {
        login_status_->setText(tr_key("enter_password"));
        return;
    }

    const bool register_mode = register_mode_checkbox_ && register_mode_checkbox_->isChecked();
    const QString username = register_username_edit_ ? register_username_edit_->text().trimmed() : QString();
    if (register_mode && username.size() < 3) {
        login_status_->setText(tr_key("username_min"));
        return;
    }

    login_button_->setEnabled(false);
    login_button_->setText(register_mode ? tr_key("creating") : tr_key("logging_in"));
    animate_button_pulse(login_button_);

    const auto res = register_mode
        ? auth_api_.register_account({phone.toStdString(), password.toStdString(), username.toStdString()})
        : auth_api_.login({phone.toStdString(), password.toStdString()});

    login_button_->setEnabled(true);
    login_button_->setText(register_mode ? tr_key("create_button") : tr_key("login_button"));

    if (!res.ok()) {
        const QString detail = QString::fromStdString(res.body.value("detail", ""));
        if (detail.isEmpty()) {
            login_status_->setText(QString("Ошибка входа: HTTP %1").arg(res.status_code));
        } else {
            login_status_->setText(QString("Ошибка входа: HTTP %1 (%2)").arg(res.status_code).arg(detail));
        }
        return;
    }

    access_token_ = res.body.value("access_token", "");
    if (access_token_.empty()) {
        login_status_->setText("Сервер не вернул токен");
        return;
    }

    if (register_mode && profile_username_edit_) {
        profile_username_edit_->setText(username);
    }
    finalize_auth_success(phone);
}

void MainWindow::on_send_clicked() {
    const QString text = message_edit_->text().trimmed();

    if (text.isEmpty() || current_recipient_id_.isEmpty()) {
        return;
    }

    send_button_->setEnabled(false);
    const auto res = msg_api_.send_message(access_token_, {current_recipient_id_.toStdString(), text.toStdString()});
    send_button_->setEnabled(true);

    if (!res.ok()) {
        chat_status_label_->setText(QString("Ошибка отправки: HTTP %1").arg(res.status_code));
        return;
    }

    animate_button_pulse(send_button_);
    append_my_message(text);
    chat_status_label_->setText("online");
    message_edit_->clear();
}

} // namespace pingram::ui
