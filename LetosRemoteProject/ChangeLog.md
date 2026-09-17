# 1.3.3
- SQLite3MultipleCiphers amalgamation gets its symbols prefixed, so they don't collide with the symbols of the SQLite3 built into the Android.
- Added `sqlite3` binaary builds with `compile_mc.sh`, so they can be easily copied to the DbAndroid in Letos.

# 1.3.2
- Switched the letosremote connector entirely to SQLite3MultipleCiphers.