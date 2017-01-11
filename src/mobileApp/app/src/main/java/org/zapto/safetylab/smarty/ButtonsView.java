package org.zapto.safetylab.smarty;

import android.app.Activity;
import android.content.Context;
import android.graphics.Color;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.GridLayout;


public class ButtonsView {

    private ClientConfig m_config;
    private Activity m_activity;
    private Context m_context;

    private CommandExecutor m_executor;

    private GridLayout m_gridLayout;
    private ButtonInfo[] m_buttons;

    enum LightState { OFF, ON }
    enum ButtonState { ENABLED, DISABLED }

    private static final int UPDATE_BUTTON_COLOR_SKIP_STATE = -1;


    public ButtonsView(ClientConfig config, Activity activity, CommandExecutor executor) {
        m_config = config;
        m_activity = activity;
        m_context = m_activity.getApplicationContext();
        m_executor = executor;

        createGridLayout();
    }

    public View getView() {
        return m_gridLayout;
    }

    private void createGridLayout() {
        m_gridLayout = new GridLayout(m_context);
        m_gridLayout.setLayoutParams(new ViewGroup.LayoutParams(
                GridLayout.LayoutParams.MATCH_PARENT,
                GridLayout.LayoutParams.MATCH_PARENT));
        m_gridLayout.setBackgroundColor(Color.BLACK);


        ConfigLightInfo[] lights = m_config.getLights();
        final int rowCount = (lights.length + 1) / 2;

        m_gridLayout.setColumnCount(2);
        m_gridLayout.setRowCount(rowCount);

        m_buttons = new ButtonInfo[lights.length];

        int index = 0;
        for (ConfigLightInfo configLightInfo : lights) {
            GridLayout.Spec row = GridLayout.spec(index / 2, 1.0f);
            GridLayout.Spec col = GridLayout.spec(index % 2, 1.0f);
            GridLayout.LayoutParams layout = new GridLayout.LayoutParams(row, col);
            layout.setMargins(5, 5, 5, 5);
            layout.setGravity(Gravity.FILL);

            m_buttons[index] = new ButtonInfo(configLightInfo.pin, configLightInfo.name);

            createButton(m_buttons[index], layout, index);
            ++index;
        }
    }

    private void createButton(ButtonInfo info, GridLayout.LayoutParams layout, final int index) {
        info.button = new Button(m_context);

        info.button.setText(info.name);
        info.button.setBackgroundColor(Color.rgb(45, 45, 45));

        final Room room = Room.forValue(m_buttons[index].pin - 1);

        info.button.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                final DeviceCommand cmd = m_buttons[index].lightState == LightState.OFF
                        ? DeviceCommand.TURN_ON : DeviceCommand.TURN_OFF;

                m_executor.executeCommand(cmd, room);
            }
        });

        info.button.setOnLongClickListener(new Button.OnLongClickListener() {
            @Override
            public boolean onLongClick(View v) {
                DeviceCommand cmd = null;
                if (m_buttons[index].buttonState == ButtonState.ENABLED) {
                    cmd = DeviceCommand.DISABLE_BUTTON;
                    m_buttons[index].buttonState = ButtonState.DISABLED;
                } else {
                    cmd = DeviceCommand.ENABLE_BUTTON;
                    m_buttons[index].buttonState = ButtonState.ENABLED;
                }

                m_executor.executeCommand(cmd, room);
                updateButtonColors(UPDATE_BUTTON_COLOR_SKIP_STATE);
                return true;
            }
        });

        m_gridLayout.addView(info.button, layout);
    }

    public void updateButtonColors(int state) {
        assert(m_buttons != null);

        for (int idx = 0; idx < m_buttons.length; ++idx) {
            assert(m_buttons[idx] != null);
            ButtonInfo info = m_buttons[idx];

            if (state != UPDATE_BUTTON_COLOR_SKIP_STATE) {
                boolean isOn = ((1 << (info.pin - 1)) & state) != 0;
                info.lightState = isOn ? LightState.ON : LightState.OFF;
            }

            int buttonColor = info.buttonState == ButtonState.DISABLED
                    ? (info.lightState == LightState.ON
                        ? Color.rgb(0xA8, 0, 0)
                        : Color.rgb(0x40, 0, 0))
                    : (info.lightState == LightState.ON
                        ? Color.rgb(0xB4, 0x6C, 0x16)
                        : Color.rgb(45, 45, 45));

            info.button.setBackgroundColor(buttonColor);
        }
    }
}
