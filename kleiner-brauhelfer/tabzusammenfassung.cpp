#include "tabzusammenfassung.h"
#include "ui_tabzusammenfassung.h"
#include <QFileInfo>
#include <QFileDialog>
#include <QDesktopServices>
#include "brauhelfer.h"
#include "settings.h"
#include "templatetags.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

TabZusammenfassung::TabZusammenfassung(QWidget *parent) :
    TabAbstract(parent),
    ui(new Ui::TabZusammenfassung)
{
    ui->setupUi(this);

    connect(bh, SIGNAL(modified()), this, SLOT(updateWebView()), Qt::QueuedConnection);
    connect(bh, SIGNAL(discarded()), this, SLOT(updateWebView()), Qt::QueuedConnection);
    connect(bh->sud(), SIGNAL(loadedChanged()), this, SLOT(updateWebView()), Qt::QueuedConnection);
}

TabZusammenfassung::~TabZusammenfassung()
{
    delete ui;
}

void TabZusammenfassung::onTabActivated()
{
    updateWebView();
}

void TabZusammenfassung::updateWebView()
{
    if (!isTabActive())
        return;
    Brauhelfer::SudStatus status = static_cast<Brauhelfer::SudStatus>(bh->sud()->getStatus());
    if (status == Brauhelfer::SudStatus::Rezept)
        ui->webview->setHtmlFile("spickzettel.html");
    else
        ui->webview->setHtmlFile("zusammenfassung.html");
    TemplateTags::render(ui->webview, TemplateTags::TagAll, bh->sud()->row());
}

void TabZusammenfassung::printPreview()
{
    ui->webview->printPreview();
}

void TabZusammenfassung::toPdf()
{
    gSettings->beginGroup("General");

    QString path = gSettings->value("exportPath", QDir::homePath()).toString();

    QString fileName = bh->sud()->getSudname();
    QString filePath = QFileDialog::getSaveFileName(this, tr("PDF speichern unter"),
                                     path + "/" + fileName +  ".pdf", "PDF (*.pdf)");
    if (!filePath.isEmpty())
    {
        gSettings->setValue("exportPath", QFileInfo(filePath).absolutePath());
        QRectF rect = gSettings->value("PrintMargins", QRectF(5, 10, 5, 15)).toRectF();
        QMarginsF margins = QMarginsF(rect.left(), rect.top(), rect.width(), rect.height());
        ui->webview->printToPdf(filePath, margins);
        QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
    }

    gSettings->endGroup();
}

void TabZusammenfassung::on_btnToPdf_clicked()
{
    toPdf();
}

void TabZusammenfassung::on_btnPrintPreview_clicked()
{
    printPreview();
}
