//
// Created by virgil on 2/12/20.
//

#include "tempdb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int callback(void *NotUsed, int argc, char **argv,char **azColName) {
    NotUsed = 0;
    for (int i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

int read_rec()
{
    int rc = 0;
    char sql[] = "SELECT * FROM "TB_NAME;

    rc = sqlite3_exec(db, sql, callback, 0, &err_msg);
    if (rc != SQLITE_OK ) {
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }
    return 0;
}

int tpdb_open(void)
{
    int rc = 0;
    char sql[LEN_SQL_STATE] = {0};

    //open database
    rc = sqlite3_open_v2(DB_PATH, &db,SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }

    strcpy(sql,"CREATE TABLE "TB_NAME"(mac CHAR(6),time DATETIME, temp DOUBLE); ");
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK && rc != 1 ) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }

    return 0;
}

int tpdb_report(struct tdata* td)
{
    int rc;
    char sql[LEN_SQL_STATE] = {0};

    sprintf(sql,"INSERT INTO %s (mac,time,temp) VALUES (\"%s\",\"%s\",%s);",TB_NAME,td->mac,td->dtime,td->temp);
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }
    return 0;
}

int tpdb_clear_all(void)
{
    int rc = 0;
    char sql[256] = {0};

    sprintf(sql,"DELETE FROM temp_rec;");
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }
    return 0;
}

void tpdb_close(void)
{
    sqlite3_close(db);
}


