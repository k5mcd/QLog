#include <QApplication>
#include <QtSql/QtSql>
#include <QMessageBox>
#include <QProgressDialog>
#include <QResource>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>
#include <QTime>
#include <QSystemSemaphore>
#include <QSharedMemory>
#include <QMessageBox>
#include <QDebug>
#include <QSplashScreen>

#include "debug.h"
#include "Migration.h"
#include "ui/MainWindow.h"
#include "Rig.h"
#include "Rotator.h"
#include "AppGuard.h"
#include "logformat/AdiFormat.h"

MODULE_IDENTIFICATION("qlog.core.main");

QMutex debug_mutex;

static void loadStylesheet(QApplication* app) {
    FCT_IDENTIFICATION;

    QFile style(":/res/stylesheet.css");
    style.open(QFile::ReadOnly | QIODevice::Text);
    app->setStyleSheet(style.readAll());
    style.close();
}

static void setupTranslator(QApplication* app) {
    FCT_IDENTIFICATION;

    QTranslator* qtTranslator = new QTranslator(app);
    qtTranslator->load("qt_" + QLocale::system().name(),
    QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app->installTranslator(qtTranslator);

    QTranslator* translator = new QTranslator(app);
    translator->load(":/i18n/qlog_" + QLocale::system().name().left(2));
    app->installTranslator(translator);
}

static void createDataDirectory() {
    FCT_IDENTIFICATION;

    QDir dataDir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
    qCDebug(runtime) << dataDir.path();

    if (!dataDir.exists()) {
        dataDir.mkpath(dataDir.path());
    }
}

static bool openDatabase() {
    FCT_IDENTIFICATION;

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    QDir dir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
    QString path = dir.filePath("qlog.db");
    db.setDatabaseName(path);

    if (!db.open()) {
        qCritical() << db.lastError();
        return false;
    }
    else {
        return true;
    }
}

static bool backupDatabase()
{
    FCT_IDENTIFICATION;
    /* remove old backups */
    /* retention time is 30 days but a minimum number of backup files is 5 */
    const int retention_time = 30;
    const int min_backout_count = 5;

    QDir dir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
    QString path = dir.filePath("qlog_backup_" + QDateTime::currentDateTime().toString("yyyyMMddhhmmss") + ".adi");
    QString filter("qlog_backup_%1%1%1%1%1%1%1%1%1%1%1%1%1%1.adi");
    filter = filter.arg("[0123456789]");

    QFileInfoList file_list = QDir(dir).entryInfoList(QStringList(filter), QDir::Files, QDir::Name);

    qCDebug(runtime) << file_list;

    /* Keep the minimum number of backups */
    /* If a number of backup is greater than min_backout_count then remove files older 30 days but always
       protect a minimum number of backup file files */
    for (int i = 0; i < qMin(min_backout_count, file_list.size()); i++)
    {
        file_list.takeLast();  // remove last 5 files from sorted list by name to protect them from removing.
    }

    /* remove those older than 30 days from the remaining files. */
    Q_FOREACH (auto fileInfo, file_list)
    {
        if (fileInfo.birthTime().date().daysTo(QDate::currentDate()) > retention_time)
        {
            QString filepath = fileInfo.absoluteFilePath();
            QDir deletefile;
            deletefile.setPath(filepath);
            deletefile.remove(filepath);
            qCDebug(runtime) << "Removing file: " << filepath;
        }
    }

    /* make a backup file */
    QFile backup_file(path);

    if ( !backup_file.open(QFile::ReadWrite | QIODevice::Text))
    {
        qWarning()<<"Cannot open backup file " << path << "for writing";
        return false;
    }

    qCDebug(runtime)<<"Exporting a Database backup to " << path;

    QTextStream stream(&backup_file);
    AdiFormat adi(stream);

    adi.runExport();
    stream.flush();
    backup_file.close();

    qCDebug(runtime)<<"Database backup finished";
    return true;
}
static bool migrateDatabase() {
    FCT_IDENTIFICATION;

    Migration m;
    return m.run();
}

static void startRigThread() {
    FCT_IDENTIFICATION;

    QThread* rigThread = new QThread;
    Rig* rig = Rig::instance();
    rig->moveToThread(rigThread);
    QObject::connect(rigThread, SIGNAL(started()), rig, SLOT(start()));
    rigThread->start();
}

static void startRotThread() {
    FCT_IDENTIFICATION;

    QThread* rotThread = new QThread;
    Rotator* rot = Rotator::instance();
    rot->moveToThread(rotThread);
    QObject::connect(rotThread, SIGNAL(started()), rot, SLOT(start()));
    rotThread->start();
}

static void debugMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    debug_mutex.lock();

    QByteArray localMsg = msg.toLocal8Bit();
    QString severity_string;

    switch (type)
    {
    case QtDebugMsg:
        severity_string = "[DEBUG   ]";
        break;
    case QtInfoMsg:
        severity_string = "[INFO    ]";
        break;
    case QtWarningMsg:
        severity_string = "[WARNING ]";
        break;
    case QtCriticalMsg:
        severity_string = "[CRITICAL]";
        break;
    case QtFatalMsg:
        severity_string = "[FATAL   ]";
        break;
    default:
        severity_string = "[UNKNOWN ]";
    }

    QString cat(context.category);
    if ( cat == "default" )
    {
        cat = "[             ]\t";
    }
    else
    {
        cat = "[" + cat + "]\t";
    }

    fprintf(stderr, "%s %s%s=> %s [%s:%s:%u] \n",
                                                 QTime::currentTime().toString("HH:mm:ss.zzz").toLocal8Bit().constData(),
                                                 severity_string.toLocal8Bit().constData(),
                                                 cat.toLocal8Bit().constData(),
                                                 localMsg.constData(),
                                                 context.function,
                                                 context.file,
                                                 context.line
                                                 );
    debug_mutex.unlock();

    if ( type == QtFatalMsg )
    {
        abort();
    }
}

