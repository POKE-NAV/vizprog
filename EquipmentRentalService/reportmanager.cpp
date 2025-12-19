#include "reportmanager.h"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDesktopServices>
#include <QStandardPaths>
#include <QDateTime>
#include <QMessageBox>
#include <algorithm>

static QString repeatString(const QString &str, int count) {
    QString result;
    for (int i = 0; i < count; ++i) {
        result += str;
    }
    return result;
}

ReportManager::ReportManager(QObject *parent)
    : QObject(parent)
{
}

bool ReportManager::generateReport(ReportType type,
                                   const QDate &startDate,
                                   const QDate &endDate,
                                   const QList<Rental*> &rentals)
{
    m_lastError.clear();
    m_lastFilePath.clear();

    // Валидация дат
    if (!startDate.isValid() || !endDate.isValid()) {
        m_lastError = "Невалидные даты для отчёта";
        emit reportGenerationFailed(m_lastError);
        return false;
    }

    if (startDate > endDate) {
        m_lastError = "Дата начала не может быть позже даты окончания";
        emit reportGenerationFailed(m_lastError);
        return false;
    }

    // Фильтрация аренд по дате
    QList<Rental*> filteredRentals;
    for (Rental* rental : rentals) {
        if (rental &&
            rental->getEndDate() >= startDate &&
            rental->getStartDate() <= endDate) {
            filteredRentals.append(rental);
        }
    }

    if (filteredRentals.isEmpty()) {
        m_lastError = "Нет данных для отчёта за указанный период";
        emit reportGenerationFailed(m_lastError);
        return false;
    }

    // Генерация содержимого отчёта
    QString reportContent;

    switch (type) {
    case IncomeReport:
        reportContent = generateIncomeReport(startDate, endDate, filteredRentals);
        break;
    case EquipmentLoadReport:
        reportContent = generateEquipmentLoadReport(startDate, endDate, filteredRentals);
        break;
    case PopularityReport:
        reportContent = generatePopularityReport(startDate, endDate, filteredRentals);
        break;
    case FullRentalReport:
        reportContent = generateFullRentalReport(startDate, endDate, filteredRentals);
        break;
    default:
        m_lastError = "Неизвестный тип отчёта";
        emit reportGenerationFailed(m_lastError);
        return false;
    }

    // Создание имени файла
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    QString fileName = QString("%1_%2_%3.txt")
                           .arg(getReportTypeName(type))
                           .arg(startDate.toString("yyyyMMdd"))
                           .arg(endDate.toString("yyyyMMdd"));

    // Сохранение файла
    if (saveReportToFile(reportContent, fileName)) {
        emit reportGenerated(m_lastFilePath);
        return true;
    }

    emit reportGenerationFailed(m_lastError);
    return false;
}

