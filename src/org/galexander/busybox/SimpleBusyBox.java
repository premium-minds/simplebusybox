package org.galexander.busybox;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;
import java.io.File;


public class SimpleBusyBox extends Activity
{
	private TextView status_text = null;

	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
		status_text = (TextView)findViewById(R.id.status);
	}

	public void onResume() {
		super.onResume();
		(new Thread() { public void run() {
			determine_status(getFilesDir().toString());
		} }).start();
	}

	public void install_clicked(View v) {
	}

	private void update_status(final String s) {
		runOnUiThread(new Runnable() { public void run() {
			if (status_text != null) {
				status_text.setText(s);
			}
		} });
	}

	private int get_version() {
		int ret = 0;
		try {
			ret = getPackageManager()
				.getPackageInfo(getPackageName(), 0)
				.versionCode;
		} catch (Exception e) { }
		return ret;
	}


	private native void determine_status(String path);
	static {
		System.loadLibrary("sbb-jni");
	}
}
