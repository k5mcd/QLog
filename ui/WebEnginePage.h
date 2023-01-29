#ifndef WEBENGINEPAGE_H
#define WEBENGINEPAGE_H

#include <QWebEnginePage>

class WebEnginePage : public QWebEnginePage
{
public:
    explicit WebEnginePage(QObject *parent = nullptr);

protected:
    void javaScriptConsoleMessage(QWebEnginePage::JavaScriptConsoleMessageLevel level,
                                  const QString &message,
                                  int lineNumber,
                                  const QString &sourceID) override;
};

#endif // WEBENGINEPAGE_H
