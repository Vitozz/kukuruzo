package com.android.interpolation;

import android.app.Activity;
import android.app.Dialog;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Bundle;
import android.view.Gravity;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;
import android.content.SharedPreferences;

public class StartScreen extends Activity implements OnClickListener{
	/** Called when the activity is first created. */
	private SharedPreferences data;
	private Button mButton;
	private EditText x1edit;
	private EditText x2edit;
	private EditText xTedit;
	private EditText y1edit;
	private EditText y2edit;
	private EditText yTedit;
	
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
		mButton = (Button)findViewById(R.id.Button01);
		mButton.setOnClickListener(this);
		x1edit = (EditText)findViewById(R.id.X1);
		x2edit = (EditText)findViewById(R.id.X2);
		xTedit = (EditText)findViewById(R.id.Xtr);
		y1edit = (EditText)findViewById(R.id.Y1);
		y2edit = (EditText)findViewById(R.id.Y2);
		yTedit = (EditText)findViewById(R.id.Answ);
		data = getPreferences(0);
		readData();
		
	}
	//Menu
	public boolean onCreateOptionsMenu(Menu menu) {
		MenuInflater inflater = getMenuInflater();
		inflater.inflate(R.menu.menu, menu);
		return true;
	}
	public boolean onOptionsItemSelected(MenuItem item) {
		// Handle item selection
		switch (item.getItemId()) {
		case R.id.AboutItem:
			onAbout();
			return true;
		case R.id.ExitItem:
			onQuit();
			return true;
		case R.id.CountItem:
			onCount();
			return true;
		default:
			return super.onOptionsItemSelected(item);
		}
	}
	private void onAbout(){
		showDialog(1);
		return;
	}
	private void onQuit(){
		finish();
	}
    protected void onStop(){
        super.onStop();
        writeData();
    }
    protected void onPause() {
        super.onPause();
        writeData();
    }
    protected void onDestroy(){
    	super.onDestroy();
    	writeData();
    }
	//Dialog
	protected Dialog onCreateDialog(int i){
		DialogInterface.OnClickListener doNothing = new DialogInterface.OnClickListener()
		{
			public void onClick(DialogInterface dialog, int whichButton){
			}
		};
		int alertMessage;
		alertMessage = R.string.about_text;
		return new AlertDialog.Builder(this)
			.setTitle(R.string.about_title)
			.setMessage(alertMessage)
			.setNeutralButton(R.string.about_ok, doNothing)
			.create();
		}
	//main
	public void onClick(View v) {
		onCount();
	}
	private void onCount(){
		float[] data = {
				Float.parseFloat(x1edit.getText().toString()),
				Float.parseFloat(x2edit.getText().toString()),
				Float.parseFloat(xTedit.getText().toString()),
				Float.parseFloat(y1edit.getText().toString()),
				Float.parseFloat(y2edit.getText().toString())
		};
		yTedit.setText(String.valueOf(Interpolate(data)));
	}
	private void getToast(String text){
		Context context = getApplicationContext();
		CharSequence message = text;
		int duration = Toast.LENGTH_SHORT;
		Toast toast = Toast.makeText(context, message, duration);
		toast.setGravity(Gravity.CENTER, 0, 0);
		toast.show();
	}
	private float Interpolate(float[] args){
		float answer = ((args[2]-args[0])*(args[4]-args[3])/(args[1]-args[0])+args[3]);
		getToast(getString(R.string.is_counted) + String.valueOf(answer));
		return answer;
	}
	private void writeData(){
        SharedPreferences.Editor editor = data.edit();
        String x1 = x1edit.getText().toString();
        String x2 = x2edit.getText().toString();
        String xt = xTedit.getText().toString();
        String y1 = y1edit.getText().toString();
		String y2 = y2edit.getText().toString();
        if(x1.length()>0){
        	editor.putString("x1", x1);
        }
        if(x2.length()>0){
        	editor.putString("x2", x2);
        }
        if(xt.length()>0){
        	editor.putString("xt", xt);
        }
        if(y1.length()>0){
        	editor.putString("y1", y1);
        }
        if(y2.length()>0){
        	editor.putString("y2", y2);
        }
        editor.commit();
    }
	private void readData(){
		x1edit.setText(data.getString("x1", "0"));
		x2edit.setText(data.getString("x2", "0"));
		xTedit.setText(data.getString("xt", "0"));
		y1edit.setText(data.getString("y1", "0"));
		y2edit.setText(data.getString("y2", "0"));
	}
}