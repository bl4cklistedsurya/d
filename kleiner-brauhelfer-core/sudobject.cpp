#include "sudobject.h"
#include "brauhelfer.h"
#include "modelsud.h"
#include <QDateTime>
#include <QtMath>

SudObject::SudObject(Brauhelfer *bh) :
    QObject(bh),
    bh(bh),
    mId(-1),
    mRowSud(-1),
    proxyModelRasten(new ProxyModel(this)),
    proxyModelMalzschuettung(new ProxyModel(this)),
    proxyModelHopfengaben(new ProxyModel(this)),
    proxyModelHefegaben(new ProxyModel(this)),
    proxyModelWeitereZutatenGaben(new ProxyModel(this)),
    proxyModelSchnellgaerverlauf(new ProxyModel(this)),
    proxyModelHauptgaerverlauf(new ProxyModel(this)),
    proxyModelNachgaerverlauf(new ProxyModel(this)),
    proxyModelBewertungen(new ProxyModel(this)),
    proxyModelAnhang(new ProxyModel(this)),
    proxyModelEtiketten(new ProxyModel(this)),
    proxyModelTags(new ProxyModel(this))
{
    connect(bh, SIGNAL(saved()), this, SLOT(onSudLayoutChanged()));
    connect(bh->modelSud(), SIGNAL(modified()), this, SIGNAL(modified()));
    connect(bh->modelSud(), SIGNAL(layoutChanged()), this, SLOT(onSudLayoutChanged()));
    connect(bh->modelSud(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)),
            this, SLOT(onSudDataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)));
}

SudObject::~SudObject()
{
    delete proxyModelRasten;
    delete proxyModelMalzschuettung;
    delete proxyModelHopfengaben;
    delete proxyModelHefegaben;
    delete proxyModelWeitereZutatenGaben;
    delete proxyModelSchnellgaerverlauf;
    delete proxyModelHauptgaerverlauf;
    delete proxyModelNachgaerverlauf;
    delete proxyModelBewertungen;
    delete proxyModelAnhang;
    delete proxyModelEtiketten;
    delete proxyModelTags;
}

void SudObject::init()
{
    qInfo() << "SudObject::init()";
    modelRasten()->setSourceModel(bh->modelRasten());
    modelRasten()->setFilterKeyColumn(ModelRasten::ColSudID);
    modelMalzschuettung()->setSourceModel(bh->modelMalzschuettung());
    modelMalzschuettung()->setFilterKeyColumn(ModelMalzschuettung::ColSudID);
    modelHopfengaben()->setSourceModel(bh->modelHopfengaben());
    modelHopfengaben()->setFilterKeyColumn(ModelHopfengaben::ColSudID);
    modelHopfengaben()->sort(ModelHopfengaben::ColZeit, Qt::DescendingOrder);
    modelHefegaben()->setSourceModel(bh->modelHefegaben());
    modelHefegaben()->setFilterKeyColumn(ModelHefegaben::ColSudID);
    modelHefegaben()->sort(ModelHefegaben::ColZugabeNach, Qt::AscendingOrder);
    modelWeitereZutatenGaben()->setSourceModel(bh->modelWeitereZutatenGaben());
    modelWeitereZutatenGaben()->setFilterKeyColumn(ModelWeitereZutatenGaben::ColSudID);
    modelSchnellgaerverlauf()->setSourceModel(bh->modelSchnellgaerverlauf());
    modelSchnellgaerverlauf()->setFilterKeyColumn(ModelSchnellgaerverlauf::ColSudID);
    modelSchnellgaerverlauf()->sort(ModelSchnellgaerverlauf::ColZeitstempel, Qt::AscendingOrder);
    modelHauptgaerverlauf()->setSourceModel(bh->modelHauptgaerverlauf());
    modelHauptgaerverlauf()->setFilterKeyColumn(ModelHauptgaerverlauf::ColSudID);
    modelHauptgaerverlauf()->sort(ModelHauptgaerverlauf::ColZeitstempel, Qt::AscendingOrder);
    modelNachgaerverlauf()->setSourceModel(bh->modelNachgaerverlauf());
    modelNachgaerverlauf()->setFilterKeyColumn(ModelNachgaerverlauf::ColSudID);
    modelNachgaerverlauf()->sort(ModelNachgaerverlauf::ColZeitstempel, Qt::AscendingOrder);
    modelBewertungen()->setSourceModel(bh->modelBewertungen());
    modelBewertungen()->setFilterKeyColumn(ModelBewertungen::ColSudID);
    modelAnhang()->setSourceModel(bh->modelAnhang());
    modelAnhang()->setFilterKeyColumn(ModelAnhang::ColSudID);
    modelEtiketten()->setSourceModel(bh->modelEtiketten());
    modelEtiketten()->setFilterKeyColumn(ModelEtiketten::ColSudID);
    modelTags()->setSourceModel(bh->modelTags());
    modelTags()->setFilterKeyColumn(ModelTags::ColSudID);

    QRegExp regExpId(QString("^%1$").arg(mId), Qt::CaseInsensitive, QRegExp::RegExp);
    modelRasten()->setFilterRegExp(regExpId);
    modelMalzschuettung()->setFilterRegExp(regExpId);
    modelHopfengaben()->setFilterRegExp(regExpId);
    modelHefegaben()->setFilterRegExp(regExpId);
    modelWeitereZutatenGaben()->setFilterRegExp(regExpId);
    modelSchnellgaerverlauf()->setFilterRegExp(regExpId);
    modelHauptgaerverlauf()->setFilterRegExp(regExpId);
    modelNachgaerverlauf()->setFilterRegExp(regExpId);
    modelBewertungen()->setFilterRegExp(regExpId);
    modelAnhang()->setFilterRegExp(regExpId);
    modelEtiketten()->setFilterRegExp(regExpId);
    modelTags()->setFilterRegExp(QRegExp(QString("^(%1|-.*)$").arg(mId), Qt::CaseInsensitive, QRegExp::RegExp));
}

