package com.android.regexptester;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.os.Bundle;
import java.util.regex.Pattern;
import java.util.regex.Matcher;
import android.content.SharedPreferences;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.CheckBox;
import android.widget.Spinner;
import android.widget.TabHost;

public class OnLaunch extends Activity {
	private SharedPreferences preferences;
	private static EditText text1;
	private static EditText regexp;
	private static Button btn1;
	private static CheckBox iscase;
	private static CheckBox iscomm;
	private static CheckBox ismulti;
	private static CheckBox isuni;
	private static CheckBox isunix;
	private static Spinner spin;
	private ArrayAdapter<String> adapter;
	private int selectedItem = 0;
	private String DialogMessage = "";
	private int dialogId = 0;
	private final static int MIN = 0;
	private final static int MAX = 20;
	
	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
		btn1 = (Button) findViewById(R.id.RunTest);
		text1 = (EditText) findViewById(R.id.RegTextEdit);
		regexp = (EditText) findViewById(R.id.Regexp);
		iscase = (CheckBox) findViewById(R.id.isCase);
		iscomm = (CheckBox) findViewById(R.id.isComm);
		ismulti = (CheckBox) findViewById(R.id.isMulti);
		isuni = (CheckBox) findViewById(R.id.isUnicode);
		isunix = (CheckBox) findViewById(R.id.isUnix);
		spin = (Spinner) findViewById(R.id.groups);
		//setup tabhost
		TabHost tab_host = (TabHost) findViewById(R.id.mytabhost);
		tab_host.setup();
		tab_host.addTab(tab_host.newTabSpec("tab1").setContent(R.id.tab1).setIndicator(getString(R.string.tab1Name)));
		tab_host.addTab(tab_host.newTabSpec("tab2").setContent(R.id.tab2).setIndicator(getString(R.string.tab2Name)));
		//
		String numers[] = new String[MAX];
		for(int i=MIN; i<MAX; i++){
			numers[i]= Integer.toString(i);
		}
		adapter = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_item, numers);
		adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
		spin.setAdapter(adapter);
		spin.setOnItemSelectedListener(itemSelected);
		btn1.setOnClickListener(btnDoneOnClick);
		preferences = getPreferences(0);
		readPrefs();
	}
	@Override 
	protected void onStop(){
		super.onStop();
		writePrefs();
	}
	protected void onPause() {
		super.onPause();
		writePrefs();
	}
	protected void onDestroy(){
		super.onDestroy();
		writePrefs();
	}
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
		DialogMessage = getString(R.string.AboutMessage);
		dialogId = 3;
		showDialog(dialogId);
	}
	private void onQuit(){
		finish();
	}
	private final Spinner.OnItemSelectedListener itemSelected = new Spinner.OnItemSelectedListener(){
		@Override
		public void onItemSelected(AdapterView<?> adapterView, View v, int pos,
				long id) {
			selectedItem = pos;
			
		}
		@Override
		public void onNothingSelected(AdapterView<?> adapterView) {
			selectedItem = 0;
		}
	};
	protected Dialog onCreateDialog(int i){
		int dialogTitle = R.string.DialogTitle;
		DialogInterface.OnClickListener doNothing = new DialogInterface.OnClickListener()
		{
			public void onClick(DialogInterface dialog, int whichButton){
				if(whichButton == -3){
					dismissDialog(dialogId);
					removeDialog(dialogId);
				}
			}
		};
		switch (i){
			case 1:
				dialogTitle = R.string.DialogTitle;
				break;
			case 2:
				dialogTitle = R.string.Error03;
				break;
			case 3:
				dialogTitle = R.string.AboutTitle;
				break;
		}
		return new AlertDialog.Builder(this)
			.setTitle(dialogTitle)
			.setMessage(DialogMessage)
			.setNeutralButton("Ok", doNothing)
			.create();
	}
	private final Button.OnClickListener btnDoneOnClick = new Button.OnClickListener() {
		public void onClick(View v) {
			onCount();
		}
	};
	
	private void onCount(){
		String RegExp = "", Text = "";
		StringBuffer RegexpBuff = new StringBuffer();
		if(regexp.length()>0){
			RegExp = regexp.getText().toString();
		}else{
			RegExp="";
		}
		if(text1.length()>0){
			Text = text1.getText().toString();
		}else{
			Text="";
		}
		if(RegExp!=""){
			if(iscase.isChecked()){
				RegexpBuff.append("(?i)");
			}
			if(iscomm.isChecked()){
				RegexpBuff.append("(?x)");
			}
			if(ismulti.isChecked()){
				RegexpBuff.append("(?m)");
			}
			if(isuni.isChecked()){
				RegexpBuff.append("(?u)");
			}
			if(isunix.isChecked()){
				RegexpBuff.append("(?d)");
			}
			RegexpBuff.append(RegExp);
			CheckRegexp(RegexpBuff.toString(), Text);
		}
	}
	
	private void CheckRegexp(String RegExp, String Text){
		StringBuffer matches = new StringBuffer();
		Integer Group = selectedItem;
		try{
			Pattern p =Pattern.compile(RegExp);
			Matcher m = p.matcher(Text);
			if(Group != null && (Group <= m.groupCount())){
				int count = 0;
				while(m.find()){
					count++;
					if(!m.group(Group).contains("\n")){
						matches.append(getString(R.string.matchName)+Integer.toString(count)+":\n"+ m.group(Group)+"\n");
					}else{
						matches.append(getString(R.string.matchName)+Integer.toString(count)+":\n"+ m.group(Group));
					}
				}
				if(matches.length()>0){
					DialogMessage = matches.toString();
					dialogId = 1;
					showDialog(dialogId);
				}
			}else if(Group >= m.groupCount()){
				String message = getString(R.string.Error01)+Integer.toString(Group) + getString(R.string.Error02) + Integer.toString(m.groupCount());
				DialogMessage = message;
				dialogId = 2;
				showDialog(dialogId);
			}
		}catch(Exception e){
			DialogMessage = e.toString();
			dialogId = 2;
			showDialog(dialogId);
		}
	}
	
	private void readPrefs(){
		String text = preferences.getString("main_text", "Text");
		String re = preferences.getString("regexp", ".*");
		Boolean Case = preferences.getBoolean("iscase", false);
		Boolean Comm = preferences.getBoolean("iscomm", false);
		Boolean Multi = preferences.getBoolean("ismulti", false);
		Boolean UNI = preferences.getBoolean("isuni", false);
		Boolean Unix = preferences.getBoolean("isunix", false);
		if(re != ""){
			regexp.setText(re);
		}
		if (text != ""){
			text1.setText(text);
		}
		iscase.setChecked(Case);
		iscomm.setChecked(Comm);
		ismulti.setChecked(Multi);
		isuni.setChecked(UNI);
		isunix.setChecked(Unix);
	}
	private void writePrefs(){
		SharedPreferences.Editor editor = preferences.edit();
		if(regexp.length()>0){
			editor.putString("main_text", text1.getText().toString());
		}
		if(text1.length()>0){
			editor.putString("regexp", regexp.getText().toString());
		}
		editor.putBoolean("iscase", iscase.isChecked());
		editor.putBoolean("iscomm", iscomm.isChecked());
		editor.putBoolean("ismulti", ismulti.isChecked());
		editor.putBoolean("isuni", isuni.isChecked());
		editor.putBoolean("isunix", isunix.isChecked());
		editor.commit();
	}
}