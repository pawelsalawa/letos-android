# letos-android
Service providing support for Letos DbAndroid plugin on device's side.

# Licenses
This project reuses parts of SQLite Android Bindings (https://www.sqlite.org/android/) which is licensed under Apache License 2.0. Specifically this includes source code under `LetosRemoteProject/letosremote/src/main/java/org/sqlite/database`.

It also reuses amalgamation from https://github.com/utelle/SQLite3MultipleCiphers project, which is licensed under MIT license.

# What is this project?
It's a kind of a connector between Letos DbAndroid plugin and your Android application. It's a library, which you can add to your app to make SQLite databases of your app available from Letos during the development phase.

Example
```java
// ...
import pl.com.salsoft.letosremote.LetosService;

public class MainActivity extends ActionBarActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        LetosService.instance().start(this); // This is the line for letosremote
    }

    @Override
    public void onDestroy() {
        LetosService.instance().stop(); // And this is another line for letosremote
        super.onDestroy();
    }
    // ...
}
```

More details at https://github.com/pawelsalawa/letos/wiki/DbAndroid 

# Compatibility
Versions of LetosRemoteProject (the `letosremote-release.aar` output artfifact) and DbAndroid plugin from Letos are at the minor version level,
meaning you have to run 1.3.x version of LetosRemoteProject with 1.3.x version of DbAndroid plugin, and so on.

If you use `letosremote-release.aar` artifact fetched directly from Letos menu, you don't have to worry about this, as the plugin will always give you the correct version.
The compatibility is only relevant if you use the `org.letos:letosremote` dependency in your project from Maven Central.