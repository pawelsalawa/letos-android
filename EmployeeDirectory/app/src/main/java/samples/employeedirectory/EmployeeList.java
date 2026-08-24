package samples.employeedirectory;

import android.app.ListActivity;
import android.content.Intent;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;
import android.widget.ListAdapter;
import android.widget.SimpleCursorAdapter;
import android.widget.ListView;

import org.letos.letosremote.LetosService;

public class EmployeeList extends ListActivity {

	protected EditText searchText;
	protected SQLiteDatabase db;
	protected Cursor cursor;
	protected ListAdapter adapter;

    public void onCreate(Bundle savedInstanceState) {
    	super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        searchText = (EditText) findViewById(R.id.searchText);
    	db = (new DatabaseHelper(this)).getWritableDatabase();

        //LetosService.instance().setPassword("aaa");
        LetosService.instance().start(this);
        // ensure click listener is set (onClick in XML may not be resolved reliably on some devices)
        try {
            android.view.View btn = findViewById(R.id.searchButton);
            if (btn != null) {
                btn.setOnClickListener(new android.view.View.OnClickListener() {
                    @Override
                    public void onClick(android.view.View v) {
                        search(v);
                    }
                });
            } else {
            // no-op if button not found
            }
        } catch (Throwable t) {
            // ignore listener attachment errors
        }
    }

    @Override
    public void onDestroy() {
        LetosService.instance().stop();
        super.onDestroy();
    }
    
    public void onListItemClick(ListView parent, View view, int position, long id) {
    	Intent intent = new Intent(this, EmployeeDetails.class);
    	Cursor cursor = (Cursor) adapter.getItem(position);
    	intent.putExtra("EMPLOYEE_ID", cursor.getInt(cursor.getColumnIndex("_id")));
    	startActivity(intent);
    }
    
    public void search(View view) {
        // Simple search by full name
        String term = searchText.getText().toString();
        cursor = db.rawQuery("SELECT _id, firstName, lastName, title FROM employee WHERE firstName || ' ' || lastName LIKE ?", new String[]{"%" + term + "%"});
        adapter = new SimpleCursorAdapter(
                this,
                R.layout.employee_list_item,
                cursor,
                new String[] {"firstName", "lastName", "title"},
                new int[] {R.id.firstName, R.id.lastName, R.id.title},
                0);
        setListAdapter(adapter);
    }
    
}