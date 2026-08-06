#include "main_window.h"

#include <QDebug>
#include <QMessageBox>

#include "../json/parser_json.h"
#include "../common/logger.h"


namespace fs = std::filesystem;

MainWindow::MainWindow(const player_config::PlayerConfig &config, QWidget *parent)
:m_playerConfig(config)
,QMainWindow(parent)
{
    auto centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    mainLayout_ = new QVBoxLayout(centralWidget);

    setupFilters();
    setupResultsTable();
    setupPlayerControls();

    mainLayout_->addLayout(filterLayout_);
    mainLayout_->addWidget(resultsTable_);
    mainLayout_->addLayout(playerLayout_);

    networkManager_ = new QNetworkAccessManager(this);

    connect(networkManager_, &QNetworkAccessManager::finished, this, &MainWindow::onRequestFinished);
    connect(btnSearch_, &QPushButton::clicked, this, &MainWindow::onSearchClicked);

    m_statusTimer = new QTimer(this);
    connect(m_statusTimer, &QTimer::timeout, this, &MainWindow::updateTime);
    m_statusTimer->start(30000);

    updateTime();
}

void MainWindow::updateTime()
{
    QUrl url(QString::fromStdString(createHttpStatRequest()));

    if ( m_downloadTimerReply == nullptr ) {
        m_downloadTimerReply = networkManager_->get(QNetworkRequest(url));
    }
}

void MainWindow::setupFilters()
{
    filterLayout_ = new QVBoxLayout();

    QHBoxLayout *filterTitle_ = new QHBoxLayout();
    searchInputTitle = new QLineEdit();
    searchInputTitle->setMinimumWidth(200);
    searchInputTitle->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    searchInputTitle->setPlaceholderText("The title (or part of the title) of the movie");
    filterTitle_->addWidget(searchInputTitle) ;

    QHBoxLayout *filterActor_ = new QHBoxLayout();
    searchInputActor = new QLineEdit();
    searchInputActor->setMinimumWidth(200) ;
    searchInputActor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    searchInputActor->setPlaceholderText("The actor's name (or part of the name)");
    filterActor_->addWidget(searchInputActor) ;

    QHBoxLayout *filterGenre_ = new QHBoxLayout();
    searchInputGenre = new QLineEdit();
    searchInputGenre->setMinimumWidth(200);
    searchInputGenre->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    searchInputGenre->setPlaceholderText("The ganre's name (or part of the name)");
    filterGenre_->addWidget(searchInputGenre) ;

    auto label = new QLabel("Запрос:");

    btnSearch_ = new QPushButton("Найти");

    filterLayout_->addWidget(label);
    filterLayout_->addLayout(filterTitle_);
    filterLayout_->addLayout(filterActor_);
    filterLayout_->addLayout(filterGenre_);

    filterLayout_->addWidget(btnSearch_);

    btnSearch_->setEnabled(false);
    btnSearch_->setDefault(true);
    btnSearch_->setAutoDefault(true);

    connect(searchInputTitle, &QLineEdit::textChanged, this, &MainWindow::updateSearchButtonState);
    connect(searchInputActor,  &QLineEdit::textChanged, this, &MainWindow::updateSearchButtonState);
    connect(searchInputGenre, &QLineEdit::textChanged, this, &MainWindow::updateSearchButtonState);

    auto triggerSearchIfEnabled = [this]() {
        if (btnSearch_->isEnabled()) {
            btnSearch_->click();
        }
    };
    connect(searchInputTitle, &QLineEdit::returnPressed, triggerSearchIfEnabled);
    connect(searchInputActor, &QLineEdit::returnPressed, triggerSearchIfEnabled);
    connect(searchInputGenre, &QLineEdit::returnPressed, triggerSearchIfEnabled);

}

void MainWindow::updateSearchButtonState()
{
    bool hasInput =
        !searchInputTitle->text().isEmpty() ||
        !searchInputActor->text().isEmpty()  ||
        !searchInputGenre->text().isEmpty() ;

    btnSearch_->setEnabled(hasInput);
}

void MainWindow::setupResultsTable()
{
    resultsTable_ = new QTableWidget();
    resultsTable_->setColumnCount(6);
    resultsTable_->setHorizontalHeaderLabels({
        "Name", "Year", "Director", "Actors","Genre",  "Status"
    });
    resultsTable_->horizontalHeader()->setStretchLastSection(true);
    resultsTable_->setSelectionBehavior(QAbstractItemView::SelectRows);

    connect(resultsTable_, &QTableWidget::cellDoubleClicked, this, &MainWindow::doubleClickTableCell);
}

