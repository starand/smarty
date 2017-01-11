package org.zapto.safetylab.smarty;

import android.app.Activity;
import android.os.Handler;
import android.util.Log;
import android.view.View;


public class ConnectionEventHandler implements ConnectionHandler.EventListener {

    private SmartyApplication m_app;
    private Activity m_activity;
    private Handler m_mainHandler;

    private ButtonsView m_buttonsView;

    private byte m_lightsState;
    CommandExecutor m_executor;


    public ConnectionEventHandler(SmartyApplication app) {
        m_app = app;

        m_activity = app.getMainActivity();
        m_mainHandler = new Handler(m_activity.getApplicationContext().getMainLooper());
    }

    public void onConnect(CommandExecutor executor, String ipAddress, int port) {
        Log.i("Connection", "Connected to " + ipAddress + ":" + port);
        m_app.showToast("Connected");

        m_executor = executor;
    }

    public void onDisconnect() {
        Log.i("Connection", "Disconnected");
        m_app.showToast("Disconnected");
    }

    public void onError(final String errorMessage) {
        Log.e("ConnectionEvent", errorMessage);
    }

    public void onHandshakeResponse(int state, byte lights, int event_modes) {
        m_lightsState = lights;
        m_app.setEventModesBitset(event_modes);
    }

    public void onConfigUpdate(String config) {
        if (!m_app.parseConfig(config)) {
            return;
        }

        m_buttonsView = new ButtonsView(m_app.getConfig(), m_activity, m_executor);
        setContentView(m_buttonsView.getView());
        updateButtonColors(m_lightsState);
    }

    public void onMobileNotification(int desktop, int type, String params) {
        //Log.d("Packet", "MobileNotifiction: desktop - " + desktop + " Type - " +
        // type + " Param = " + params);
    }

    public void onLightNotification(byte state) {
        m_lightsState = state;
        updateButtonColors(m_lightsState);
    }

    private void setContentView(final View view) {
        m_mainHandler.post(new Runnable() {
            @Override
            public void run() {
                m_activity.setContentView(view);
            }
        });
    }

    private void updateButtonColors(final int state) {
        m_mainHandler.post(new Runnable() {
            @Override
            public void run() {
                m_buttonsView.updateButtonColors(state);
            }
        });
    }

    public void onModesNotification(int state) {
        m_app.setEventModesBitset(state);
    }
}
