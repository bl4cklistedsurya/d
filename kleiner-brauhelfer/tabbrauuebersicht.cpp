#include "tabbrauuebersicht.h"
#include "ui_tabbrauuebersicht.h"
#include "brauhelfer.h"
#include "settings.h"
#include "proxymodelsud.h"
#include "model/datedelegate.h"
#include "model/doublespinboxdelegate.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

class ProxyModelBrauuebersicht : public ProxyModelSud
{
public:

    ProxyModelBrauuebersicht(QObject* parent = nullptr) :
        ProxyModelSud(parent)
    {
    }

    using ProxyModelSud::data;

    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE
    {
        if (role == Qt::BackgroundRole)
        {
            if (index.column() == mColAuswahl1)
                return gSettings->DiagramLinie1Light;
            if (index.column() == mColAuswahl2)
                return gSettings->DiagramLinie2Light;
        }
        return ProxyModelSud::data(index, role);
    }

    int mColAuswahl1;
    int mColAuswahl2;
};

TabBrauUebersicht::TabBrauUebersicht(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabBrauUebersicht)
{
    ui->setupUi(this);

    gSettings->beginGroup("TabBrauuebersicht");

    ui->splitter->setSizes({INT_MAX, INT_MAX});
    mDefaultSplitterState = ui->splitter->saveState();
    ui->splitter->restoreState(gSettings->value("splitterState").toByteArray());

    ui->diagram->colorL1 = gSettings->DiagramLinie1;
    ui->diagram->colorL2 = gSettings->DiagramLinie2;
    ui->diagram->colorL3 = gSettings->DiagramLinie3;

    mAuswahlListe.append({"", 0, tr("<keine>"), "", 0, 0});
    mAuswahlListe.append({"erg_AbgefuellteBiermenge", 1, tr("Abgefüllte Biermenge [l]"), tr("l"), 0, 0});
    mAuswahlListe.append({"SWIst", 1, tr("Stammwürze [°P]"), tr("°P"), 0, 0});
    mAuswahlListe.append({"erg_Sudhausausbeute", 0, tr("Sudhausausbeute [%]"), tr("%"), 0, 90});
    mAuswahlListe.append({"erg_EffektiveAusbeute", 0, tr("Effektive Sudhausausbeute [%]"), tr("%"), 0, 90});
    mAuswahlListe.append({"erg_S_Gesammt", 1, tr("Gesamtschüttung [kg]"), tr("kg"), 0, 0});
    mAuswahlListe.append({"erg_Alkohol", 1, tr("Alkohol [%]"), tr("%"), 0, 0});
    mAuswahlListe.append({"sEVG", 0, tr("Scheinbarer Endvergärungsgrad [%]"), tr("%"), 0, 90});
    mAuswahlListe.append({"tEVG", 0, tr("Tatsächlicher Endvergärungsgrad [%]"), tr("%"), 0, 90});
    mAuswahlListe.append({"erg_Preis", 2, tr("Kosten [%1/l]").arg(QLocale().currencySymbol()), tr("%1/l").arg(QLocale().currencySymbol()), 0, 0});

    gSettings->endGroup();
}

TabBrauUebersicht::~TabBrauUebersicht()
{
    delete ui;
}

void TabBrauUebersicht::saveSettings()
{
    gSettings->beginGroup("TabBrauuebersicht");
    gSettings->setValue("tableState", ui->tableView->horizontalHeader()->saveState());
    gSettings->setValue("Auswahl1", ui->cbAuswahlL1->currentIndex());
    gSettings->setValue("Auswahl2", ui->cbAuswahlL2->currentIndex());
    gSettings->setValue("splitterState", ui->splitter->saveState());
    gSettings->endGroup();
}

void TabBrauUebersicht::restoreView()
{
    ui->tableView->horizontalHeader()->restoreState(mDefaultTableState);
    ui->splitter->restoreState(mDefaultSplitterState);
}