void SudObject::load(int id)
{
    if (mId != id)
    {
        mId = id;
        mRowSud = bh->modelSud()->getRowWithValue(ModelSud::ColID, mId);

        if (mId != -1)
            qInfo() << "SudObject::load():" << getSudname() << "(" << mId << ")";
        else
            qInfo() << "SudObject::unload()";

        QRegExp regExpId(QString("^%1$").arg(mId), Qt::CaseInsensitive, QRegExp::RegExp);
        modelRasten()->setFilterRegExp(regExpId);
        modelMalzschuettung()->setFilterRegExp(regExpId);
        modelHopfengaben()->setFilterRegExp(regExpId);
        modelHefegaben()->setFilterRegExp(regExpId);
        modelWeitereZutatenGaben()->setFilterRegExp(regExpId);
        modelSchnellgaerverlauf()->setFilterRegExp(regExpId);
        modelHauptgaerverlauf()->setFilterRegExp(regExpId);
        modelNachgaerverlauf()->setFilterRegExp(regExpId);
        modelBewertungen()->setFilterRegExp(regExpId);
        modelAnhang()->setFilterRegExp(regExpId);
        modelEtiketten()->setFilterRegExp(regExpId);
        modelTags()->setFilterRegExp(QRegExp(QString("^(%1|-.*)$").arg(mId), Qt::CaseInsensitive, QRegExp::RegExp));

        if (isLoaded())
        {
            QVariant qvId = getValue(ModelSud::ColID);
            if (!qvId.isValid() || qvId.toInt() != mId)
            {
                qCritical("SudObject::load() ID mismatch");
                unload();
                return;
            }
        }
        emit loadedChanged();
        emit modified();
    }
}

void SudObject::unload()
{
    load(-1);
}

bool SudObject::isLoaded() const
{
    return mId != -1;
}

void SudObject::onSudLayoutChanged()
{
    if (getValue(ModelSud::ColDeleted).toBool())
    {
        unload();
    }
    else if (mId != getValue(ModelSud::ColID).toInt())
    {
        mRowSud = bh->modelSud()->getRowWithValue(ModelSud::ColID, mId);
        if (mRowSud < 0)
           unload();
    }
}

void SudObject::onSudDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    if (mRowSud >= topLeft.row() && mRowSud <= bottomRight.row())
        emit dataChanged(topLeft, bottomRight, roles);
}

ProxyModel* SudObject::modelRasten() const
{
    return proxyModelRasten;
}

ProxyModel* SudObject::modelMalzschuettung() const
{
    return proxyModelMalzschuettung;
}