QString ReportManager::generateIncomeReport(const QDate &startDate,
                                            const QDate &endDate,
                                            const QList<Rental*> &rentals)
{
    QString report = formatReportHeader(IncomeReport, startDate, endDate);

    double totalIncome = 0;
    int completedCount = 0;
    int activeCount = 0;

    QTextStream stream(&report);
    stream << "\n";
    stream << repeatString("_", 80) << "\n";
    stream << "ДЕТАЛЬНАЯ ИНФОРМАЦИЯ ПО ДОХОДАМ:\n";
    stream << repeatString("_", 80) << "\n\n";

    stream << QString("%1 %2 %3 %4 %5 %6\n")
                  .arg("ID", 6)
                  .arg("Клиент", 20)
                  .arg("Оборудование", 25)
                  .arg("Период", 20)
                  .arg("Сумма", 10)
                  .arg("Статус", 15);
    stream << repeatString("-", 80) << "\n";

    for (Rental* rental : rentals) {
        if (!rental) continue;

        QString period = QString("%1 - %2")
                             .arg(rental->getStartDate().toString("dd.MM"))
                             .arg(rental->getEndDate().toString("dd.MM.yy"));

        stream << QString("%1 %2 %3 %4 %5 %6\n")
                      .arg(QString::number(rental->getId()), 6)
                      .arg(rental->getClientLogin().left(18), 20)
                      .arg(rental->getEquipmentName().left(23), 25)
                      .arg(period, 20)
                      .arg(QString::number(rental->getTotalPrice(), 'f', 2) + " ₽", 10)
                      .arg(rental->getStatus(), 15);

        totalIncome += rental->getTotalPrice();

        if (rental->isCompleted()) completedCount++;
        if (rental->isActive()) activeCount++;
    }

    stream << "\n" << repeatString("_", 80) << "\n";
    stream << "СТАТИСТИКА:\n";
    stream << repeatString("_", 80) << "\n\n";

    stream << QString("Всего аренд за период: %1\n").arg(rentals.count());
    stream << QString("Завершённых аренд: %1\n").arg(completedCount);
    stream << QString("Активных аренд: %1\n").arg(activeCount);
    stream << QString("Общий доход: %1 ₽\n").arg(totalIncome, 0, 'f', 2);
    stream << QString("Средний доход с аренды: %1 ₽\n")
                  .arg(rentals.count() > 0 ? totalIncome / rentals.count() : 0, 0, 'f', 2);

    stream << "\n" << repeatString("_", 80) << "\n";
    stream << "СВОДКА ПО МЕСЯЦАМ:\n";
    stream << repeatString("_", 80) << "\n\n";

    // Группировка по месяцам
    QMap<QString, double> monthlyIncome;
    for (Rental* rental : rentals) {
        QString monthKey = rental->getStartDate().toString("yyyy-MM");
        monthlyIncome[monthKey] += rental->getTotalPrice();
    }

    for (auto it = monthlyIncome.begin(); it != monthlyIncome.end(); ++it) {
        QDate monthDate = QDate::fromString(it.key() + "-01", "yyyy-MM-dd");
        stream << QString("%1: %2 ₽\n")
                      .arg(monthDate.toString("MMMM yyyy"))
                      .arg(it.value(), 0, 'f', 2);
    }

    stream << "\nОтчёт сформирован: "
           << QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss") << "\n";

    return report;
}

QString ReportManager::generateEquipmentLoadReport(const QDate &startDate,
                                                   const QDate &endDate,
                                                   const QList<Rental*> &rentals)
{
    QString report = formatReportHeader(EquipmentLoadReport, startDate, endDate);

    int totalDays = startDate.daysTo(endDate) + 1;
    QTextStream stream(&report);

    stream << "\n" << repeatString("_", 80) << "\n";
    stream << "ЗАГРУЖЕННОСТЬ ОБОРУДОВАНИЯ:\n";
    stream << repeatString("_", 80) << "\n\n";

    // Группировка по оборудованию
    QMap<QString, int> equipmentDays;
    QMap<QString, int> equipmentRentals;

    for (Rental* rental : rentals) {
        if (!rental) continue;

        QString equipment = rental->getEquipmentName();
        int rentalDays = rental->getRentalDays();

        equipmentDays[equipment] += rentalDays;
        equipmentRentals[equipment]++;
    }

    stream << QString("%1 %2 %3 %4 %5\n")
                  .arg("Оборудование", 30)
                  .arg("Дней аренды", 12)
                  .arg("Кол-во аренд", 12)
                  .arg("Загруженность %", 15)
                  .arg("Сред.длит.", 10);
    stream << repeatString("-", 80) << "\n";

    // Сортировка по загруженности
    QList<QPair<QString, int>> sortedEquipment;
    for (auto it = equipmentDays.begin(); it != equipmentDays.end(); ++it) {
        sortedEquipment.append(qMakePair(it.key(), it.value()));
    }

    std::sort(sortedEquipment.begin(), sortedEquipment.end(),
              [](const QPair<QString, int> &a, const QPair<QString, int> &b) {
                  return a.second > b.second;
              });

    for (const auto &pair : sortedEquipment) {
        QString equipment = pair.first;
        int days = pair.second;
        int rentalCount = equipmentRentals[equipment];
        double loadPercentage = (days * 100.0) / totalDays;
        double avgDuration = rentalCount > 0 ? (double)days / rentalCount : 0;

        stream << QString("%1 %2 %3 %4 %5\n")
                      .arg(equipment.left(28), 30)
                      .arg(days, 12)
                      .arg(rentalCount, 12)
                      .arg(QString::number(loadPercentage, 'f', 1) + "%", 15)
                      .arg(QString::number(avgDuration, 'f', 1), 10);
    }

    stream << "\n" << repeatString("_", 80) << "\n";
    stream << "АНАЛИЗ ЗАГРУЖЕННОСТИ:\n";
    stream << repeatString("_", 80) << "\n\n";

    stream << QString("Всего дней в периоде: %1\n").arg(totalDays);
    stream << QString("Всего уникального оборудования: %1\n").arg(sortedEquipment.size());

    // Анализ распределения
    int highLoadCount = 0, mediumLoadCount = 0, lowLoadCount = 0;
    for (const auto &pair : sortedEquipment) {
        int days = pair.second;
        double percentage = (days * 100.0) / totalDays;

        if (percentage > 50) highLoadCount++;
        else if (percentage > 20) mediumLoadCount++;
        else lowLoadCount++;
    }

    stream << QString("\nВысокая загруженность (>50%%): %1 единиц\n").arg(highLoadCount);
    stream << QString("Средняя загруженность (20-50%%): %1 единиц\n").arg(mediumLoadCount);
    stream << QString("Низкая загруженность (<20%%): %1 единиц\n").arg(lowLoadCount);

    // Рекомендации
    stream << "\n" << repeatString("_", 80) << "\n";
    stream << "РЕКОМЕНДАЦИИ:\n";
    stream << repeatString("_", 80) << "\n\n";

    if (highLoadCount > 0) {
        stream << "✓ Оборудование с высокой загруженностью может требовать\n";
        stream << "  дополнительных экземпляров или технического обслуживания.\n";
    }

    if (lowLoadCount > mediumLoadCount) {
        stream << "✓ Много оборудования простаивает - рассмотрите возможность\n";
        stream << "  списания или сдачи в субаренду.\n";
    }

    stream << "\nОтчёт сформирован: "
           << QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss") << "\n";

    return report;
}

