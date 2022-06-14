#include <QApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QLocale>
#include <QSettings>
#include <QTranslator>
#include <QDebug>
#include <canvas/canvas.h>
#include <windows/osxwindow.h>
#include <windows/mainwindow.h>

#ifdef Q_OS_MACOS
#define MACOS
#endif

//start add for crashpad
#include <QApplication>
#include <vector>
#include "client/crash_report_database.h"
#include "client/crashpad_client.h"
#include "client/settings.h"

class Paths
{
    QString m_exeDir;

    public:
        Paths(QString exeDir);
        QString getAttachmentPath();
        QString getHandlerPath();
        QString getReportsPath();
        QString getMetricsPath();
        #if defined(Q_OS_UNIX)
            static std::string getPlatformString(QString string);
        #elif defined(Q_OS_WINDOWS)
            static std::wstring getPlatformString(QString string);
        #else
            #error getPlatformString not implemented on this platform
        #endif
};
Paths::Paths(QString exeDir) {
    m_exeDir = exeDir;
}

QString Paths::getAttachmentPath() {
    #if defined(Q_OS_MACOS) || defined(Q_OS_MAC64)
        return m_exeDir + "/../attachment.txt";
    #elif defined(Q_OS_WINDOWS)
        return m_exeDir + "\\..\\attachment.txt";
    #elif defined(Q_OS_LINUX)
        return m_exeDir + "/attachment.txt";
    #else
        #error getAttachmentPath() not implemented on this platform
    #endif
}

QString Paths::getHandlerPath() {
    #if defined(Q_OS_MAC)
        return m_exeDir + "/../../../crashpad/crashpad_handler";
    #elif defined(Q_OS_WINDOWS)
        return m_exeDir + "\\..\\crashpad\\crashpad_handler.exe";
    #elif defined(Q_OS_LINUX)
        return m_exeDir + "/crashpad/crashpad_handler";
    #else
        #error getHandlerPath not implemented on this platform
    #endif
}

QString Paths::getReportsPath() {
    #if defined(Q_OS_MAC)
        return m_exeDir + "/../../../crashpad";
    #elif defined(Q_OS_WINDOWS)
        return m_exeDir + "\\..\\crashpad";
    #elif defined(Q_OS_LINUX)
        return m_exeDir + "/crashpad";
    #else
        #error getReportsPath not implemented on this platform
    #endif
}

QString Paths::getMetricsPath() {
    #if defined(Q_OS_MAC)
        return m_exeDir + "/../../../crashpad";
    #elif defined(Q_OS_WINDOWS)
        return m_exeDir + "\\..\\crashpad";
    #elif defined(Q_OS_LINUX)
        return m_exeDir + "/crashpad";
    #else
        #error getMetricsPath not implemented on this platform
    #endif
}

#if defined(Q_OS_UNIX)
std::string Paths::getPlatformString(QString string){
    return string.toStdString();
}
#elif defined(Q_OS_WINDOWS)
std::wstring Paths::getPlatformString(QString string) {
    return string.toStdWString();
}
#else
    #error getPlatformString not implemented on this platform
#endif

using namespace base;
using namespace crashpad;

bool initializeCrashpad(QString dbName, QString appName, QString appVersion);
QString getExecutableDir(void);

#if defined(Q_OS_WIN)
    #define NOMINMAX
    #include <windows.h>
#endif

#if defined(Q_OS_MAC)
    #include <mach-o/dyld.h>
#endif

#if defined(Q_OS_LINUX)
    #include <unistd.h>
    #define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif

QString getExecutableDir() {
#if defined(Q_OS_MAC)
    unsigned int bufferSize = 512;
    std::vector<char> buffer(bufferSize + 1);

    if(_NSGetExecutablePath(&buffer[0], &bufferSize))
    {
        buffer.resize(bufferSize);
        _NSGetExecutablePath(&buffer[0], &bufferSize);
    }

    char* lastForwardSlash = strrchr(&buffer[0], '/');
    if (lastForwardSlash == NULL) return NULL;
    *lastForwardSlash = 0;

    return &buffer[0];
#elif defined(Q_OS_WINDOWS)
    HMODULE hModule = GetModuleHandleW(NULL);
    WCHAR path[MAX_PATH];
    DWORD retVal = GetModuleFileNameW(hModule, path, MAX_PATH);
    if (retVal == 0) return NULL;

    wchar_t *lastBackslash = wcsrchr(path, '\\');
    if (lastBackslash == NULL) return NULL;
    *lastBackslash = 0;

    return QString::fromWCharArray(path);
#elif defined(Q_OS_LINUX)
    char pBuf[FILENAME_MAX];
    int len = sizeof(pBuf);
    int bytes = MIN(readlink("/proc/self/exe", pBuf, len), len - 1);
    if (bytes >= 0) {
        pBuf[bytes] = '\0';
    }

    char* lastForwardSlash = strrchr(&pBuf[0], '/');
    if (lastForwardSlash == NULL) return NULL;
    *lastForwardSlash = '\0';

    return QString::fromStdString(pBuf);
#else
    #error getExecutableDir not implemented on this platform
#endif
}

