package org.zapto.safetylab.smarty;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;


public class MainActivity extends AppCompatActivity {

    private SmartyApplication m_app;
    private VoiceCommandParser m_voiceCommandParser;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        m_app = (SmartyApplication) this.getApplication();
        if (!m_app.initApp(this, "safetylab.zapto.org", 25)) {
            Log.e("APP", "Cannot init application");
            finish();
        }

        m_voiceCommandParser = m_app.getVoiceCommandParser();

        startVoiceRecognitionIfNeeded();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        new MenuInflater(this).inflate(R.menu.menu_main_activity, menu);
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.menu_modes:
                Intent intent = new Intent(this, ModesActivity.class);
                startActivity(intent);
                break;
            case R.id.menu_exit:
                finish();
                break;
        }

        return super.onOptionsItemSelected(item);
    }

    private void startVoiceRecognitionIfNeeded() {
        Intent intent = getIntent();
        if (intent != null) {
            Bundle bundle = intent.getExtras();
            if (bundle != null) {

                String query = intent.getStringExtra("query");
                if (query != null) {
                    m_voiceCommandParser.parseQueryString(query);
                } else {
                    String referrer = bundle.getString("android.intent.extra.REFERRER_NAME");
                    if (referrer != null &&
                            referrer.contains("com.google.android.googlequicksearchbox")) {
                        //Log.d("INTENT", referrer);
                        Intent voiceIntent = new Intent(this, VoiceRecognitionActivity.class);
                        startActivity(voiceIntent);
                    }
                }
            }
        }
    }
}
