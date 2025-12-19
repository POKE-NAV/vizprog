#ifndef REPORTMANAGER_H
#define REPORTMANAGER_H

#include <QObject>
#include <QString>
#include <QDate>
#include <QList>
#include <QMap>
#include "rental.h"

class ReportManager : public QObject
{
    Q_OBJECT

public:
    enum ReportType {
        IncomeReport,       // Отчёт по доходам
        EquipmentLoadReport, // Отчёт по загруженности
        PopularityReport,    // Отчёт по популярности техники
        FullRentalReport     // Полный отчёт по арендам
    };

    explicit ReportManager(QObject *parent = nullptr);

    // Основные методы
    bool generateReport(ReportType type,
                        const QDate &startDate,
                        const QDate &endDate,
                        const QList<Rental*> &rentals);

    QString getLastError() const;
    QString getGeneratedReportPath() const;

    // Статические утилиты
    static QString getDesktopPath();
    static QString getReportTypeName(ReportType type);

    // Статистические методы
    static double calculateTotalIncome(const QList<Rental*> &rentals,
                                       const QDate &startDate,
                                       const QDate &endDate);

    static QMap<QString, int> calculateEquipmentLoad(const QList<Rental*> &rentals,
                                                     const QDate &startDate,
                                                     const QDate &endDate);

    static QMap<QString, int> calculatePopularity(const QList<Rental*> &rentals,
                                                  const QDate &startDate,
                                                  const QDate &endDate);

signals:
    void reportGenerated(const QString &filePath);
    void reportGenerationFailed(const QString &error);

private:
    bool saveReportToFile(const QString &content,
                          const QString &fileName);

    QString generateIncomeReport(const QDate &startDate,
                                 const QDate &endDate,
                                 const QList<Rental*> &rentals);

    QString generateEquipmentLoadReport(const QDate &startDate,
                                        const QDate &endDate,
                                        const QList<Rental*> &rentals);

    QString generatePopularityReport(const QDate &startDate,
                                     const QDate &endDate,
                                     const QList<Rental*> &rentals);

    QString generateFullRentalReport(const QDate &startDate,
                                     const QDate &endDate,
                                     const QList<Rental*> &rentals);

    QString formatReportHeader(ReportType type,
                               const QDate &startDate,
                               const QDate &endDate);

    QString formatRentalTable(const QList<Rental*> &rentals);

    QString m_lastError;
    QString m_lastFilePath;
};

#endif // REPORTMANAGER_H