int main(int argc, char* argv[]) {


    QApplication app(argc, argv);

    app.setApplicationVersion(VERSION);
    app.setOrganizationName("hamradio");
    app.setApplicationName("QLog");

    qInstallMessageHandler(debugMessageOutput);

    set_debug_level(LEVEL_PRODUCTION); // you can set more verbose rules via
                                       // environment variable QT_LOGGING_RULES (project setting/debug)

    loadStylesheet(&app);
    setupTranslator(&app);

    /* Application Singleton
     *
     * Only one instance of QLog application is allowed
     */
    AppGuard guard( "QLog" );
    if ( !guard.tryToRun() )
    {
        QMessageBox::critical(nullptr, QMessageBox::tr("QLog Error"),
                              QMessageBox::tr("QLog is already running"));
        return 1;
    }

    QPixmap pixmap(":/res/qlog.png");
    QSplashScreen splash(pixmap);
    splash.show();

    createDataDirectory();

    if (!openDatabase()) {
        QMessageBox::critical(nullptr, QMessageBox::tr("QLog Error"),
                              QMessageBox::tr("Could not connect to database."));
        return 1;
    }

    /* a migration can break a database therefore a backup is call before it */
    if (!backupDatabase())
    {
        QMessageBox::critical(nullptr, QMessageBox::tr("QLog Error"),
                              QMessageBox::tr("Could not export a QLog database to ADIF as a backup.<p>Try to export your log to ADIF manually"));
    }

    if (!migrateDatabase()) {
        QMessageBox::critical(nullptr, QMessageBox::tr("QLog Error"),
                              QMessageBox::tr("Database migration failed."));
        return 1;
    }

    startRigThread();
    startRotThread();

    MainWindow w;
    QIcon icon(":/res/qlog.png");
    splash.finish(&w);
    w.setWindowIcon(icon);
    w.show();

    return app.exec();
}
