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
        android.util.Log.d("EDebug","searchText found: " + (searchText!=null));
        searchText.post(new Runnable() {
            @Override
            public void run() {
                int[] loc = new int[2];
                searchText.getLocationOnScreen(loc);
                android.util.Log.d("EDebug", String.format("searchText vis=%d w=%d h=%d x=%d y=%d bg=%s",
                        searchText.getVisibility(), searchText.getWidth(), searchText.getHeight(), loc[0], loc[1],
                        searchText.getBackground()!=null ? "yes" : "no"));
            }
        });
    	db = (new DatabaseHelper(this)).getWritableDatabase();

        //LetosService.instance().setPassword("aaa");
        LetosService.instance().start(this);
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
    	// || is the concatenation operation in SQLite
		cursor = db.rawQuery("SELECT _id, firstName, lastName, title FROM employee WHERE firstName || ' ' || lastName LIKE ?", 
						new String[]{"%" + searchText.getText().toString() + "%"});
		adapter = new SimpleCursorAdapter(
				this, 
				R.layout.employee_list_item, 
				cursor, 
				new String[] {"firstName", "lastName", "title"}, 
				new int[] {R.id.firstName, R.id.lastName, R.id.title});
		setListAdapter(adapter);
    }
    
}