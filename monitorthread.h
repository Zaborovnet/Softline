#ifndef MONITORTHREAD_H
#define MONITORTHREAD_H

#pragma once

#include <QThread>
#include <QFileInfo>
#include <QDir>
#include <QDateTime>
#include <algorithm>
#include <QMetaType>


/** @brief Декларирование типов для передачи */
Q_DECLARE_METATYPE(QList<QFileInfo>);
Q_DECLARE_METATYPE(size_t);
Q_DECLARE_METATYPE(QString);


/**
 * @struct FileInfo
 * @brief Структура для хранения информации о файлах
 */
struct FileInfo
{
  QString filePath_str;     ///< @brief Абсолютный путь до файла
  size_t size_i;            ///< @brief Размер файла в байтах
  QDateTime lastModified_o; ///< @brief Время последней модификации
};

/**
  @brief Класс-мониторинг за файлами/директориями
*/
class MonitorThread : public QThread
{
  Q_OBJECT

  public:
  /**
   * @brief Конструктор
   */
  explicit MonitorThread( QObject *parent = nullptr);

  /**
   * @brief Деструктор
   */
  ~MonitorThread();

  /**
   * @brief Устанавливает путь до директории
   * @param[in] dir_str Путь до директории
   */
  void setDirectory(const QString &dir_str);
  /**
   * @brief Устанавливает квоту на размер директории
   * @param[in] quotaBytes_i Размер квоты в байтах
   */
  void setQuota(size_t quotaBytes_i);
  /**
   * @brief Запуск обработчика файлов
   */
  void startWatch();
  /**
   * @brief Остановка обработчика файлов
   */
  void stopWatch();

  protected:
  /**
   * @brief Переопределённый метод по запуску обропотчика в потоке
   */
  void run() override;

  private:
  QString _dir_str;    ///< @brief Путь до целевой директории
  size_t _quota_i;     ///< @brief Максимальный размер директории в MB (МегаБайтах)
  bool _stop_b;        ///< @brief Абсолютный путь до файла

  /** @brief Сканирование директории
   * @param[in] dirPath_str Абсолютный путь до листа с файлами
   * @param[in] files_lst Лист с файлами
   * @param[in] totalSize_i Общий объём всех файлов
   */
  void scanDirectory(const QString &dirPath_str, QList<FileInfo> &files_lst, size_t &totalSize_i);

  signals:
  /** @brief В случае ошибки отсылаем, что за ошибка*/
  void errorOccurred(const QString &error_str);    
  /** @brief Для отсыла листа со всеми файлами в целевой директории, а так же общего размера всех директории*/
  void fileListUpdated(const QList<FileInfo> &files_lst, size_t totalSize_i);
};

#endif // MONITORTHREAD_H