QString ReportManager::generatePopularityReport(const QDate &startDate,
                                                const QDate &endDate,
                                                const QList<Rental*> &rentals)
{
    QString report = formatReportHeader(PopularityReport, startDate, endDate);

    QTextStream stream(&report);

    stream << "\n" << repeatString("=", 80) << "\n";
    stream << "РЕЙТИНГ ПОПУЛЯРНОСТИ ОБОРУДОВАНИЯ:\n";
    stream << repeatString("_", 80) << "\n\n";

    // Группировка по оборудованию
    QMap<QString, int> rentalCount;
    QMap<QString, double> totalIncome;

    for (Rental* rental : rentals) {
        if (!rental) continue;

        QString equipment = rental->getEquipmentName();
        rentalCount[equipment]++;
        totalIncome[equipment] += rental->getTotalPrice();
    }

    // Сортировка по популярности
    QList<QPair<QString, int>> sortedByCount;
    for (auto it = rentalCount.begin(); it != rentalCount.end(); ++it) {
        sortedByCount.append(qMakePair(it.key(), it.value()));
    }

    std::sort(sortedByCount.begin(), sortedByCount.end(),
              [](const QPair<QString, int> &a, const QPair<QString, int> &b) {
                  return a.second > b.second;
              });

    stream << QString("%1 %2 %3 %4 %5\n")
                  .arg("№", 3)
                  .arg("Оборудование", 30)
                  .arg("Кол-во аренд", 12)
                  .arg("Доход, ₽", 15)
                  .arg("Доля рынка %", 15);
    stream << repeatString("-", 80) << "\n";

    int totalRentals = rentals.count();
    int rank = 1;

    for (const auto &pair : sortedByCount) {
        QString equipment = pair.first;
        int count = pair.second;
        double income = totalIncome[equipment];
        double marketShare = totalRentals > 0 ? (count * 100.0) / totalRentals : 0;

        stream << QString("%1 %2 %3 %4 %5\n")
                      .arg(rank, 3)
                      .arg(equipment.left(28), 30)
                      .arg(count, 12)
                      .arg(QString::number(income, 'f', 2), 15)
                      .arg(QString::number(marketShare, 'f', 1) + "%", 15);
        rank++;
    }

    // Топ-5 оборудования
    stream << "\n" << repeatString("_", 80) << "\n";
    stream << "ТОП-5 САМОГО ПОПУЛЯРНОГО ОБОРУДОВАНИЯ:\n";
    stream << repeatString("_", 80) << "\n\n";

    for (int i = 0; i < qMin(5, sortedByCount.size()); i++) {
        stream << QString("%1. %2 - %3 аренд\n")
                      .arg(i + 1)
                      .arg(sortedByCount[i].first)
                      .arg(sortedByCount[i].second);
    }

    // Анализ
    stream << "\n" << repeatString("_", 80) << "\n";
    stream << "АНАЛИЗ ПОПУЛЯРНОСТИ:\n";
    stream << repeatString("_", 80) << "\n\n";

    if (!sortedByCount.isEmpty()) {
        QString mostPopular = sortedByCount.first().first;
        int mostRentals = sortedByCount.first().second;

        stream << QString("Самое популярное оборудование: %1 (%2 аренд)\n")
                      .arg(mostPopular)
                      .arg(mostRentals);

        if (sortedByCount.size() > 1) {
            QString secondPopular = sortedByCount[1].first;
            int secondRentals = sortedByCount[1].second;
            double difference = mostRentals - secondRentals;

            stream << QString("Отрыв от второго места: %1 аренд\n").arg(difference);
        }
    }

    // Рекомендации по закупкам
    stream << "\n" << repeatString("_", 80) << "\n";
    stream << "РЕКОМЕНДАЦИИ ПО ЗАКУПКАМ:\n";
    stream << repeatString("_", 80) << "\n\n";

    stream << "✓ Рассмотреть закупку дополнительных единиц\n";
    stream << "  самого популярного оборудования.\n";
    stream << "✓ Проанализировать причины низкой популярности\n";
    stream << "  некоторых позиций оборудования.\n";

    stream << "\nОтчёт сформирован: "
           << QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss") << "\n";

    return report;
}