void MainWindow::doubleClickTableCell(int row, int column)
{
    QUrl url(QString::fromStdString(createHttpVideoRequest()));
    QUrlQuery query;
    query.addQueryItem("by_number", QString::number(m_filmData[row].m_filmId));
    url.setQuery(query);

    logInfo("Request to the server:" + url.toString().toStdString());

    if (!fs::exists(m_playerConfig.video_path)) {
        fs::create_directories(m_playerConfig.video_path);
    }


    m_videoSavePath = QString::fromStdString(m_playerConfig.video_path) + "/" + QString::number(m_filmData[row].m_filmId) + ".mp4" ;
    QFile::remove(m_videoSavePath);

    if (m_videoFile) {
        delete m_videoFile;
    }
    m_videoFile = new QFile(m_videoSavePath);
    if (!m_videoFile->open(QIODevice::WriteOnly)) {
        qWarning() << "Cannot open file for writing:" << m_videoSavePath;
        delete m_videoFile;
        m_videoFile = nullptr;
        return;
    }

    m_downloadVideoReply = networkManager_->get(QNetworkRequest(url));

    connect(m_downloadVideoReply, &QNetworkReply::finished, this, &MainWindow::onDownloadFinished);
    connect(m_downloadVideoReply, &QIODevice::readyRead, this, [this]() {
        if (m_videoFile) {
            m_videoFile->write(m_downloadVideoReply->readAll());
        }
    });
}

void MainWindow::onDownloadFinished()
{
    if (!m_downloadVideoReply) return;

    auto error = m_downloadVideoReply->error();
    QString errorStr = m_downloadVideoReply->errorString();
    qint64 expectedSize = m_downloadVideoReply->header(QNetworkRequest::ContentLengthHeader).toLongLong();

    if (m_videoFile) {
        m_videoFile->close();
        delete m_videoFile;
        m_videoFile = nullptr;
    }

    QFileInfo fi(m_videoSavePath);
    qint64 downloadedSize = fi.size();

    bool isComplete = (expectedSize <= 0) || (downloadedSize == expectedSize);

    if (downloadedSize == 0 || !isComplete) {
        // broken or empty — delete
        QFile::remove(m_videoSavePath);
        logWarn("Download failed or incomplete. File removed.");
    }
    else {
        // RemoteHostClosedError - We're not doing anything
        if (error == QNetworkReply::RemoteHostClosedError) {
            logInfo("Server closed connection, but file is complete. Playing...");
        }
        else {
            if (error != QNetworkReply::NoError) {
                // Other errors - we stop
                QFile::remove(m_videoSavePath);
                logWarn("Real network error:" +errorStr.toStdString());
                m_downloadVideoReply->deleteLater();
                m_downloadVideoReply = nullptr;
                return;
            }
        }

        logInfo("Starting playback...");
        playLocalFile(m_videoSavePath);
    }

    m_downloadVideoReply->deleteLater();
    m_downloadVideoReply = nullptr;
}


void MainWindow::setupPlayerControls()
{
    playerLayout_ = new QVBoxLayout();

    playerStatus_ = new QLabel("Status of server ");
    playerStatus_->setStyleSheet("color: #555; font-size: 12px;");
    showStatusServer();
    playerLayout_->addWidget(playerStatus_);
}

void MainWindow::showStatusServer()
{
    QString statusText = QString("Number of films: %1 | Active workers: %2 | Queue length: %3 | Number of errors: %4 ")
        .arg(m_statData.total_films)
        .arg(m_statData.pf_active_workers)
        .arg(m_statData.pf_queue_length)
        .arg(m_statData.pf_errors_total);

    playerStatus_->setText("Status of server: " + statusText);
}

QUrl MainWindow::buildSearchUrl() const
{
    QUrl url(QString::fromStdString(createHttpSearchRequest()));
    QUrlQuery query;

    if ( !searchInputTitle->text().trimmed().isEmpty()) {
        const QString text = searchInputTitle->text().trimmed();
        if (!text.isEmpty()) {
            query.addQueryItem("by_title", text);
        }
    }
    if (!searchInputActor->text().trimmed().isEmpty()) {
        const QString text = searchInputActor->text().trimmed();
        if (!text.isEmpty()) {
            query.addQueryItem("by_actor", text);
        }
    }

    if (!searchInputGenre->text().trimmed().isEmpty()) {
        const QString text = searchInputGenre->text().trimmed();
        if (!text.isEmpty()) {
            query.addQueryItem("by_genre", text);
        }
    }
    url.setQuery(query);
    return url;
}

