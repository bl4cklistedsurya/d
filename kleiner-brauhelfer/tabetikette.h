#ifndef TABETIKETTE_H
#define TABETIKETTE_H

#include "tababstract.h"
#include "helper/htmlhighlighter.h"

namespace Ui {
class TabEtikette;
}

class QPrinter;

class TabEtikette : public TabAbstract
{
    Q_OBJECT

public:
    explicit TabEtikette(QWidget *parent = nullptr);
    virtual ~TabEtikette() Q_DECL_OVERRIDE;
    void printPreview() Q_DECL_OVERRIDE;
    void toPdf() Q_DECL_OVERRIDE;

private slots:
    void updateAll();
    void updateValues();
    void updateTemplateFilePath();
    void updateSvg();
    void updateTags();
    void updateTemplateTags();
    void updateAuswahlListe();
    void onPrinterPaintRequested(QPrinter *printer);
    void on_cbAuswahl_activated(int index);
    void on_tbAnzahl_valueChanged(int value);
    void on_tbLabelBreite_valueChanged(double value);
    void on_tbLabelHoehe_valueChanged(double value);
    void on_cbSeitenverhaeltnis_clicked(bool checked);
    void on_btnGroesseAusSvg_clicked();
    void on_tbAbstandHor_valueChanged(double value);
    void on_tbAbstandVert_valueChanged(double value);
    void on_tbRandOben_valueChanged(double value);
    void on_tbRandLinks_valueChanged(double value);
    void on_tbRandRechts_valueChanged(double value);
    void on_tbRandUnten_valueChanged(double value);
    void on_cbTagsErsetzen_stateChanged();
    void on_cbEditMode_clicked(bool checked);
    void on_tbTemplate_textChanged();
    void on_btnSaveTemplate_clicked();
    void on_btnToPdf_clicked();
    void on_btnLoeschen_clicked();

private:
    void onTabActivated() Q_DECL_OVERRIDE;
    bool checkSave();
    QString generateSvg(const QString &svg);
    QVariant data(int col) const;
    bool setData(int col, const QVariant &value);

private:
    Ui::TabEtikette *ui;
    QString mTemplateFilePath;
    HtmlHighLighter* mHtmlHightLighter;
    QVariantMap mTemplateTags;
    QPrinter* mPrinter;
};

#endif // TABETIKETTE_H