QString ReportManager::generateFullRentalReport(const QDate &startDate,
                                                const QDate &endDate,
                                                const QList<Rental*> &rentals)
{
    QString report = formatReportHeader(FullRentalReport, startDate, endDate);

    QTextStream stream(&report);

    // Сводная статистика
    int totalRentals = rentals.count();
    double totalIncome = 0;
    int totalDays = 0;
    QMap<QString, int> statusCount;

    for (Rental* rental : rentals) {
        if (!rental) continue;

        totalIncome += rental->getTotalPrice();
        totalDays += rental->getRentalDays();
        statusCount[rental->getStatus()]++;
    }

    stream << "\n" << repeatString("_", 80) << "\n";
    stream << "СВОДНАЯ СТАТИСТИКА:\n";
    stream << repeatString("_", 80) << "\n\n";

    stream << QString("Период отчёта: с %1 по %2\n")
                  .arg(startDate.toString("dd.MM.yyyy"))
                  .arg(endDate.toString("dd.MM.yyyy"));
    stream << QString("Всего аренд: %1\n").arg(totalRentals);
    stream << QString("Общий доход: %1 ₽\n").arg(totalIncome, 0, 'f', 2);
    stream << QString("Общее количество дней аренды: %1\n").arg(totalDays);
    stream << QString("Средняя стоимость аренды: %1 ₽\n")
                  .arg(totalRentals > 0 ? totalIncome / totalRentals : 0, 0, 'f', 2);

    stream << "\nРаспределение по статусам:\n";
    for (auto it = statusCount.begin(); it != statusCount.end(); ++it) {
        double percentage = (it.value() * 100.0) / totalRentals;
        stream << QString("  %1: %2 (%3%)\n")
                      .arg(it.key())
                      .arg(it.value())
                      .arg(percentage, 0, 'f', 1);
    }

    // Детальная таблица
    stream << "\n" << repeatString("_", 80) << "\n";
    stream << "ДЕТАЛЬНЫЙ СПИСОК АРЕНД:\n";
    stream << repeatString("_", 80) << "\n\n";

    stream << QString("%1 %2 %3 %4 %5 %6 %7\n")
                  .arg("ID", 6)
                  .arg("Клиент", 15)
                  .arg("Оборудование", 20)
                  .arg("Дата нач.", 12)
                  .arg("Дата окон.", 12)
                  .arg("Сумма, ₽", 10)
                  .arg("Статус", 12);
    stream << repeatString("-", 80) << "\n";

    for (Rental* rental : rentals) {
        stream << QString("%1 %2 %3 %4 %5 %6 %7\n")
        .arg(QString::number(rental->getId()), 6)
            .arg(rental->getClientLogin().left(13), 15)
            .arg(rental->getEquipmentName().left(18), 20)
            .arg(rental->getStartDate().toString("dd.MM.yy"), 12)
            .arg(rental->getEndDate().toString("dd.MM.yy"), 12)
            .arg(QString::number(rental->getTotalPrice(), 'f', 2), 10)
            .arg(rental->getStatus().left(10), 12);
    }

    // Аналитические выводы
    stream << "\n" << repeatString("_", 80) << "\n";
    stream << "АНАЛИТИЧЕСКИЕ ВЫВОДЫ:\n";
    stream << repeatString("_", 80) << "\n\n";

    if (totalRentals == 0) {
        stream << "За указанный период не было совершено аренд.\n";
    } else {
        double avgRentalDays = (double)totalDays / totalRentals;
        stream << QString("Средняя продолжительность аренды: %1 дней\n").arg(avgRentalDays, 0, 'f', 1);

        if (statusCount.contains("Активный") && statusCount["Активный"] > totalRentals * 0.3) {
            stream << "✓ Высокий процент активных аренд - хороший показатель текущей загруженности\n";
        }

        if (statusCount.contains("Завершенный") && statusCount["Завершенный"] > totalRentals * 0.5) {
            stream << "✓ Большинство аренд успешно завершены - хороший показатель обслуживания\n";
        }
    }

    stream << "\nОтчёт сформирован: "
           << QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss") << "\n";

    return report;
}

