#include "monitorthread.h"



MonitorThread::MonitorThread(QObject *parent)
  : QThread(parent)
{
  _stop_b = true;
}

MonitorThread::~MonitorThread()
{
  _quota_i = 0;
}

void MonitorThread::setDirectory(const QString &dir_str) {
  _dir_str = dir_str;
}

void MonitorThread::setQuota(size_t quotaBytes_i) {
  _quota_i = quotaBytes_i;
}

void MonitorThread::startWatch() {
  _stop_b = false;
}

void MonitorThread::stop() {
  _stop_b = true;
}

void MonitorThread::run() {
  while (!_stop_b) {
    if (_dir_str.isEmpty()) {
      QThread::sleep(1000); // Повторная проверка через 1 секунду (на пустоту директории)
      continue;
    }

    QList<FileInfo> files_lst;
    size_t totalSize_i = 0;

    // Сканируем директорию на файлы
    scanDirectory(_dir_str, files_lst, totalSize_i);

    if (totalSize_i > _quota_i) {
      // Сортируем по возрастанию (первый самый старый, последний самый новый)
      std::sort(files_lst.begin(), files_lst.end(), [](const FileInfo &FirstFile_o, const FileInfo &SecondFile_o) {
                  return FirstFile_o.lastModified_o < SecondFile_o.lastModified_o;
                });

      while (!files_lst.isEmpty() && totalSize_i > _quota_i) {
        const FileInfo oldest_o = files_lst.first();
        QFile file_o(oldest_o.filePath_str);
        if (file_o.remove()) {
          totalSize_i -= oldest_o.size_i;
          files_lst.removeFirst();
        } else {
          emit errorOccurred(QString("Failed to remove: %1").arg(oldest_o.filePath_str));
          files_lst.removeFirst();
        }
      }
      scanDirectory(_dir_str, files_lst, totalSize_i);
    }

    emit fileListUpdated(files_lst, totalSize_i);
    QThread::sleep(1000); //Следующая проверка через 1 секунду
  }
}

void MonitorThread::scanDirectory(const QString &dirPath_str, QList<FileInfo> &files_lst, size_t &totalSize_i) {
  QDir dir(dirPath_str);
  // Если директория не найдена
  if (!dir.exists()) {
    emit errorOccurred(QString("Directory not found: %1").arg(dirPath_str));
    return;
  }

  // Ищем все директории/файлы которые лежат в целевой директории, которую проверяем
  const QFileInfoList entries = dir.entryInfoList(
    QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs | QDir::Hidden | QDir::System
    );

  for (const QFileInfo &entry : entries) {
    // Если попали в директорию, то вызываем рекурсивно эту функцию на директорию
    if (entry.isDir()) {
      scanDirectory(entry.absoluteFilePath(), files_lst, totalSize_i);
    // Если это файл, записываем абсолютный путь до него, размер и время последнего редактирования
    } else if (entry.isFile()) {
      FileInfo fi_o;
      fi_o.filePath_str = entry.absoluteFilePath();
      fi_o.size_i = entry.size();
      fi_o.lastModified_o = entry.lastModified();
      files_lst.append(fi_o); // Закидываем в список к остальным найденным файлам
      totalSize_i += fi_o.size_i; // Считаеи общий размер
    }
  }
}
