#ifndef MONITORTHREAD_H
#define MONITORTHREAD_H

#pragma once

#include <QThread>
#include <QFileInfo>
#include <QDir>
#include <QDateTime>
#include <algorithm>



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



class MonitorThread : public QThread
{
  Q_OBJECT

  public:
  explicit MonitorThread( QObject *parent = nullptr);
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
   * @brief Запуск обработчика новых файлов
   */
  void startWatch();
  /**
   * @brief Остановка обработчика новых файлов
   */
  void stop();

  protected:
  void run() override;  ///< @brief Переопределённый метод по запуску потока

  private:
  QString _dir_str;    ///< @brief Путь до целевой директории
  size_t _quota_i;     ///< @brief Максимальный размер директории в MB (МегаБайтах)
  bool _stop_b;       ///< @brief Абсолютный путь до файла

  void scanDirectory(const QString &dirPath_str, QList<FileInfo> &files_lst, size_t &totalSize_i);

  signals:
  void errorOccurred(const QString &error_str);
  void fileListUpdated(QList<FileInfo> &files_lst, size_t totalSize_i);
};

#endif // MONITORTHREAD_H
