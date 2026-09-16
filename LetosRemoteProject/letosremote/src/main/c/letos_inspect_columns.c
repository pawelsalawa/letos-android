#include <sqlite3.h>
#include <string.h>

typedef struct LetosInspectColumnsVtab {
    sqlite3_vtab base;
    sqlite3* db;
} LetosInspectColumnsVtab;

typedef struct LetosInspectColumnsCursor {
    sqlite3_vtab_cursor base;
    sqlite3_stmt* stmt;
    char* query;
    int columnIndex;
    int eof;
} LetosInspectColumnsCursor;

static char* letosDupText(const char* z) {
    if (!z) return NULL;
    size_t n = strlen(z);
    char* out = sqlite3_malloc64(n + 1);
    if (!out) return NULL;
    memcpy(out, z, n + 1);
    return out;
}

static void letosClearCursor(LetosInspectColumnsCursor* c) {
    if (c->stmt) {
        sqlite3_finalize(c->stmt);
        c->stmt = NULL;
    }
    sqlite3_free(c->query);
    c->query = NULL;
    c->columnIndex = 0;
    c->eof = 1;
}

static int letosInspectColumnsDisconnect(sqlite3_vtab* pVtab) {
    sqlite3_free(pVtab);
    return SQLITE_OK;
}

static int letosInspectColumnsConnect(
    sqlite3* db, void* pAux, int argc, const char* const* argv,
    sqlite3_vtab** ppVtab, char** pzErr
) {
    (void)pAux;
    (void)argc;
    (void)argv;
    (void)pzErr;

    int rc = sqlite3_declare_vtab(db,
        "CREATE TABLE x("
        "query HIDDEN, "
        "database TEXT, "
        "table_name TEXT, "
        "column_name TEXT, "
        "alias TEXT)"
    );
    if (rc != SQLITE_OK) return rc;

    LetosInspectColumnsVtab* vtab = sqlite3_malloc64(sizeof(*vtab));
    if (!vtab) return SQLITE_NOMEM;
    memset(vtab, 0, sizeof(*vtab));
    vtab->db = db;
    *ppVtab = &vtab->base;
    return SQLITE_OK;
}

static int letosInspectColumnsBestIndex(sqlite3_vtab* tab, sqlite3_index_info* pIdxInfo) {
    (void)tab;
    for (int i = 0; i < pIdxInfo->nConstraint; ++i) {
        const struct sqlite3_index_constraint* c = &pIdxInfo->aConstraint[i];
        if (c->usable && c->iColumn == 0 && c->op == SQLITE_INDEX_CONSTRAINT_EQ) {
            pIdxInfo->aConstraintUsage[i].argvIndex = 1;
            pIdxInfo->aConstraintUsage[i].omit = 1;
            pIdxInfo->idxNum = 1;
            pIdxInfo->estimatedCost = 10.0;
            return SQLITE_OK;
        }
    }
    pIdxInfo->estimatedCost = 1e9;
    return SQLITE_OK;
}

static int letosInspectColumnsOpen(sqlite3_vtab* p, sqlite3_vtab_cursor** ppCursor) {
    (void)p;
    LetosInspectColumnsCursor* c = sqlite3_malloc64(sizeof(*c));
    if (!c) return SQLITE_NOMEM;
    memset(c, 0, sizeof(*c));
    c->eof = 1;
    *ppCursor = &c->base;
    return SQLITE_OK;
}

static int letosInspectColumnsClose(sqlite3_vtab_cursor* cur) {
    LetosInspectColumnsCursor* c = (LetosInspectColumnsCursor*)cur;
    letosClearCursor(c);
    sqlite3_free(c);
    return SQLITE_OK;
}

