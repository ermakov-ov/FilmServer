
#ifndef TESTQT_MAIN_WINDOW_H
#define TESTQT_MAIN_WINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QTableWidget>
#include <QProgressBar>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QtNetwork>  // QNetworkAccessManager, QNetworkReply, QUrl, QUrlQuery
#include <vector>
#include "../common/common.h"
#include "load_config.h"

using FilmData = std::vector<common::FilmAnswer>  ;

struct StatData  {
    std::string service_name;
    std::string service_version;

    std::size_t total_films = 0;
    std::size_t total_actors = 0;
    std::size_t total_directors = 0;
    std::size_t total_genres = 0;
    std::size_t index_tokens_count = 0;
    double      avg_films_per_actor = 0.0;

    std::size_t pf_errors_total = 0;
    std::size_t pf_avg_response_time_ms = 0;
    std::size_t pf_requests_total = 0;
    std::size_t pf_active_workers = 0;
    std::size_t pf_queue_length = 0;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(const player_config::PlayerConfig &config, QWidget *parent = nullptr);

private:
    FilmData     m_filmData;
    StatData     m_statData;
    player_config::PlayerConfig m_playerConfig;
    // Layouts
    QVBoxLayout* filterLayout_ = nullptr;
    QVBoxLayout* playerLayout_ = nullptr;
    QVBoxLayout* mainLayout_ = nullptr;

    // Фильтры
    QLineEdit* searchInputTitle = nullptr;
    QLineEdit* searchInputActor = nullptr;
    QLineEdit* searchInputGenre = nullptr;
    QPushButton* btnSearch_ = nullptr;

    // Таблица результатов
    QTableWidget* resultsTable_ = nullptr;

    // Панель плеера
    QLabel* playerStatus_ = nullptr;

    QProcess *m_vlcProcess = nullptr;

    // Сетевой клиент
    QNetworkAccessManager* networkManager_ = nullptr;
    QNetworkReply *m_downloadVideoReply = nullptr;
    QNetworkReply *m_downloadTimerReply = nullptr;
    QNetworkReply *m_downloadSearchReply = nullptr;

    QFile *m_videoFile = nullptr;      // указатель, чтобы жить между вызовами
    QString m_videoSavePath;          // путь, чтобы потом передать в плеер

    QTimer *m_statusTimer = nullptr;

    void setupFilters();
    void setupResultsTable();
    void setupPlayerControls();


    QUrl buildSearchUrl() const;
    void loadInitialFilms();
    void fillTableFromJson(const QByteArray& jsonData);
    void getStatDataFromJson(const QByteArray& jsonData);
    std::string createHttpRequest(const std::string &name_service) const ;
    std::string createHttpSearchRequest() const ;
    std::string createHttpVideoRequest() const ;
    std::string createHttpStatRequest() const ;
    void onSearchFinished(QNetworkReply* reply);
    void onStatsFinished(QNetworkReply* reply);
    bool isVlcAvailable() const;

private slots:
    void onSearchClicked();
    void onRequestFinished(QNetworkReply* reply);

    void updateSearchButtonState();
    void doubleClickTableCell(int row, int column);
    void onDownloadFinished();
    void playLocalFile(const QString &path);
    void onVlcFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onVlcError(QProcess::ProcessError error);
    void showStatusServer();
    void updateTime();
};


#endif // TESTQT_MAIN_WINDOW_H