ProxyModel* SudObject::modelHopfengaben() const
{
    return proxyModelHopfengaben;
}

ProxyModel* SudObject::modelHefegaben() const
{
    return proxyModelHefegaben;
}

ProxyModel* SudObject::modelWeitereZutatenGaben() const
{
    return proxyModelWeitereZutatenGaben;
}

ProxyModel* SudObject::modelSchnellgaerverlauf() const
{
    return proxyModelSchnellgaerverlauf;
}

ProxyModel* SudObject::modelHauptgaerverlauf() const
{
    return proxyModelHauptgaerverlauf;
}

ProxyModel* SudObject::modelNachgaerverlauf() const
{
    return proxyModelNachgaerverlauf;
}

ProxyModel* SudObject::modelBewertungen() const
{
    return proxyModelBewertungen;
}

ProxyModel* SudObject::modelAnhang() const
{
    return proxyModelAnhang;
}

ProxyModel* SudObject::modelEtiketten() const
{
    return proxyModelEtiketten;
}

ProxyModel* SudObject::modelTags() const
{
    return proxyModelTags;
}

int SudObject::id() const
{
    return mId;
}

int SudObject::row() const
{
    return mRowSud;
}

QVariant SudObject::getValue(int col) const
{
    return bh->modelSud()->data(mRowSud, col);
}

bool SudObject::setValue(int col, const QVariant &value)
{
    qInfo() << "SudObject::setValue():" << bh->modelSud()->fieldName(col) << "=" << value.toString();
    return bh->modelSud()->setData(mRowSud, col, value);
}

QVariant SudObject::getAnlageData(int col) const
{
    return bh->modelSud()->dataAnlage(mRowSud, col);
}

QVariant SudObject::getWasserData(int col) const
{
    return bh->modelSud()->dataWasser(mRowSud, col);
}

void SudObject::brauzutatenAbziehen()
{
    ProxyModel *model = modelMalzschuettung();
    for (int r = 0; r < model->rowCount(); ++r)
    {
        bh->rohstoffAbziehen(Brauhelfer::RohstoffTyp::Malz,
                             model->data(r, ModelMalzschuettung::ColName).toString(),
                             model->data(r, ModelMalzschuettung::Colerg_Menge).toDouble());
    }

    model = modelHopfengaben();
    for (int r = 0; r < model->rowCount(); ++r)
    {
        bh->rohstoffAbziehen(Brauhelfer::RohstoffTyp::Hopfen,
                             model->data(r, ModelHopfengaben::ColName).toString(),
                             model->data(r, ModelHopfengaben::Colerg_Menge).toDouble());
    }

    model = modelHefegaben();
    for (int r = 0; r < model->rowCount(); ++r)
    {
        if (model->data(r, ModelHefegaben::ColZugabeNach).toInt() == 0)
        {
            bh->rohstoffAbziehen(Brauhelfer::RohstoffTyp::Hefe,
                                 model->data(r, ModelHefegaben::ColName).toString(),
                                 model->data(r, ModelHefegaben::ColMenge).toDouble());
        }
    }

    model = modelWeitereZutatenGaben();
    for (int r = 0; r < model->rowCount(); ++r)
    {
        Brauhelfer::ZusatzZeitpunkt zeitpunkt = static_cast<Brauhelfer::ZusatzZeitpunkt>(model->data(r, ModelWeitereZutatenGaben::ColZeitpunkt).toInt());
        if (zeitpunkt != Brauhelfer::ZusatzZeitpunkt::Gaerung || model->data(r, ModelWeitereZutatenGaben::ColZugabeNach).toInt() == 0)
        {
            Brauhelfer::ZusatzTyp typ = static_cast<Brauhelfer::ZusatzTyp>(model->data(r, ModelWeitereZutatenGaben::ColTyp).toInt());
            bh->rohstoffAbziehen(typ == Brauhelfer::ZusatzTyp::Hopfen ? Brauhelfer::RohstoffTyp::Hopfen : Brauhelfer::RohstoffTyp::Zusatz,
                                 model->data(r, ModelWeitereZutatenGaben::ColName).toString(),
                                 model->data(r, ModelWeitereZutatenGaben::Colerg_Menge).toDouble());
        }
    }
}