void MainWindow::onSearchClicked()
{
    resultsTable_->setRowCount(0);

    QUrl url = buildSearchUrl();
    logInfo("Request to the server:" + url.toString().toStdString());
    m_downloadSearchReply = networkManager_->get(QNetworkRequest(url));
}

void MainWindow::onRequestFinished(QNetworkReply* reply)
{
    if (reply == m_downloadSearchReply) {
        onSearchFinished(reply);
        return;
    }
    if ( reply == m_downloadTimerReply) {
        onStatsFinished(reply);
        showStatusServer();
        return;
    }
}

void MainWindow::onSearchFinished(QNetworkReply* reply)
{
    m_filmData.clear();
    if (!reply)
        return;

    QVariant error = reply->error();
    if (reply->error() != QNetworkReply::NetworkError::NoError) {
        logError("Network error:" +reply->errorString().toStdString());
        reply->deleteLater();
        return;
    }

    QByteArray responseData = reply->readAll();
    reply->deleteLater();

    fillTableFromJson(responseData);
    m_downloadSearchReply = nullptr;
}

void MainWindow::onStatsFinished(QNetworkReply* reply)
{
    if (!reply)
        return;

    if (reply->error() != QNetworkReply::NetworkError::NoError) {
        logError("Network error:" +reply->errorString().toStdString());
        reply->deleteLater();
        return;
    }

    QByteArray responseData = reply->readAll();
    reply->deleteLater();
    getStatDataFromJson(responseData);
    m_downloadTimerReply = nullptr;
}

bool MainWindow::isVlcAvailable() const
{
    QProcess checkProc;
    QStringList args;
    args << "--version";

    checkProc.start("vlc", args);
    if (!checkProc.waitForStarted(1000)) {
        return false;
    }
    if (!checkProc.waitForFinished(2000)) {
        return false;
    }
    return checkProc.exitCode() == 0;
}

void MainWindow::fillTableFromJson(const QByteArray& jsonData)
{
    parser::ParserJson parser(jsonData.toStdString());

    try {
        json_data::JsonValuePtr obj = parser.parse();

        if ( obj.get() == nullptr || !obj->isObject()) {
            return ;
        }

        json_data::JsonObject *obj_ptr = static_cast<json_data::JsonObject*>(obj.get());

        auto *result_json = obj_ptr->findMutable("results");
        if (result_json == nullptr || !result_json->isArray()) {
            return ;
        }
        const json_data::JsonArray* res_array = static_cast<json_data::JsonArray*>(result_json);
        const auto &films = res_array->asArray();
        int row = 0 ;

        for (const auto& filmVal : films) {
            common::FilmAnswer answerItem;
            auto film = static_cast<json_data::JsonObject*>(filmVal.get());
            resultsTable_->insertRow(row);

            auto setItem = [&](int col, const QString& text) {
                resultsTable_->setItem(row, col, new QTableWidgetItem(text));
            };
            {
                auto *jsn_id = film->find("film_id");
                auto *jsn_title = film->find("title");
                auto *jsn_year   = film->find("year");
                auto *jsn_director = film->find("director");
                if ( !jsn_id || !jsn_title || !jsn_year || !jsn_director ) {
                    return ;
                }

                setItem(0, QString::fromStdString(jsn_title->asString()));
                setItem(1, QString::number(jsn_year->asNumber()));
                setItem(2, QString::fromStdString(jsn_director->asString()));

                answerItem.m_filmId = jsn_id->asNumber();
                answerItem.m_title = jsn_title->asString();
                answerItem.m_releaseYear = jsn_year->asNumber();
                answerItem.m_directorNames = jsn_director->asString();
            }
            //----------- actors
            {
                auto *jsn_actors = film->findMutable("actors");
                if (jsn_actors == nullptr || !jsn_actors->isArray()) {
                    return ;
                }
                const json_data::JsonArray*actors_ptr  = static_cast<json_data::JsonArray*>(jsn_actors);

                QString actorsStr;
                const auto &actors = actors_ptr->asArray();
                bool first_word = true ;
                for (const auto &acter : actors) {

                    if (first_word == true) {
                        first_word = false ;
                    }
                    else {
                        actorsStr += ", ";
                    }

                    actorsStr += QString::fromStdString(acter->asString());
                    answerItem.m_actorNames.push_back(acter->asString());
                }
                setItem(3, actorsStr);
            }
            //----------- genres
            {
                auto *jsn_genre = film->findMutable("genre");
                const json_data::JsonArray*genre_ptr  = static_cast<json_data::JsonArray*>(jsn_genre);

                QString genresStr;
                const auto &genres = genre_ptr->asArray();
                bool first_word = true ;
                for (const auto &genre : genres) {

                    if (first_word == true) {
                        first_word = false ;
                    }
                    else {
                        genresStr += ", ";
                    }
                    answerItem.m_genreNames.push_back(genre->asString());

                    genresStr += QString::fromStdString(genre->asString());
                }
                setItem(4, genresStr);
            }
            setItem(5, "OK");

            m_filmData.push_back(std::move(answerItem));

            row++;
        }
    }
    catch (parser::LexerError &error) {
        qDebug() << "Error parsing the response from the server " << error.what();
        logError(std::string("Error parsing the response from the server ") + error.what());
    }
    catch (parser::ParseError &error) {
        qDebug() << "Error parsing the response from the server " << error.what();
        logError(std::string("Error parsing the response from the server ") + error.what());
    }
    catch (parser::StreamError &error) {
        qDebug() << "Error parsing the response from the server " << error.what();
        logError(std::string("Error parsing the response from the server ") + error.what());
    }
    catch (std::runtime_error &error) {
        qDebug() << "Error parsing the response from the server " << error.what();
        logError(std::string("Error parsing the response from the server ") + error.what());
    }

}

