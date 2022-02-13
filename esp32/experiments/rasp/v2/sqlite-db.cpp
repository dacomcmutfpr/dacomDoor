#ifndef APP_SQLITE_DB
#define APP_SQLITE_DB

#include <MFRC522.h>
#include <sqlite3.h>

const static String SQL_NULL = "@@NULL";

class SqlRow
{
  int n;
  char **data;
  char **cols;
  bool allocated;

public:
  SqlRow() { this->allocated = false; }

  void setup(int n, char **data, char **coll)
  {
    this->clear();

    this->n = n;
    this->data = (char **)calloc(n, sizeof(char **));
    this->cols = (char **)calloc(n, sizeof(char **));
    for (int i = 0; i < n; i++)
    {
      this->data[i] = strdup(data[i]);
      this->cols[i] = strdup(coll[i]);
    }
    this->allocated = true;
  }

  void clear()
  {
    if (!this->allocated)
      return;

    for (int i = 0; i < this->n; i++)
    {
      free(this->data[i]);
      free(this->cols[i]);
    }
    free(this->data);
    free(this->cols);
    this->allocated = false;
  }

  String getCol(String colName)
  {
    if (!this->allocated)
      return SQL_NULL;

    for (int i = 0; i < this->n; i++)
    {
      if (colName.equals(this->cols[i]))
      {
        if (this->data[i])
          return String(this->data[i]);
        else
          return SQL_NULL;
      }
    }

    return SQL_NULL;
  }

  static void print(int n, char **data, char **cols)
  {
    int i;
    for (i = 0; i < n; i++)
    {
      Serial.print(cols[i]);
      Serial.print(": ");
      if (data[i])
        Serial.println(data[i]);
      else
        Serial.println(SQL_NULL);
    }
  }

  void print()
  {
    if (this->allocated)
      SqlRow::print(this->n, this->data, this->cols);
  }
};

// used on: exec, exec1
static bool db_query_returned;
static SqlRow first_row;

class SqliteDB
{
private:
  const char *filename;
  sqlite3 *db = NULL;

public:
  SqliteDB(const char *filename) { this->filename = filename; }

  void init() { sqlite3_initialize(); }

  int initSession()
  {
    int rc = sqlite3_open(this->filename, &this->db);
    if (rc)
    {
      Serial.printf("[ERROR]: Can't open database: %s\n", sqlite3_errmsg(this->db));
      return rc;
    }
    return rc;
  }

  void closeSession() { sqlite3_close(this->db); }

  int execWithContext(const char *sql, void *ctx, int (*cb)(void *, int, char **, char **))
  {
    char *zErrMsg = NULL;
    Serial.println("[LOG]: Executing SQL:\n'''");
    Serial.println(sql);
    Serial.println("'''");
    long start = micros();
    int rc = sqlite3_exec(this->db, sql, cb, ctx, &zErrMsg);
    if (rc != SQLITE_OK)
    {
      Serial.print(F("[ERROR]: SQL error "));
      Serial.printf("[%d]:", rc);
      Serial.println(zErrMsg);
      sqlite3_free(zErrMsg);
    }
    Serial.print(F("[LOG]: Time taken: "));
    Serial.println(micros() - start);
    Serial.print("\n");
    return rc;
  }

  int execWith(const char *sql, int (*cb)(void *, int, char **, char **)) { return this->execWithContext(sql, NULL, cb); }

  int exec(const char *sql)
  {
    return this->execWith(sql, [](void *id, int n, char **data, char **cols) {
      db_query_returned = true;
      return 0;
    });
  }
  int exec1(const char *sql)
  {
    return this->execWith(sql, [](void *id, int n, char **data, char **cols) {
      if (db_query_returned)
        return 0;
      first_row.setup(n, data, cols);
      db_query_returned = true;
      return 0;
    });
  }
  int execPrint(const char *sql)
  {
    return this->execWith(sql, [](void *id, int n, char **data, char **cols) {
      SqlRow::print(n, data, cols);
      Serial.println("<==== END ROW ====>");
      return 0;
    });
  }
};

#endif
