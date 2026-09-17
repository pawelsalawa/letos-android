#include <sqlite3.h>
#include <string.h>

typedef struct LetosInspectColumnsVtab {
    mc_sqlite3_vtab base;
    mc_sqlite3* db;
} LetosInspectColumnsVtab;

typedef struct LetosInspectColumnsCursor {
    mc_sqlite3_vtab_cursor base;
    mc_sqlite3_stmt* stmt;
    char* query;
    int columnIndex;
    int eof;
} LetosInspectColumnsCursor;

static char* letosDupText(const char* z) {
    if (!z) return NULL;
    size_t n = strlen(z);
    char* out = mc_sqlite3_malloc64(n + 1);
    if (!out) return NULL;
    memcpy(out, z, n + 1);
    return out;
}

static void letosClearCursor(LetosInspectColumnsCursor* c) {
    if (c->stmt) {
        mc_sqlite3_finalize(c->stmt);
        c->stmt = NULL;
    }
    mc_sqlite3_free(c->query);
    c->query = NULL;
    c->columnIndex = 0;
    c->eof = 1;
}

static int letosInspectColumnsDisconnect(mc_sqlite3_vtab* pVtab) {
    mc_sqlite3_free(pVtab);
    return SQLITE_OK;
}

static int letosInspectColumnsConnect(
    mc_sqlite3* db, void* pAux, int argc, const char* const* argv,
    mc_sqlite3_vtab** ppVtab, char** pzErr
) {
    (void)pAux;
    (void)argc;
    (void)argv;
    (void)pzErr;

    int rc = mc_sqlite3_declare_vtab(db,
        "CREATE TABLE x("
        "query HIDDEN, "
        "database TEXT, "
        "table_name TEXT, "
        "column_name TEXT, "
        "alias TEXT)"
    );
    if (rc != SQLITE_OK) return rc;

    LetosInspectColumnsVtab* vtab = mc_sqlite3_malloc64(sizeof(*vtab));
    if (!vtab) return SQLITE_NOMEM;
    memset(vtab, 0, sizeof(*vtab));
    vtab->db = db;
    *ppVtab = &vtab->base;
    return SQLITE_OK;
}