void MainWindow::getStatDataFromJson(const QByteArray& jsonData)
{
    parser::ParserJson parser(jsonData.toStdString());

    try {
        json_data::JsonValuePtr obj = parser.parse();

        if ( obj.get() == nullptr || !obj->isObject()) {
            return ;
        }
        json_data::JsonObject *obj_ptr = static_cast<json_data::JsonObject*>(obj.get());
        {
            auto *service_json = obj_ptr->findMutable("service");
            if (service_json == nullptr || !service_json->isObject()) {
                return ;
            }
            json_data::JsonObject *service_json_ptr = static_cast<json_data::JsonObject*>(service_json);

            auto service_name_jsn = service_json_ptr->find("name");
            auto service_version_jsn = service_json_ptr->find("version");

            if (service_version_jsn == nullptr || service_version_jsn == nullptr) {
                return ;
            }

            m_statData.service_name = service_name_jsn->asString();
            m_statData.service_version = service_version_jsn->asString();
        }
        {
            auto *data_json = obj_ptr->findMutable("data");
            if (data_json == nullptr || !data_json->isObject()) {
                return ;
            }
            json_data::JsonObject *data_json_ptr = static_cast<json_data::JsonObject*>(data_json);

            auto total_films_jsn = data_json_ptr->find("total_films");
            auto total_actors_jsn = data_json_ptr->find("total_actors");
            auto total_directors_jsn = data_json_ptr->find("total_directors");
            auto total_genres_jsn = data_json_ptr->find("total_genres");
            auto index_tokens_count_jsn = data_json_ptr->find("index_tokens_count");
            auto avg_films_per_actor_jsn = data_json_ptr->find("avg_films_per_actor");

            if (total_films_jsn == nullptr || total_actors_jsn == nullptr
                || total_directors_jsn == nullptr || total_genres_jsn == nullptr
                || index_tokens_count_jsn == nullptr || avg_films_per_actor_jsn == nullptr) {
                return ;
            }

            m_statData.total_films = total_films_jsn->asNumber();
            m_statData.total_actors = total_actors_jsn->asNumber();
            m_statData.total_directors = total_directors_jsn->asNumber();
            m_statData.total_genres = total_genres_jsn->asNumber();
            m_statData.index_tokens_count = index_tokens_count_jsn->asNumber();
            m_statData.avg_films_per_actor = avg_films_per_actor_jsn->asNumber();
        }
        {
            auto *pf_json = obj_ptr->findMutable("performance");
            if (pf_json == nullptr || !pf_json->isObject()) {
                return ;
            }
            json_data::JsonObject *pf_json_json_ptr = static_cast<json_data::JsonObject*>(pf_json);

            auto errors_total_jsn = pf_json_json_ptr->find("errors_total");
            auto avg_response_time_msjsn = pf_json_json_ptr->find("avg_response_time_ms");
            auto requests_total_jsn = pf_json_json_ptr->find("requests_total");
            auto active_workers_jsn = pf_json_json_ptr->find("active_workers");
            auto queue_length_jsn = pf_json_json_ptr->find("queue_length");

            if (errors_total_jsn == nullptr || requests_total_jsn == nullptr
                || active_workers_jsn == nullptr || queue_length_jsn == nullptr) {
                return ;
            }

            m_statData.pf_errors_total = errors_total_jsn->asNumber();
            m_statData.pf_avg_response_time_ms = avg_response_time_msjsn->asNumber();
            m_statData.pf_requests_total = requests_total_jsn->asNumber();
            m_statData.pf_active_workers = active_workers_jsn->asNumber();
            m_statData.pf_queue_length = queue_length_jsn->asNumber();
        }
    }
    catch (parser::LexerError &error) {
        qDebug() << "Error parsing the response from the server " << error.what();
        logError(std::string("Error parsing the response from the server ") + error.what());
    }
    catch (parser::ParseError &error) {
        qDebug() << "Error parsing the response from the server " << error.what();
        logError(std::string("Error parsing the response from the server ") + error.what());
    }
    catch (parser::StreamError &error) {
        qDebug() << "Error parsing the response from the server " << error.what();
        logError(std::string("Error parsing the response from the server ") + error.what());
    }
    catch (std::runtime_error &error) {
        qDebug() << "Error parsing the response from the server " << error.what();
        logError(std::string("Error parsing the response from the server ") + error.what());
    }
}