static int letosInspectColumnsFilter(
    sqlite3_vtab_cursor* pCursor, int idxNum, const char* idxStr,
    int argc, sqlite3_value** argv
) {
    (void)idxStr;
    LetosInspectColumnsCursor* c = (LetosInspectColumnsCursor*)pCursor;
    LetosInspectColumnsVtab* vtab = (LetosInspectColumnsVtab*)pCursor->pVtab;

    letosClearCursor(c);

    if (idxNum != 1 || argc < 1) {
        return SQLITE_ERROR;
    }

    const unsigned char* q = sqlite3_value_text(argv[0]);
    if (!q) {
        return SQLITE_NOMEM;
    }

    c->query = letosDupText((const char*)q);
    if (!c->query) {
        return SQLITE_NOMEM;
    }

    int rc = sqlite3_prepare_v3(vtab->db, c->query, -1, 0, &c->stmt, NULL);
    if (rc != SQLITE_OK) {
        letosClearCursor(c);
        c->eof = 1;
        return rc;
    }

    if (sqlite3_column_count(c->stmt) <= 0) {
        c->eof = 1;
    } else {
        c->columnIndex = 0;
        c->eof = 0;
    }
    return SQLITE_OK;
}

static int letosInspectColumnsNext(sqlite3_vtab_cursor* pCursor) {
    LetosInspectColumnsCursor* c = (LetosInspectColumnsCursor*)pCursor;
    if (!c->stmt) {
        c->eof = 1;
        return SQLITE_OK;
    }
    c->columnIndex++;
    if (c->columnIndex >= sqlite3_column_count(c->stmt)) {
        c->eof = 1;
    }
    return SQLITE_OK;
}

static int letosInspectColumnsEof(sqlite3_vtab_cursor* pCursor) {
    return ((LetosInspectColumnsCursor*)pCursor)->eof;
}

static int letosInspectColumnsColumn(sqlite3_vtab_cursor* pCursor, sqlite3_context* ctx, int i) {
    LetosInspectColumnsCursor* c = (LetosInspectColumnsCursor*)pCursor;
    if (!c->stmt || c->eof) {
        sqlite3_result_null(ctx);
        return SQLITE_OK;
    }

    switch (i) {
        case 0:
            sqlite3_result_text(ctx, c->query ? c->query : "", -1, SQLITE_TRANSIENT);
            break;
        case 1: {
            const char* v = sqlite3_column_database_name(c->stmt, c->columnIndex);
            if (v) sqlite3_result_text(ctx, v, -1, SQLITE_TRANSIENT);
            else sqlite3_result_null(ctx);
            break;
        }
        case 2: {
            const char* v = sqlite3_column_table_name(c->stmt, c->columnIndex);
            if (v) sqlite3_result_text(ctx, v, -1, SQLITE_TRANSIENT);
            else sqlite3_result_null(ctx);
            break;
        }
        case 3: {
            const char* v = sqlite3_column_origin_name(c->stmt, c->columnIndex);
            if (v) sqlite3_result_text(ctx, v, -1, SQLITE_TRANSIENT);
            else sqlite3_result_null(ctx);
            break;
        }
        case 4: {
            const char* v = sqlite3_column_name(c->stmt, c->columnIndex);
            if (v) sqlite3_result_text(ctx, v, -1, SQLITE_TRANSIENT);
            else sqlite3_result_null(ctx);
            break;
        }
        default:
            sqlite3_result_null(ctx);
            break;
    }
    return SQLITE_OK;
}

static int letosInspectColumnsRowid(sqlite3_vtab_cursor* pCursor, sqlite3_int64* pRowid) {
    LetosInspectColumnsCursor* c = (LetosInspectColumnsCursor*)pCursor;
    *pRowid = (sqlite3_int64)(c->columnIndex + 1);
    return SQLITE_OK;
}

static sqlite3_module letosInspectColumnsModule = {
    0,
    0,
    letosInspectColumnsConnect,
    letosInspectColumnsBestIndex,
    letosInspectColumnsDisconnect,
    0,
    letosInspectColumnsOpen,
    letosInspectColumnsClose,
    letosInspectColumnsFilter,
    letosInspectColumnsNext,
    letosInspectColumnsEof,
    letosInspectColumnsColumn,
    letosInspectColumnsRowid,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};

static int letosInspectColumnsExtensionInit(
    sqlite3* db, char** pzErrMsg, const struct sqlite3_api_routines* pApi
) {
    (void)pzErrMsg;
    (void)pApi;
    return sqlite3_create_module(db, "letos_inspect_columns", &letosInspectColumnsModule, NULL);
}

int letosSqliteExtraInit(const char* unused) {
    (void)unused;
    return sqlite3_auto_extension((void (*)(void))letosInspectColumnsExtensionInit);
}
