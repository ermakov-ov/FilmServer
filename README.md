# film-server

A demonstration HTTP server written in C++ showcasing the implementation of key components: a custom JSON parser, 
a thread pool, and fast search mechanisms over data without using a database. Works in conjunction with the player client (Qt6).
> **Important:** This project is intended to demonstrate architectural solutions and development techniques. It is not meant for production use.

## Что реализовано и зачем

- **Custom JSON parser (JsonParser/json_lib)** — a basic implementation for loading and parsing JSON files without external dependencies. It is not positioned as an industrial‑grade library comparable to RapidJSON or nlohmann::json. It serves as a demonstration component showing how to solve a narrow task (loading project data) under specific conditions and constraints.
- **FilmDb and indexing ** — earch by title, actors, directors, and genres using `std::unordered_map<std::string, std::vector<int>>` with tokenization and lowercase normalization. Demonstrates how to implement fast search without a DBMS.
- **Thread Pool** — a custom thread pool implementation for request handling.
- **Минималистичный API** — only basic endpoints to illustrate the core principles.
- **Integration with the  `player`** — a Qt6 application with tabs and search functionality that leverages these mechanisms.

## API endpoints

| Method | Path                   | Description                                                                                                                                                                                                                                      |
|--------|------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `GET`  | `/api/v1/films`        | Search for films with filtering by parameters: by_title, by_actor, by_director, by_genre. Filtering is applied only for the provided parameters. If no parameters are provided, no search is performed and an empty result is returned. |
| `GET`  | `/api/v1/stats`        | Statistics (demonstration of aggregation).                                                                                                                                                                                                             |
| `GET`  | `/api/v1/openapi.yaml` | OpenAPI specification.                                                                                                                                                                                                                            |
| `GET`  | `/docs`                | Swagger UI (minimal version).                                                                                                                                                                                                                 |

## Примеры запросов (curl)

```bash
# By title
curl "http://127.0.0.1:8080/api/v1/films?by_title=matrix"

# By actor
curl "http://127.0.0.1:8080/api/v1/films?by_actor=keanu"

# By director
curl "http://127.0.0.1:8080/api/v1/films?by_director=spielberg"

# By genre
curl "http://127.0.0.1:8080/api/v1/films?by_genre=action"
```

## Search logic overview

- Strings (titles, actor names, director names, genres) are converted to lowercase.
- They are split into tokens (words).
- Tokens are mapped to film IDs in an unordered_map (similar mappings exist for actors, titles, directors, and genres).
- The result is formed by intersecting the sets of IDs according to the specified filters.


### Data and configuration

The system does not use a database. Information about films, actors, directors, and genres is stored in JSON files.
Paths to the files are specified in the server’s configuration file.


### Example server configuration (server_config.json in JSON format)
```bash
{
  "db": {
    "paths": {
      "actors": "./data/actors.json",
      "films": "./data/films.json",
      "video": "./data/videos",
      "directors": "./data/directors.json",
      "genres": "./data/genres.json"
    }
  },
  "connection": {
    "port": 8080
  },
  "logging": {
    "log_dir": "./"
  }
}
```

### Example data files
## films.json
```bash
[
  {
    "id": 100,
    "title": "Forrest Gump",
    "releaseYear": 1994,
    "description": "Life is like a box of chocolates...",
    "genres": ["Drama", "Romance"],
    "actorIds": [1],
    "directorId": 22
  },
  {
    "id": 101,
    "title": "The Shawshank Redemption",
    "releaseYear": 1994,
    "description": "Two imprisoned men bond over a number of years...",
    "genres": ["Drama"],
    "actorIds": [2],
    "directorId": 11
  }
]
```
## actors.json
```bash
[
  {"id": 1, "name": "Tom Hanks"},
  {"id": 2, "name": "Morgan Freeman"},
  {"id": 3, "name": "Leonardo DiCaprio"}
]

```
## directors.json
```bash
[
  {"id": 1, "name": "Tom Hanks"},
  {"id": 2, "name": "Morgan Freeman"},
  {"id": 3, "name": "Leonardo DiCaprio"}
]

```
## genres.json
```bash
[
  {"id": 1, "name": "Action"},
  {"id": 2, "name": "Adventure"}
]

```
### Player client configuration
The player client also uses a configuration file to define paths and connection parameters.

```bash
{
  "video": {
    "path": "./videos"
  },
  "logging": {
    "log_dir": "./"
  },
  "connection": {
    "address": "127.0.0.1",
    "port": 8080
  }
}
```
Video playback uses the VLC player. The check for VLC availability is implemented in a separate method isVlcAvailable, 
which is called only when attempting to start a file.

### Logging

Both the server and the client write logs to the directory specified in the configuration (log_dir). The log format is 
plain text, line‑by‑line, including the log level (INFO/WARN/ERROR), timestamp, and message.

### Build and run
Requirements
- C++17 or higher (tested with GNU g++ 13.3.0)
- CMake ≥ 3.16 (tested with 3.28)
- Qt6 (for the client)


### Build (CMake)
```bash
rm -rf build && mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
cd ..
```

### Run

```bash
./build/src/apps/server/Server -c ./server_config.json
./build/src/apps/client/Player -c ./player_config.json
```
Data and media files are located in the data/ folder; logs are written to data/log/.