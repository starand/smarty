package org.zapto.safetylab.smarty;


public enum Room {
    KITCHEN(0), BATHROOM(1), TOILET(2), CORRIDOR(3), HALL(4), LAMP(5);

    private int m_value;

    Room(int value) {
        m_value = value;
    }

    public byte value() {
        return (byte) m_value;
    }

    @Override
    public String toString() {
        switch (m_value) {
            case 0:
                return "Kitchen";
            case 1:
                return "Bathroom";
            case 2:
                return "Toilet";
            case 3:
                return "Corridor";
            case 4:
                return "Hall";
            case 5:
                return "Lamp";
            default:
                return "ERROR: Room.toString() method was not updated for " + m_value;
        }
    }

    static Room forValue(int val) {
        return Room.values()[val];
    }
}
