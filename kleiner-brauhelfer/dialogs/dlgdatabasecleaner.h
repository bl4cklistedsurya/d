#ifndef DLGDATABASECLEANER_H
#define DLGDATABASECLEANER_H

#include <QDialog>
#include <functional>
#include "sqltablemodel.h"

namespace Ui {
class DlgDatabaseCleaner;
}

class DlgDatabaseCleaner : public QDialog
{
    Q_OBJECT

public:
    explicit DlgDatabaseCleaner(QWidget* parent = nullptr);
    ~DlgDatabaseCleaner();

private slots:
    void selectionChanged();
    void on_btnDelete_clicked();
    void on_btnWeiter_clicked();

private:
    void next();
    void setTableIds(int type);
    bool testNullField(SqlTableModel* model, const QList<int>& fields, int type);
    bool testInvalidId(SqlTableModel* model, const QList<int>& fields, int type);

private:
    Ui::DlgDatabaseCleaner *ui;
    QMap<int, QString> mSudIds;
    QMap<int, QString> mAnlagenIds;
    QList<std::function<bool()> > mTestFncs;
    QList<std::function<bool()> >::const_iterator mItTestFncs;
};

#endif // DLGDATABASECLEANER_H
