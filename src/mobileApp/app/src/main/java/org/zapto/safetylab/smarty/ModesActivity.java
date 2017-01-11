package org.zapto.safetylab.smarty;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.View;
import android.widget.CheckBox;
import android.widget.LinearLayout;

public class ModesActivity extends Activity {

    private SmartyApplication m_app;
    private ClientConfig m_config;
    private CommandExecutor m_executor;

    private LinearLayout m_linearLayout;

    private ConfigModeInfo[] m_modes;
    private CheckBox[] m_checkBoxes;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_modes);

        m_linearLayout = (LinearLayout) findViewById(R.id.linearLayout);

        m_app = (SmartyApplication) this.getApplication();

        m_executor = m_app.getCommandExecutor();
        m_config = m_app.getConfig();
        assert(m_config != null);
        m_modes = m_config.getModes();
        assert(m_modes != null);

        createModesCheckList();
    }

    private void createModesCheckList() {
        m_checkBoxes = new CheckBox[m_modes.length];
        int eventModesBitset = m_app.getEventModesBitset();

        for (int idx = 0; idx < m_modes.length; ++idx) {
            ConfigModeInfo mode = m_modes[idx];

            CheckBox checkBox = new CheckBox(this);
            checkBox.setText(mode.name);

            boolean checked = (eventModesBitset & (1 << idx)) != 0;
            checkBox.setChecked(checked);

            final int index = idx;
            checkBox.setOnClickListener(new CheckBox.OnClickListener() {
                @Override
                public void onClick(View v) {
                    int modesBitest = m_app.getEventModesBitset();
                    modesBitest ^= 1 << index;
                    m_app.setEventModesBitset(modesBitest);

                    updateModes();
                }
            });

            m_linearLayout.addView(checkBox);
            m_checkBoxes[idx] = checkBox;
        }
    }

    void updateModesCheckList(final int modes) {
        new Handler(m_app.getMainLooper()).post(new Runnable() {
            @Override
            public void run() {
                for (int idx = 0; idx < m_modes.length; ++idx) {
                    boolean checked = (modes & (1 << idx)) != 0;
                    m_checkBoxes[idx].setChecked(checked);
                }
            }
        });
    }

    private void updateModes() {
        m_executor.requestModesUpdate(m_app.getEventModesBitset());
    }

    @Override
    protected void onResume() {
        super.onResume();
        m_app.setModesActivityForeground(this);
    }

    @Override
    protected void onPause() {
        m_app.setModesActivityForeground(null);
        super.onPause();
    }
}
