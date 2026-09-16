package org.letos.letosremote.internal;

import android.content.Context;
import android.database.Cursor;
import android.util.Log;

import org.sqlite.database.sqlite.SQLiteAbortException;
import org.sqlite.database.sqlite.SQLiteAccessPermException;
import org.sqlite.database.sqlite.SQLiteBindOrColumnIndexOutOfRangeException;
import org.sqlite.database.sqlite.SQLiteBlobTooBigException;
import org.sqlite.database.sqlite.SQLiteCantOpenDatabaseException;
import org.sqlite.database.sqlite.SQLiteConstraintException;
import org.sqlite.database.sqlite.SQLiteDatabase;
import org.sqlite.database.sqlite.SQLiteDatabaseCorruptException;
import org.sqlite.database.sqlite.SQLiteDatabaseLockedException;
import org.sqlite.database.sqlite.SQLiteDatatypeMismatchException;
import org.sqlite.database.sqlite.SQLiteDiskIOException;
import org.sqlite.database.sqlite.SQLiteDoneException;
import org.sqlite.database.sqlite.SQLiteException;
import org.sqlite.database.sqlite.SQLiteFullException;
import org.sqlite.database.sqlite.SQLiteMisuseException;
import org.sqlite.database.sqlite.SQLiteOutOfMemoryException;
import org.sqlite.database.sqlite.SQLiteReadOnlyDatabaseException;
import org.sqlite.database.sqlite.SQLiteTableLockedException;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Created by Pawel Salawa on 04.02.15.
 */
public class LetosDbService {
    private final HashMap<String,SQLiteDatabase> managedDatabases = new HashMap<>();
    private final Context context;

    public LetosDbService(Context context) {
        this.context = context.getApplicationContext();
    }

    public List<String> getDbList() {
        List<String> filteredList = new ArrayList<>();
        for (String dbFile : context.databaseList()) {
            if (dbFile.endsWith("-journal") || dbFile.endsWith("-shm") || dbFile.endsWith("-wal"))
                continue;

            if (dbFile.contains("?")) {
                dbFile = dbFile.substring(0, dbFile.indexOf("?"));
            }

            filteredList.add(dbFile);
        }
        return filteredList;
    }

    public boolean deleteDb(String dbName) {
        for (String dbFile : context.databaseList()) {
            if (dbFile.endsWith("-journal") || dbFile.endsWith("-shm") || dbFile.endsWith("-wal"))
                continue;

            String cmpName = dbFile.contains("?") ?
                    dbFile.substring(0, dbFile.indexOf("?")) :
                    dbFile;

            if (cmpName.equals(dbName)) {
                return context.deleteDatabase(dbFile);
            }
        }
        return false;
    }

    public synchronized void releaseAll() {
        for (SQLiteDatabase db : managedDatabases.values()) {
            try {
                db.close();
            } catch (Exception ignored) {
            }
        }

        managedDatabases.clear();
    }

    public QueryResults exec(String dbName, String query) {
        Log.d(
                "LETOS",
                "SQL thread=" + Thread.currentThread().getId()
                        + "/" + Thread.currentThread().getName()
                        + " sql=" + query
        );
        SQLiteDatabase db;
        synchronized (this) {
            db = getDb(dbName);
        }
        QueryResults results;
        try (Cursor cursor = db.rawQuery(query, null)) {
            results = new QueryResults();
            results.readResults(cursor);
        } catch (SQLiteAbortException e) {
            results = new QueryResults(e, ErrorCode.SQLITE_ABORT);
        } catch (SQLiteAccessPermException e) {
            results = new QueryResults(e, ErrorCode.SQLITE_PERM);
        } catch (SQLiteBindOrColumnIndexOutOfRangeException e) {
            results = new QueryResults(e, ErrorCode.SQLITE_RANGE);
        } catch (SQLiteBlobTooBigException e) {
            results = new QueryResults(e, ErrorCode.SQLITE_TOOBIG);
        } catch (SQLiteCantOpenDatabaseException e) {
            results = new QueryResults(e, ErrorCode.SQLITE_CANTOPEN);
        } catch (SQLiteConstraintException e) {
            results = new QueryResults(e, ErrorCode.SQLITE_CONSTRAINT);
        } catch (SQLiteDatabaseCorruptException e) {
            results = new QueryResults(e, ErrorCode.SQLITE_CORRUPT);
        } catch (SQLiteDatabaseLockedException e) {
            results = new QueryResults(e, ErrorCode.SQLITE_BUSY);
        } catch (SQLiteDatatypeMismatchException e) {
            results = new QueryResults(e, ErrorCode.SQLITE_MISMATCH);
        } catch (SQLiteDiskIOException e) {
            results = new QueryResults(e, ErrorCode.SQLITE_IOERR);
        } catch (SQLiteDoneException e) {
            results = new QueryResults(e, ErrorCode.SQLITE_DONE);
        } catch (SQLiteFullException e) {
            results = new QueryResults(e, ErrorCode.SQLITE_FULL);
        } catch (SQLiteMisuseException e) {
            results = new QueryResults(e, ErrorCode.SQLITE_MISUSE);
        } catch (SQLiteOutOfMemoryException e) {
            results = new QueryResults(e, ErrorCode.SQLITE_NOMEM);
        } catch (SQLiteReadOnlyDatabaseException e) {
            results = new QueryResults(e, ErrorCode.SQLITE_READONLY);
        } catch (SQLiteTableLockedException e) {
            results = new QueryResults(e, ErrorCode.SQLITE_LOCKED);
        } catch (SQLiteException e) {
            results = new QueryResults(e, ErrorCode.SQLITE_ERROR);
        } catch (IllegalStateException e) {
            // Database was closed concurrently (e.g. releaseAll() called from another thread)
            results = new QueryResults(new SQLiteException(e.getMessage()), ErrorCode.SQLITE_ERROR);
        }

        return results;
    }

    private SQLiteDatabase getDb(String name) {
        SQLiteDatabase cached = managedDatabases.get(name);
        if (cached != null && cached.isOpen()) {
            return cached;
        }

        SQLiteDatabase removed = managedDatabases.remove(name);
        if (removed != null && removed.isOpen()) {
            try {
                removed.close();
            } catch (Exception ignored) {
            }
        }

        String path;
        if (name.startsWith("file:")) {
            path = name;
        } else {
            path = context.getDatabasePath(name).getPath();
        }

        SQLiteDatabase db = SQLiteDatabase.openOrCreateDatabase(
                path,
                null
        );

        managedDatabases.put(name, db);
        return db;
    }
}
