#ifndef DLGCHECKUPDATE_H
#define DLGCHECKUPDATE_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>

namespace Ui {
class DlgCheckUpdate;
}

class DlgCheckUpdate : public QDialog
{
    Q_OBJECT

public:
    explicit DlgCheckUpdate(const QString &url, const QDate& since, QWidget *parent = nullptr);
    ~DlgCheckUpdate();
    void checkForUpdate();
    bool hasUpdate() const;
    bool ignoreUpdate() const;
    bool doCheckUpdate() const;

signals:
    void finished();

private slots:
    void httpFinished();

private:
    bool parseReplyGithub(const QByteArray &str);

private:
    Ui::DlgCheckUpdate *ui;
    QUrl url;
    QNetworkAccessManager qnam;
    QNetworkReply *reply;
    bool mHasUpdate;
    QDate mDateSince;
};

#endif // DLGCHECKUPDATE_H
