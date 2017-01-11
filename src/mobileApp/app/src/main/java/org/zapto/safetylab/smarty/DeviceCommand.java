package org.zapto.safetylab.smarty;


public enum DeviceCommand {
    STATUS(0), TURN_ON(1), TURN_OFF(2), TURN_ALL(3), TURN_EQUAL(4),
    DISABLE_BUTTON(5), ENABLE_BUTTON(6);

    private final int m_value;

    DeviceCommand(int v) {
        m_value = v;
    }

    public byte value() {
        return (byte) m_value;
    }


    @Override
    public String toString() {
        switch (m_value) {
        case 0:
            return "Status";
        case 1:
            return "TurnOn";
        case 2:
            return "TurnOff";
        case 3:
            return "TurnAll";
        case 4:
            return "TurnEqual";
        case 5:
            return "DisableButton";
        case 6:
            return "EnableButton";
        default:
            return "ERROR: DeviceCommand.toString() method was not updated for " + m_value;
        }
    }
}