static int letosInspectColumnsBestIndex(mc_sqlite3_vtab* tab, mc_sqlite3_index_info* pIdxInfo) {
    (void)tab;
    for (int i = 0; i < pIdxInfo->nConstraint; ++i) {
        const struct mc_sqlite3_index_constraint* c = &pIdxInfo->aConstraint[i];
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

static int letosInspectColumnsOpen(mc_sqlite3_vtab* p, mc_sqlite3_vtab_cursor** ppCursor) {
    (void)p;
    LetosInspectColumnsCursor* c = mc_sqlite3_malloc64(sizeof(*c));
    if (!c) return SQLITE_NOMEM;
    memset(c, 0, sizeof(*c));
    c->eof = 1;
    *ppCursor = &c->base;
    return SQLITE_OK;
}

static int letosInspectColumnsClose(mc_sqlite3_vtab_cursor* cur) {
    LetosInspectColumnsCursor* c = (LetosInspectColumnsCursor*)cur;
    letosClearCursor(c);
    mc_sqlite3_free(c);
    return SQLITE_OK;
}

static int letosInspectColumnsFilter(
    mc_sqlite3_vtab_cursor* pCursor, int idxNum, const char* idxStr,
    int argc, mc_sqlite3_value** argv
) {
    (void)idxStr;
    LetosInspectColumnsCursor* c = (LetosInspectColumnsCursor*)pCursor;
    LetosInspectColumnsVtab* vtab = (LetosInspectColumnsVtab*)pCursor->pVtab;

    letosClearCursor(c);

    if (idxNum != 1 || argc < 1) {
        return SQLITE_ERROR;
    }

    const unsigned char* q = mc_sqlite3_value_text(argv[0]);
    if (!q) {
        return SQLITE_NOMEM;
    }

    c->query = letosDupText((const char*)q);
    if (!c->query) {
        return SQLITE_NOMEM;
    }

    int rc = mc_sqlite3_prepare_v3(vtab->db, c->query, -1, 0, &c->stmt, NULL);
    if (rc != SQLITE_OK) {
        letosClearCursor(c);
        c->eof = 1;
        return rc;
    }

    if (mc_sqlite3_column_count(c->stmt) <= 0) {
        c->eof = 1;
    } else {
        c->columnIndex = 0;
        c->eof = 0;
    }
    return SQLITE_OK;
}

static int letosInspectColumnsNext(mc_sqlite3_vtab_cursor* pCursor) {
    LetosInspectColumnsCursor* c = (LetosInspectColumnsCursor*)pCursor;
    if (!c->stmt) {
        c->eof = 1;
        return SQLITE_OK;
    }
    c->columnIndex++;
    if (c->columnIndex >= mc_sqlite3_column_count(c->stmt)) {
        c->eof = 1;
    }
    return SQLITE_OK;
}

static int letosInspectColumnsEof(mc_sqlite3_vtab_cursor* pCursor) {
    return ((LetosInspectColumnsCursor*)pCursor)->eof;
}

static int letosInspectColumnsColumn(mc_sqlite3_vtab_cursor* pCursor, mc_sqlite3_context* ctx, int i) {
    LetosInspectColumnsCursor* c = (LetosInspectColumnsCursor*)pCursor;
    if (!c->stmt || c->eof) {
        mc_sqlite3_result_null(ctx);
        return SQLITE_OK;
    }

    switch (i) {
        case 0:
            mc_sqlite3_result_text(ctx, c->query ? c->query : "", -1, SQLITE_TRANSIENT);
            break;
        case 1: {
            const char* v = mc_sqlite3_column_database_name(c->stmt, c->columnIndex);
            if (v) mc_sqlite3_result_text(ctx, v, -1, SQLITE_TRANSIENT);
            else mc_sqlite3_result_null(ctx);
            break;
        }
        case 2: {
            const char* v = mc_sqlite3_column_table_name(c->stmt, c->columnIndex);
            if (v) mc_sqlite3_result_text(ctx, v, -1, SQLITE_TRANSIENT);
            else mc_sqlite3_result_null(ctx);
            break;
        }
        case 3: {
            const char* v = mc_sqlite3_column_origin_name(c->stmt, c->columnIndex);
            if (v) mc_sqlite3_result_text(ctx, v, -1, SQLITE_TRANSIENT);
            else mc_sqlite3_result_null(ctx);
            break;
        }
        case 4: {
            const char* v = mc_sqlite3_column_name(c->stmt, c->columnIndex);
            if (v) mc_sqlite3_result_text(ctx, v, -1, SQLITE_TRANSIENT);
            else mc_sqlite3_result_null(ctx);
            break;
        }
        default:
            mc_sqlite3_result_null(ctx);
            break;
    }
    return SQLITE_OK;
}

static int letosInspectColumnsRowid(mc_sqlite3_vtab_cursor* pCursor, mc_sqlite3_int64* pRowid) {
    LetosInspectColumnsCursor* c = (LetosInspectColumnsCursor*)pCursor;
    *pRowid = (mc_sqlite3_int64)(c->columnIndex + 1);
    return SQLITE_OK;
}

static mc_sqlite3_module letosInspectColumnsModule = {
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
    mc_sqlite3* db, char** pzErrMsg, const struct mc_sqlite3_api_routines* pApi
) {
    (void)pzErrMsg;
    (void)pApi;
    return mc_sqlite3_create_module(db, "letos_inspect_columns", &letosInspectColumnsModule, NULL);
}

int letosSqliteExtraInit(const char* unused) {
    (void)unused;
    return mc_sqlite3_auto_extension((void (*)(void))letosInspectColumnsExtensionInit);
}
