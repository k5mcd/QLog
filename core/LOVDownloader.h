#ifndef LOVDOWNLOADER_H
#define LOVDOWNLOADER_H

#include <QObject>
#include <QNetworkAccessManager>

class LOVDownloader : public QObject
{
    Q_OBJECT

public:
    enum SourceType
    {
        CTY = 0,
        SATLIST = 1,
        SOTASUMMITS = 2,
        UNDEF = 3
    };

public:
    LOVDownloader(QObject *parent = nullptr);
    ~LOVDownloader();
    void update(const SourceType &);
    void abortRequest();

signals:
    void processingSize(qint64);
    void progress(qint64 count);
    void finished(bool result);
    void noUpdate();

private:
    class SourceDefinition
    {
    public:
        SourceDefinition(const SourceType type,
                         const QString &URL,
                         const QString &fileName,
                         const QString &lastTimeConfigName,
                         const QString &tableName,
                         int ageTime) : type(type),
                                        URL(URL),
                                        fileName(fileName),
                                        lastTimeConfigName(lastTimeConfigName),
                                        tableName(tableName),
                                        ageTime(ageTime) {};
        SourceDefinition() : type(LOVDownloader::UNDEF),
                                  ageTime(0){};
        SourceType type;
        QString URL;
        QString fileName;
        QString lastTimeConfigName;
        QString tableName;
        int ageTime;
    };

    QMap<SourceType, SourceDefinition> sourceMapping = {
        {CTY, SourceDefinition(CTY,
                               "http://www.country-files.com/cty/cty.csv",
                               "cty.csv",
                               "last_cty_update",
                               "dxcc_entities",
                               21)},
        {SATLIST, SourceDefinition(SATLIST,
                                   "https://foldynl.github.io/QLog/data/satslist.csv",
                                   "satslist.csv",
                                   "last_sat_update",
                                   "sat_info",
                                   30)},
        {SOTASUMMITS, SourceDefinition(SOTASUMMITS,
                                   "https://mapping.sota.org.uk/summitslist.csv",
                                   "summitslist.csv",
                                   "last_sotasummits_update",
                                   "sota_summits",
                                   30)}
    };

    QNetworkAccessManager* nam;
    QNetworkReply *currentReply;
    bool abortRequested;

private:
    bool isTableFilled(const QString &);
    bool deleteTable(const QString &);
    void download(const SourceDefinition &);
    void parseData(const LOVDownloader::SourceDefinition &,
                   QTextStream &);
    void parseCTY(const SourceDefinition &sourceDef, QTextStream& data);
    void parseSATLIST(const SourceDefinition &sourceDef, QTextStream& data);
    void parseSOTASummits(const SourceDefinition &sourceDef, QTextStream& data);

private slots:
    void processReply(QNetworkReply*);
    void loadData(const LOVDownloader::SourceDefinition &);

};

#endif // LOVDOWNLOADER_H