void TabBrauUebersicht::setModel(QAbstractItemModel* model)
{
    int col;
    QTableView *table = ui->tableView;
    QHeaderView *header = table->horizontalHeader();
    ProxyModelBrauuebersicht *proxyModel = new ProxyModelBrauuebersicht(this);
    proxyModel->setSourceModel(model);
    proxyModel->setFilterStatus(ProxyModelSud::Abgefuellt);
    proxyModel->sort(proxyModel->fieldIndex("Braudatum"), Qt::AscendingOrder);
    table->setModel(proxyModel);
    for (int col = 0; col < proxyModel->columnCount(); ++col)
        table->setColumnHidden(col, true);

    col = proxyModel->fieldIndex("Sudname");
    table->setColumnHidden(col, false);
    header->resizeSection(col, 300);
    header->moveSection(header->visualIndex(col), 0);

    col = proxyModel->fieldIndex("Braudatum");
    table->setColumnHidden(col, false);
    table->setItemDelegateForColumn(col, new DateDelegate(false, false, table));
    header->resizeSection(col, 100);
    header->moveSection(header->visualIndex(col), 1);

    ui->cbAuswahlL2->addItem(mAuswahlListe[0].label);
    for (int i = 1; i < mAuswahlListe.count(); ++i)
    {
        col = proxyModel->fieldIndex(mAuswahlListe[i].field);
        ui->cbAuswahlL1->addItem(mAuswahlListe[i].label);
        ui->cbAuswahlL2->addItem(mAuswahlListe[i].label);
        table->setColumnHidden(col, false);
        table->setItemDelegateForColumn(col, new DoubleSpinBoxDelegate(mAuswahlListe[i].precision, table));
        header->resizeSection(col, 100);
        header->moveSection(header->visualIndex(col), i + 1);
    }

    gSettings->beginGroup("TabBrauuebersicht");

    mDefaultTableState = header->saveState();
    header->restoreState(gSettings->value("tableState").toByteArray());

    ui->cbAuswahlL1->setCurrentIndex(gSettings->value("Auswahl1").toInt());
    ui->cbAuswahlL2->setCurrentIndex(gSettings->value("Auswahl2").toInt());

    gSettings->endGroup();

    connect(model, SIGNAL(layoutChanged()), this, SLOT(updateDiagram()));
    connect(table->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
            this, SLOT(table_selectionChanged(const QItemSelection&)));
    connect(ui->diagram, SIGNAL(sig_selectionChanged(int)),
            this, SLOT(diagram_selectionChanged(int)));

    updateDiagram();
}

void TabBrauUebersicht::updateDiagram()
{
    ui->diagram->DiagrammLeeren();
    ProxyModelBrauuebersicht *model = static_cast<ProxyModelBrauuebersicht*>(ui->tableView->model());
    if (model->rowCount() > 1)
    {
        AuswahlType *auswahl1 = &mAuswahlListe[ui->cbAuswahlL1->currentIndex() + 1];
        model->mColAuswahl1 = model->fieldIndex(auswahl1->field);
        ui->diagram->BezeichnungL1 = auswahl1->label;
        ui->diagram->KurzbezeichnungL1 = auswahl1->unit;
        ui->diagram->L1Min = auswahl1->min;
        ui->diagram->L1Max = auswahl1->max;

        AuswahlType *auswahl2 = &mAuswahlListe[ui->cbAuswahlL2->currentIndex()];
        model->mColAuswahl2 = model->fieldIndex(auswahl2->field);
        ui->diagram->BezeichnungL2 = auswahl2->label;
        ui->diagram->KurzbezeichnungL2 = auswahl2->unit;
        ui->diagram->L2Min = auswahl2->min;
        ui->diagram->L2Max = auswahl2->max;

        int colBraudatum = model->fieldIndex("Braudatum");
        int colId = model->fieldIndex("ID");
        for (int row = 0; row < model->rowCount(); ++row)
        {
            double d, f;
            QDateTime dt = model->index(row, colBraudatum).data().toDateTime();
            ui->diagram->Ids.append(model->index(row, colId).data().toInt());
            d = model->index(row, model->mColAuswahl1).data().toDouble();
            f = pow(10, auswahl1->precision);
            d = qRound(d * f) / f;
            ui->diagram->L1Daten.append(d);
            ui->diagram->L1Datum.append(dt);
            if (model->mColAuswahl2 >= 0)
            {
                d = model->index(row, model->mColAuswahl2).data().toDouble();
                f = pow(10, auswahl2->precision);
                d = qRound(d * f) / f;
                ui->diagram->L2Daten.append(d);
                ui->diagram->L2Datum.append(dt);
            }
        }
    }
    ui->diagram->repaint();
}

void TabBrauUebersicht::on_tableView_doubleClicked(const QModelIndex &index)
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableView->model());
    int sudId = model->data(index.row(), "ID").toInt();
    clicked(sudId);
}

void TabBrauUebersicht::table_selectionChanged(const QItemSelection &selected)
{
    int sudId = -1;
    if (selected.indexes().count() > 0)
    {
        ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableView->model());
        const QModelIndex index = selected.indexes()[0];
        sudId = index.siblingAtColumn(model->fieldIndex("ID")).data().toInt();
    }
    ui->diagram->MarkierePunkt(sudId);
}

void TabBrauUebersicht::diagram_selectionChanged(int sudId)
{
    ProxyModelSud *model = static_cast<ProxyModelSud*>(ui->tableView->model());
    int colId = model->fieldIndex("ID");
    for (int row = 0; row < ui->tableView->model()->rowCount(); ++row)
    {
        if (ui->tableView->model()->index(row, colId).data().toInt() == sudId)
        {
            ui->tableView->selectRow(row);
            break;
        }
    }
}

void TabBrauUebersicht::on_cbAuswahlL1_currentIndexChanged(int)
{
    if (ui->cbAuswahlL1->hasFocus())
    {
        updateDiagram();
        ui->tableView->setFocus();
    }
}

void TabBrauUebersicht::on_cbAuswahlL2_currentIndexChanged(int)
{
    if (ui->cbAuswahlL2->hasFocus())
    {
        updateDiagram();
        ui->tableView->setFocus();
    }
}
