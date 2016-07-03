package org.galexander.busybox;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import java.io.File;


public class SimpleBusyBox extends Activity
{
	private TextView status_text = null;
	private TextView install_button = null;

	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
		status_text = (TextView)findViewById(R.id.status);
		install_button = (Button)findViewById(R.id.install);
	}

	public void onResume() {
		super.onResume();
		(new Thread() { public void run() {
			determine_status(getFilesDir().toString());
		} }).start();
	}

	public void install_clicked(View v) {
	}

	public void update_status(final String s) {
		runOnUiThread(new Runnable() { public void run() {
			if (status_text != null) {
				status_text.setText(s);
			}
		} });
	}

	private native void determine_status(String path);

	static {
		System.loadLibrary("sbb-jni");
	}
}
