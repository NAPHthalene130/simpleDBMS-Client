#ifndef AUTHWIDGET_H
#define AUTHWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QFormLayout>

class AuthWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AuthWidget(QWidget *parent = nullptr);
    ~AuthWidget();

    signals:
        void loginSuccess(const QString &host, int port,
                          const QString &user, const QString &password);

private slots:
    void onLoginClicked();
    void onTestConnection();

private:
    void setupUI();

    QLineEdit *m_hostEdit;
    QLineEdit *m_portEdit;
    QLineEdit *m_userEdit;
    QLineEdit *m_passwordEdit;
    QPushButton *m_loginBtn;
    QPushButton *m_testBtn;
};

#endif // AUTHWIDGET_H