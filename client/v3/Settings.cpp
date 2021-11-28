#include "Settings.hpp"

#include <QDir>
#include <QStandardPaths>
#include <iostream>

QSettings *Settings::settings = nullptr;

Settings::Settings() {
#ifdef __ANDROID_API__
/*if(QSettings().fileName().startsWith("/.config"))
{
    QSettings::setPath(QSettings::NativeFormat, QSettings::UserScope,
QDir::homePath() + "/.config");
}*/
#endif
}

void Settings::init() {
#ifdef __ANDROID_API__
  #ifdef Q_OS_WIN32
  Settings::settings = new QSettings();
  #else
    #ifdef Q_OS_LINUX
  /*const QString
  settingsPath=QStandardPaths::standardLocations(QStandardPaths::DataLocation).first()+"/config.ini";
  std::cout << settingsPath.toStdString() << std::endl;
  Settings::settings=new QSettings(settingsPath,QSettings::NativeFormat);*/
  // use default for now:
  Settings::settings = new QSettings();
    #else
  Settings::settings = new QSettings();
    #endif
  #endif
#else
  Settings::settings = new QSettings();
#endif
}
