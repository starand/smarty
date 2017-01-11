package org.zapto.safetylab.smarty;

import android.widget.Button;


public class ButtonInfo {
    int pin;
    String name;

    Button button;
    ButtonsView.LightState lightState;
    ButtonsView.ButtonState buttonState;

    ButtonInfo(int p, String n) {
        pin = p;
        name = n;
        lightState = ButtonsView.LightState.OFF;
        buttonState = ButtonsView.ButtonState.ENABLED;
    }
}
