package $CONFIG_PROJECT_PACKAGE;

import android.app.Activity;
import android.os.Bundle;

public class MainActivity extends Activity {
	public native void createModule();
	public native void destroyModule();

	static {
		System.loadLibrary("$CONFIG_PROJECT_NAME");
	}

	@Override protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		createModule();
	}

	@Override protected void onDestroy() {
		super.onDestroy();
		destroyModule();
	}
}
