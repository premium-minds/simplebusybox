package org.galexander.busybox;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;
import java.io.InputStream;


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
		check_status();
	}

	public void install_clicked(View v) {
		(new Thread() { public void run() {
			do_install();
		} }).start();
	}

	private void do_install() {
		try {
			InputStream is = getResources().openRawResource(R.raw.busybox);
			if (is == null) {
				update_status("Error opening busybox resource.");
			} else {
				try {
					native_install(get_path(), is);
				} finally {
					is.close();
				}
			}
		} catch (Exception e) {
			update_status("Exception while unpacking: " +
						e.getMessage());
		}
	}

	private void check_status() {
		(new Thread() { public void run() {
			determine_status(get_path());
		} }).start();
	}

	private void update_status(final String s) {
		runOnUiThread(new Runnable() { public void run() {
			if (status_text != null) {
				status_text.setText(s);
			}
		} });
	}

	private String get_path() {
		return getFilesDir().toString();
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
	private native void native_install(String path, InputStream is);
	static {
		System.loadLibrary("sbb-jni");
	}
}