bool initializeCrashpad(QString dbName, QString appName, QString appVersion)
{
    // Get directory where the exe lives so we can pass a full path to handler, reportsDir and metricsDir
    QString exeDir = getExecutableDir();

    // Helper class for cross-platform file systems
    Paths crashpadPaths(exeDir);

    // Ensure that crashpad_handler is shipped with your application
    FilePath handler(Paths::getPlatformString(crashpadPaths.getHandlerPath()));

    // Directory where reports will be saved. Important! Must be writable or crashpad_handler will crash.
    FilePath reportsDir(Paths::getPlatformString(crashpadPaths.getReportsPath()));

    // Directory where metrics will be saved. Important! Must be writable or crashpad_handler will crash.
    FilePath metricsDir(Paths::getPlatformString(crashpadPaths.getMetricsPath()));

    // Configure url with your BugSplat database
    QString url = "https://" + dbName + ".bugsplat.com/post/bp/crash/crashpad.php";

    // Metadata that will be posted to BugSplat
    QMap<std::string, std::string> annotations;
    annotations["format"] = "minidump";                 // Required: Crashpad setting to save crash as a minidump
    annotations["database"] = dbName.toStdString();     // Required: BugSplat database
    annotations["product"] = appName.toStdString();     // Required: BugSplat appName
    annotations["version"] = appVersion.toStdString();  // Required: BugSplat appVersion
    annotations["key"] = "Sample key";                  // Optional: BugSplat key field
    annotations["user"] = "fred@bugsplat.com";          // Optional: BugSplat user email
    annotations["list_annotations"] = "Sample comment";	// Optional: BugSplat crash description

    // Disable crashpad rate limiting so that all crashes have dmp files
    std::vector<std::string> arguments;
    arguments.push_back("--no-rate-limit");

    // Initialize crashpad database
    std::unique_ptr<CrashReportDatabase> database = CrashReportDatabase::Initialize(reportsDir);
    if (database == NULL) return false;

    // Enable automated crash uploads
    Settings *settings = database->GetSettings();
    if (settings == NULL) return false;
    settings->SetUploadsEnabled(true);

    // Attachments to be uploaded alongside the crash - default bundle size limit is 20MB
    std::vector<FilePath> attachments;
    FilePath attachment(Paths::getPlatformString(crashpadPaths.getAttachmentPath()));
#if defined(Q_OS_WINDOWS) || defined(Q_OS_LINUX)
    // Crashpad hasn't implemented attachments on OS X yet
    attachments.push_back(attachment);
#endif

    // Start crash handler
    CrashpadClient *client = new CrashpadClient();
    bool status = client->StartHandler(handler, reportsDir, metricsDir, url.toStdString(), annotations.toStdMap(), arguments, true, true, attachments);
    return status;
}
//end add for crashpad

int mainCLI(int argc, char *argv[]) {
  qInfo() << "Swiftray CLI interface";
  Canvas vcanvas;
  QFile file(argv[2]);
  Q_ASSERT_X(file.exists(), "Swiftray CLI", "File not found");
  Q_ASSERT_X(file.open(QFile::ReadOnly), "Swiftray CLI", "Can not open the file");
  QByteArray data = file.readAll();
  vcanvas.loadSVG(data);
  //vcanvas.exportGcode();
  return 0;
}

int main(int argc, char *argv[]) {

  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
  QApplication app(argc, argv);
  QCoreApplication::setOrganizationName("FLUX");
  QCoreApplication::setOrganizationDomain("flux3dp.com");
  QCoreApplication::setApplicationName("Swiftray");
  QCoreApplication::setApplicationVersion(QT_VERSION_STR);

  QString dbName = "Fred";
  QString appName = "swiftray";
  QString appVersion = "1.0";
  initializeCrashpad(dbName, appName, appVersion);
  *(volatile int *)0 = 0;

  // CLI
  if (argc > 1 && strcmp(argv[1], "cli") == 0) {
    return mainCLI(argc, argv);
  }

  // Set app icon
  app.setWindowIcon(QIcon(":/images/icon.png"));
  
  // Force anti-aliasing
  
  /*QSurfaceFormat format = QSurfaceFormat::defaultFormat();
  format.setSamples(8);
  QSurfaceFormat::setDefaultFormat(format);*/

  // Set translator
  QSettings settings;
  QString locale;
  QVariant language_code = settings.value("window/language", 0);

  switch(language_code.toInt()) {
    case 0:
      locale = "en-US";
      break;
    case 1:
      locale = "zh-Hant-TW";
      break;
    default:
      locale = "en-US";
      break;
  }

  QTranslator translator;
  translator.load(":/i18n/" + locale);
  app.installTranslator(&translator);

  // Load Canvas to QML Engine
  qmlRegisterType<Canvas>("Swiftray", 1, 0, "Canvas");
  
  // Load MainWindow
  MainWindow win;
  win.show();
#ifdef MACOS
  setOSXWindowTitleColor(&win);
#endif
  return app.exec();
}
