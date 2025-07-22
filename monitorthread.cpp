#include "monitorthread.h"
#include "QDebug"


MonitorThread::MonitorThread(QObject *parent)
  : QThread(parent)
{
  qRegisterMetaType <QList<FileInfo>> ("QList<FileInfo>");
  qRegisterMetaType <size_t> ("size_t");
  qRegisterMetaType <QString> ("QString");

  _stop_b = false;
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
  // _stop_b = false;
  // qDebug() << "start";
}

void MonitorThread::stopWatch() {
  _stop_b = true;
  qDebug() << "stop";
}

void MonitorThread::run() {
  qDebug() << "run";
  while (!_stop_b) {
    if (_dir_str.isEmpty()) {
      QThread::sleep(1); // Повторная проверка через 1 секунду (на пустоту директории)
      qDebug() << "Dir is empty";
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

      // Если список файлов не пустой и общий размер превышает квоту, то запускаем очистку
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
    QThread::sleep(1); //Следующая проверка через 1 секунду

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
      qDebug() << fi_o.filePath_str << fi_o.size_i << fi_o.lastModified_o << totalSize_i;

    }
  }
}