void MainWindow::playLocalFile(const QString &path)
{
    const QFileInfo fi(path);
    if (!fi.exists() || !fi.isFile()) {
        logError(std::string("File does not exist or is not a file: ") + path.toStdString());
        QMessageBox::critical(this, "Error", "The file was not found or is not a file.");
        return;
    }

    // Проверка наличия VLC через отдельный метод
    if (!isVlcAvailable()) {
        logError("VLC not available for playback.");
        QMessageBox::critical(
            this,
            "Playback error",
            "Couldn't start VLC.\n\nMake sure that VLC is installed and added to the PATH."
        );
        return;
    }

    logInfo("Starting VLC to play: " + path.toStdString() + " (size: " + std::to_string(fi.size()) + " bytes)");

    m_vlcProcess = new QProcess(this);

    QStringList args;
    args << "--play-and-exit"<< "--no-video-title-show" << path;

    connect(m_vlcProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &MainWindow::onVlcFinished);
    connect(m_vlcProcess, &QProcess::errorOccurred,
            this, &MainWindow::onVlcError);

    m_vlcProcess->start("vlc", args);

    if (!m_vlcProcess->waitForStarted(2000)) {
        std::string err = m_vlcProcess->errorString().toStdString();
        logError("Failed to start VLC process: " + err);
        delete m_vlcProcess;
        m_vlcProcess = nullptr;

        QMessageBox::critical(
            this,
            "Playback error",
            "Couldn't start VLC.\n\n" + QString::fromStdString(err)
        );
        return;
    }

    logDebug("VLC process started successfully.");
}

void MainWindow::onVlcFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    logInfo(std::string("VLC finished. Exit code: ") + std::to_string(exitCode) +
            ", Status: " + (exitStatus == QProcess::NormalExit ? "Normal" : "Crashed"));

    // Освобождаем указатель
    if (m_vlcProcess) {
        m_vlcProcess->deleteLater();
        m_vlcProcess = nullptr;
    }
}

void MainWindow::onVlcError(QProcess::ProcessError error)
{
    std::string msg = m_vlcProcess ? m_vlcProcess->errorString().toStdString() : "Unknown error";
    logError(std::string("VLC process error: ") + msg);

    if (m_vlcProcess) {
        m_vlcProcess->deleteLater();
        m_vlcProcess = nullptr;
    }
}

std::string MainWindow::createHttpRequest(const std::string &name_service) const
{
    std::string httpRequest("http://");
    std::string httpVersion = httpRequest + m_playerConfig.address + ":" + std::to_string(m_playerConfig.port) + name_service ;
    return httpRequest + m_playerConfig.address + ":" + std::to_string(m_playerConfig.port) + name_service;
}

std::string MainWindow::createHttpSearchRequest() const
{
    return createHttpRequest("/api/v1/search") ;
}

std::string MainWindow::createHttpVideoRequest() const
{
    return createHttpRequest("/api/v1/video") ;
}

std::string MainWindow::createHttpStatRequest() const
{
    return createHttpRequest("/api/v1/stats") ;
}

