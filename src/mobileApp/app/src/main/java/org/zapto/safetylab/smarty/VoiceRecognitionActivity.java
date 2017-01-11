package org.zapto.safetylab.smarty;

import android.content.Intent;
import android.speech.RecognizerIntent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.ArrayAdapter;
import android.widget.ListView;

import java.util.ArrayList;
import java.util.Locale;


public class VoiceRecognitionActivity extends AppCompatActivity {

    static final int VOICE_RECOGNISE_REQUEST = 1001;

    ListView m_lvVoice;

    private VoiceCommandParser m_voiceCommandParser;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_voice_recognition);

        m_lvVoice = (ListView) findViewById(R.id.voiceListView);
        SmartyApplication app = (SmartyApplication) this.getApplication();
        m_voiceCommandParser = app.getVoiceCommandParser();

        startVoiceRecognition();
    }

    private void startVoiceRecognition() {
        Intent intent = new Intent(RecognizerIntent.ACTION_RECOGNIZE_SPEECH);

        intent.putExtra(RecognizerIntent.EXTRA_LANGUAGE_MODEL, RecognizerIntent.LANGUAGE_MODEL_FREE_FORM);
        intent.putExtra(RecognizerIntent.EXTRA_PROMPT, "Say SMARTY command:");
        intent.putExtra(RecognizerIntent.EXTRA_LANGUAGE, Locale.getDefault().getLanguage());

        startActivityForResult(intent, VOICE_RECOGNISE_REQUEST);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == VOICE_RECOGNISE_REQUEST && resultCode == RESULT_OK) {
            ArrayList<String> results = data.getStringArrayListExtra(RecognizerIntent.EXTRA_RESULTS);
            m_lvVoice.setAdapter(new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, results));

            if (!results.isEmpty()) {
                String query = results.get(0);
                m_voiceCommandParser.parseQueryString(query);
            }
        }

        super.onActivityResult(requestCode, resultCode, data);
        finish();
    }
}