QString ReportManager::formatReportHeader(ReportType type,
                                          const QDate &startDate,
                                          const QDate &endDate)
{
    QString header;
    QTextStream stream(&header);

    stream << repeatString("_", 80) << "\n";
    stream << "ОТЧЁТ СИСТЕМЫ ПРОКАТА ОБОРУДОВАНИЯ\n";
    stream << repeatString("_", 80) << "\n\n";

    stream << QString("Тип отчёта: %1\n").arg(getReportTypeName(type));
    stream << QString("Период: с %1 по %2\n")
                  .arg(startDate.toString("dd.MM.yyyy"))
                  .arg(endDate.toString("dd.MM.yyyy"));
    stream << QString("Дата формирования: %1\n")
                  .arg(QDateTime::currentDateTime().toString("dd.MM.yyyy"));
    stream << "\n" << repeatString("_", 80) << "\n\n";

    return header;
}

bool ReportManager::saveReportToFile(const QString &content, const QString &fileName)
{
    QString desktopPath = getDesktopPath();
    if (desktopPath.isEmpty()) {
        m_lastError = "Не удалось определить путь к рабочему столу";
        return false;
    }

    QString fullPath = QDir(desktopPath).filePath(fileName);

    QFile file(fullPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_lastError = QString("Не удалось создать файл: %1").arg(file.errorString());
        return false;
    }

    QTextStream out(&file);

    out.setEncoding(QStringConverter::Utf8);

    out << content;
    file.close();

    m_lastFilePath = fullPath;
    return true;
}

QString ReportManager::getDesktopPath()
{
    return QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
}

QString ReportManager::getReportTypeName(ReportType type)
{
    switch (type) {
    case IncomeReport: return "Отчет_по_доходам";
    case EquipmentLoadReport: return "Отчет_по_загруженности";
    case PopularityReport: return "Отчет_по_популярности";
    case FullRentalReport: return "Полный_отчет_по_арендам";
    default: return "Отчет";
    }
}

QString ReportManager::getLastError() const
{
    return m_lastError;
}

QString ReportManager::getGeneratedReportPath() const
{
    return m_lastFilePath;
}

double ReportManager::calculateTotalIncome(const QList<Rental*> &rentals,
                                           const QDate &startDate,
                                           const QDate &endDate)
{
    double total = 0;
    for (Rental* rental : rentals) {
        if (rental &&
            rental->getStartDate() >= startDate &&
            rental->getEndDate() <= endDate) {
            total += rental->getTotalPrice();
        }
    }
    return total;
}

QMap<QString, int> ReportManager::calculateEquipmentLoad(const QList<Rental*> &rentals,
                                                         const QDate &startDate,
                                                         const QDate &endDate)
{
    QMap<QString, int> result;
    for (Rental* rental : rentals) {
        if (rental &&
            rental->getStartDate() >= startDate &&
            rental->getEndDate() <= endDate) {
            result[rental->getEquipmentName()] += rental->getRentalDays();
        }
    }
    return result;
}

QMap<QString, int> ReportManager::calculatePopularity(const QList<Rental*> &rentals,
                                                      const QDate &startDate,
                                                      const QDate &endDate)
{
    QMap<QString, int> result;
    for (Rental* rental : rentals) {
        if (rental &&
            rental->getStartDate() >= startDate &&
            rental->getEndDate() <= endDate) {
            result[rental->getEquipmentName()]++;
        }
    }
    return result;
}
