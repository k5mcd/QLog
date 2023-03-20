#ifndef MAPWEBCHANNELHANDLER_H
#define MAPWEBCHANNELHANDLER_H

#include <QObject>
#include <QWebEnginePage>

class MapWebChannelHandler : public QObject
{
    Q_OBJECT
public:
    explicit MapWebChannelHandler(const QString configID,
                                    QObject *parent = nullptr);
    void restoreLayerControlStates(QWebEnginePage *page);
    QString generateMapMenuJS(bool gridLayer = true,
                              bool grayline = false,
                              bool aurora = false,
                              bool muf = false,
                              bool ibp = false);

public slots:
    void handleLayerSelectionChanged(const QVariant &data,
                                     const QVariant &state);
private:
    QString configID;

    void connectWebChannel(QWebEnginePage *page);
};

#endif // MAPWEBCHANNELHANDLER_H
