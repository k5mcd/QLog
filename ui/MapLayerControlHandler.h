#ifndef MAPLAYERCONTROLHANDLER_H
#define MAPLAYERCONTROLHANDLER_H

#include <QObject>
#include <QWebEnginePage>

class MapLayerControlHandler : public QObject
{
    Q_OBJECT
public:
    explicit MapLayerControlHandler(const QString configID,
                                    QObject *parent = nullptr);
    void restoreControls(QWebEnginePage *page);
    QString injectMapMenuJS(bool gridLayer = true,
                            bool grayline = false,
                            bool aurora = false);

public slots:
    void handleLayerSelectionChanged(const QVariant &data,
                                     const QVariant &state);
private:
    QString configID;

    void connectChannel(QWebEnginePage *page);
};

#endif // MAPLAYERCONTROLHANDLER_H
