package org.zapto.safetylab.smarty;

import android.app.Activity;
import android.app.Application;
import android.content.Context;
import android.os.Handler;
import android.util.Log;
import android.widget.Toast;


public class SmartyApplication extends Application {

    private ClientConfig m_config;

    private ConnectionHandler m_connectionHandler;
    private ConnectionEventHandler m_connectionEventHandler;
    private VoiceCommandParser m_voiceCommandParser;

    private Activity m_mainActivity;
    private Context m_context;
    private Handler m_mainHandler;

    private int m_eventModes;
    private ModesActivity m_modesActivityForeground;

    public boolean initApp(Activity activity, String host, int port) {

        m_mainActivity = activity;
        m_context = activity.getApplicationContext();
        m_mainHandler = new Handler(m_context.getMainLooper());
        m_connectionHandler = new ConnectionHandler(host, port);

        m_connectionEventHandler = new ConnectionEventHandler(this);
        m_connectionHandler.addListener(m_connectionEventHandler);
        m_connectionHandler.start();

        m_voiceCommandParser = new VoiceCommandParser(this);

        return true;
    }

    public boolean connected() {
        return m_connectionHandler != null && m_connectionHandler.connected();
    }

    public CommandExecutor getCommandExecutor() {
        return m_connectionHandler;
    }

    public VoiceCommandParser getVoiceCommandParser() {
        return m_voiceCommandParser;
    }

    public void showToast(final String message) {
        m_mainHandler.post(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(m_context, message, Toast.LENGTH_SHORT).show();
            }
        });
    }

    public Activity getMainActivity() {
        return m_mainActivity;
    }

    public boolean parseConfig(String config) {
        Log.d("Config", config);

        m_config = new ClientConfig(config);
        if (!m_config.parse()) {
            // report error. Fail ?
            showToast("Cannot parse config");
            return false;
        }

        return true;
    }

    public ClientConfig getConfig() {
        return m_config;
    }

    public int getEventModesBitset() {
        return m_eventModes;
    }

    public void setEventModesBitset(int modes) {
        m_eventModes = modes;
        Log.d("EventModes", "update: " + m_eventModes);

        if (m_modesActivityForeground != null) {
            m_modesActivityForeground.updateModesCheckList(m_eventModes);
        }
    }

    public void setModesActivityForeground(ModesActivity activity) {
        m_modesActivityForeground = activity;
    }
}
