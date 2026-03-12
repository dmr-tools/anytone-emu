#include "spellchecker.hh"
#include <QStandardPaths>
#include <hunspell.hxx>
#include <QJsonDocument>
#include <QJsonArray>
#include <QSettings>
#include <QFile>
#include <QDir>


SpellChecker::SpellChecker(QObject *parent)
  : QObject{parent}, _spellChecker(nullptr)
{
  auto affPath = QStandardPaths::locate(QStandardPaths::GenericDataLocation, "hunspell/en_US.aff");
  auto dicPath = QStandardPaths::locate(QStandardPaths::GenericDataLocation, "hunspell/en_US.dic");
  _spellChecker = new Hunspell(affPath.toLocal8Bit().constData(), dicPath.toLocal8Bit().constData());
  // Load static dictionary
  QFile staticDict(":/dictionary.json");
  if (staticDict.open(QIODevice::ReadOnly)) {
    auto doc = QJsonDocument::fromJson(staticDict.readAll());
    staticDict.close();
    if (doc.isArray()) {
      for (auto elm: doc.array()) {
        if (elm.isString())
          _spellChecker->add(elm.toString().toStdString());
      }
    }
  }

  // Load user dictionary
  QDir dataDir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
  if (! dataDir.exists())
    QDir("/").mkpath(dataDir.absolutePath());
  _userDictionaryFile.setFileName(dataDir.filePath("dictionary.json"));
  if (_userDictionaryFile.open(QIODevice::ReadOnly)) {
    auto doc = QJsonDocument::fromJson(_userDictionaryFile.readAll());
    _userDictionaryFile.close();
    if (doc.isArray()) {
      for (auto elm: doc.array()) {
        if (elm.isString())
          _spellChecker->add(elm.toString().toStdString());
      }
    }
  }
}

SpellChecker::~SpellChecker() {
  if (nullptr != _spellChecker)
    delete _spellChecker;
  if (_userDictionaryFile.isOpen())
    _userDictionaryFile.close();
}


bool
SpellChecker::check(const QString &word) const {
  if (! _spellChecker)
    return true;
  return _spellChecker->spell(word.toStdString());
}

QStringList
SpellChecker::suggest(const QString &word) const {
  QStringList suggestsions;
  if (check(word))
    return suggestsions;
  std::vector<std::string> res = _spellChecker->suggest(word.toStdString());
  for (auto w: res) {
    suggestsions.append(QString::fromStdString(w));
  }
  return suggestsions;
}


void
SpellChecker::addWord(const QString &word) {
  if (! _spellChecker)
    return;
  _spellChecker->add(word.toStdString());
  _userDictionary.append(word);
  if (_userDictionaryFile.open(QIODevice::WriteOnly)) {
    _userDictionaryFile.write(QJsonDocument(_userDictionary).toJson());
    _userDictionaryFile.close();
  } else {
    qDebug() << _userDictionaryFile.errorString();
  }
}
