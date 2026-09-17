/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*
** Modified to support SQLite extensions by the SQLite developers: 
** sqlite-dev@sqlite.org.
*/

#include "android_database_SQLiteCommon.h"

namespace android {

/* throw a SQLiteException with a message appropriate for the error in handle */
void throw_sqlite3_exception(JNIEnv* env, mc_sqlite3* handle) {
    throw_sqlite3_exception(env, handle, NULL);
}

/* throw a SQLiteException with the given message */
void throw_sqlite3_exception(JNIEnv* env, const char* message) {
    throw_sqlite3_exception(env, NULL, message);
}

/* throw a SQLiteException with a message appropriate for the error in handle
   concatenated with the given message
 */
void throw_sqlite3_exception(JNIEnv* env, mc_sqlite3* handle, const char* message) {
    if (handle) {
        // get the error code and message from the SQLite connection
        // the error message may contain more information than the error code
        // because it is based on the extended error code rather than the simplified
        // error code that SQLite normally returns.
        throw_sqlite3_exception(env, mc_sqlite3_extended_errcode(handle),
                mc_sqlite3_errmsg(handle), message);
    } else {
        // we use SQLITE_OK so that a generic SQLiteException is thrown;
        // any code not specified in the switch statement below would do.
        throw_sqlite3_exception(env, SQLITE_OK, "unknown error", message);
    }
}

/* throw a SQLiteException for a given error code
 * should only be used when the database connection is not available because the
 * error information will not be quite as rich */
void throw_sqlite3_exception_errcode(JNIEnv* env, int errcode, const char* message) {
    throw_sqlite3_exception(env, errcode, "unknown error", message);
}

/* throw a SQLiteException for a given error code, mc_sqlite3message, and
   user message
 */
void throw_sqlite3_exception(JNIEnv* env, int errcode,
                             const char* mc_sqlite3Message, const char* message) {
    const char* exceptionClass;
    switch (errcode & 0xff) { /* mask off extended error code */
        case SQLITE_IOERR:
            exceptionClass = "org/sqlite/database/sqlite/SQLiteDiskIOException";
            break;
        case SQLITE_CORRUPT:
        case SQLITE_NOTADB: // treat "unsupported file format" error as corruption also
            exceptionClass = "org/sqlite/database/sqlite/SQLiteDatabaseCorruptException";
            break;
        case SQLITE_CONSTRAINT:
            exceptionClass = "org/sqlite/database/sqlite/SQLiteConstraintException";
            break;
        case SQLITE_ABORT:
            exceptionClass = "org/sqlite/database/sqlite/SQLiteAbortException";
            break;
        case SQLITE_DONE:
            exceptionClass = "org/sqlite/database/sqlite/SQLiteDoneException";
            mc_sqlite3Message = NULL; // SQLite error message is irrelevant in this case
            break;
        case SQLITE_FULL:
            exceptionClass = "org/sqlite/database/sqlite/SQLiteFullException";
            break;
        case SQLITE_MISUSE:
            exceptionClass = "org/sqlite/database/sqlite/SQLiteMisuseException";
            break;
        case SQLITE_PERM:
            exceptionClass = "org/sqlite/database/sqlite/SQLiteAccessPermException";
            break;
        case SQLITE_BUSY:
            exceptionClass = "org/sqlite/database/sqlite/SQLiteDatabaseLockedException";
            break;
        case SQLITE_LOCKED:
            exceptionClass = "org/sqlite/database/sqlite/SQLiteTableLockedException";
            break;
        case SQLITE_READONLY:
            exceptionClass = "org/sqlite/database/sqlite/SQLiteReadOnlyDatabaseException";
            break;
        case SQLITE_CANTOPEN:
            exceptionClass = "org/sqlite/database/sqlite/SQLiteCantOpenDatabaseException";
            break;
        case SQLITE_TOOBIG:
            exceptionClass = "org/sqlite/database/sqlite/SQLiteBlobTooBigException";
            break;
        case SQLITE_RANGE:
            exceptionClass = "org/sqlite/database/sqlite/SQLiteBindOrColumnIndexOutOfRangeException";
            break;
        case SQLITE_NOMEM:
            exceptionClass = "org/sqlite/database/sqlite/SQLiteOutOfMemoryException";
            break;
        case SQLITE_MISMATCH:
            exceptionClass = "org/sqlite/database/sqlite/SQLiteDatatypeMismatchException";
            break;
        case SQLITE_INTERRUPT:
            exceptionClass = "android/os/OperationCanceledException";
            break;
        default:
            exceptionClass = "org/sqlite/database/sqlite/SQLiteException";
            break;
    }

    if (mc_sqlite3Message) {
        char *zFullmsg = mc_sqlite3_mprintf(
            "%s (code %d)%s%s", mc_sqlite3Message, errcode, 
            (message ? ": " : ""), (message ? message : "")
        );
        jniThrowException(env, exceptionClass, zFullmsg);
        mc_sqlite3_free(zFullmsg);
    } else {
        jniThrowException(env, exceptionClass, message);
    }
}


} // namespace android
