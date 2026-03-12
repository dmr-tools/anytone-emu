#ifndef SPELLCHECKER_HH
#define SPELLCHECKER_HH

#include <QObject>
#include <QFile>
#include <QJsonArray>

class Hunspell;


class SpellChecker : public QObject
{
  Q_OBJECT

public:
  explicit SpellChecker(QObject *parent = nullptr);
  virtual ~SpellChecker();

public slots:
  bool check(const QString &word) const;
  QStringList suggest(const QString &word) const;
  void addWord(const QString &word);

protected:
  Hunspell *_spellChecker;
  QFile _userDictionaryFile;
  QJsonArray _userDictionary;
};

#endif // SPELLCHECKER_HH
