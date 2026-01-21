#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

#include <QLineEdit>
#include "../Persistence/DatabaseManager.h"
#include <QMessageBox>

class LoginDialog : public QDialog {
    Q_OBJECT
public:
    LoginDialog(QWidget* parent = nullptr) : QDialog(parent) {
        setWindowTitle("Login - Parametric Sketcher");
        QVBoxLayout* layout = new QVBoxLayout(this);
        
        layout->addWidget(new QLabel("Username:"));
        m_usernameEdit = new QLineEdit(this);
        m_usernameEdit->setPlaceholderText("e.g. student, administrator");
        layout->addWidget(m_usernameEdit);

        layout->addWidget(new QLabel("Password:"));
        m_passwordEdit = new QLineEdit(this);
        m_passwordEdit->setEchoMode(QLineEdit::Password);
        m_passwordEdit->setPlaceholderText("password");
        layout->addWidget(m_passwordEdit);
        
        QPushButton* loginBtn = new QPushButton("Login", this);
        layout->addWidget(loginBtn);
        
        connect(loginBtn, &QPushButton::clicked, this, &LoginDialog::attemptLogin);
    }
    
    QString getSelectedActor() const { return m_role; }

private slots:
    void attemptLogin() {
        if (DatabaseManager::instance().checkLogin(m_usernameEdit->text(), m_passwordEdit->text(), m_role)) {
            accept();
        } else {
            QMessageBox::critical(this, "Error", "Invalid username or password.");
        }
    }

private:
    QLineEdit* m_usernameEdit;
    QLineEdit* m_passwordEdit;
    QString m_role;
};

#endif
