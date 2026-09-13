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

    private final static Pattern DOWNGRADE_PATT = Pattern.compile(".*downgrade\\s+database\\s+from\\s+version\\s+(\\d+)\\s+to\\s+(\\d+)");

    private HashMap<String,SQLiteDatabase> managedDatabases = new HashMap<>();
    private Context context;

    public LetosDbService(Context context) {
        this.context = context.getApplicationContext();
    }

    public List<String> getDbList() {
        List<String> filteredList = new ArrayList<>();
        for (String dbFile : context.databaseList()) {
            if (dbFile.endsWith("-journal"))
                continue;

            filteredList.add(dbFile);
        }
        return filteredList;
    }

    public boolean deleteDb(String dbName) {
        return context.deleteDatabase(dbName);
    }

    public void releaseAll() {
        for (SQLiteDatabase db : managedDatabases.values()) {
            db.close();
        }
        managedDatabases.clear();
    }

    public QueryResults exec(String dbName, String query) {
        SQLiteDatabase db = getDb(dbName);
        QueryResults results;
        try {
            Cursor cursor = db.rawQuery(query, null);
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
        }

        return results;
    }

    private SQLiteDatabase getDb(String name) {
        if (managedDatabases.containsKey(name)) {
            return managedDatabases.get(name);
        }

        SQLiteDatabase db = null;
        try {
            db = tryToGetDb(name, 1);
        } catch (SQLiteException e) {
            // If this is "cannot downgrade" problem, try to open with target version.
            String msg = e.getMessage();
            Matcher m = DOWNGRADE_PATT.matcher(msg);
            if (m.find()) {
                db = tryToGetDb(name, Integer.parseInt(m.group(1)));
            } else {
                throw e;
            }
        }
        managedDatabases.put(name, db);
        return db;
    }

    private SQLiteDatabase tryToGetDb(String name, int version) {
        LetosDbOpenHelper helper = new LetosDbOpenHelper(context, name, version);
        Log.e("LETOS-DB", "before getWritableDatabase");
        try {
            SQLiteDatabase db = helper.getWritableDatabase();
            Log.e("LETOS-DB", "after getWritableDatabase");
            return db;
        } catch (Throwable t) {
            Log.e("LETOS-DB", "getWritableDatabase failed", t);
            throw t;
        }
//        return helper.getWritableDatabase();
    }
}
