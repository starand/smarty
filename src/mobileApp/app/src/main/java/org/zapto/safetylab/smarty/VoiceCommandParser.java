package org.zapto.safetylab.smarty;


import android.speech.tts.TextToSpeech;
import android.util.Log;

import java.util.HashSet;
import java.util.Locale;
import java.util.Set;


public class VoiceCommandParser {

    private SmartyApplication m_app;
    private CommandExecutor m_executor;
    private TextToSpeech m_speaker;


    VoiceCommandParser(SmartyApplication app) {
        m_app = app;
        m_executor = m_app.getCommandExecutor();
    }


    private void onErrorMessage(final String error) {
        Log.e("COMMAND", error);

        m_app.showToast(error);

        m_speaker = new TextToSpeech(m_app.getApplicationContext(),
            new TextToSpeech.OnInitListener() {
                @Override
                public void onInit(int status) {
                    if(status != TextToSpeech.ERROR) {
                        m_speaker.setLanguage(Locale.UK);
                        m_speaker.speak(error, TextToSpeech.QUEUE_ADD, null, "uniq_id");
                        m_speaker.stop();
                        m_speaker.shutdown();
                    } else {
                        Log.d("TTS", "Status = " + status );
                    }
                }
            });
    }

    void parseQueryString(String command) {
        command = command.toLowerCase(Locale.getDefault());
        Log.d("VOICE COMMAND", command);

        DeviceCommand dev_cmd = DeviceCommand.STATUS;
        Set<Room> roomNames = null;

        if (command.contains("включи") || command.contains("включити")
                || command.contains("turn on")) {
            dev_cmd = DeviceCommand.TURN_ON;
            roomNames = parseRoomName(command);
        } else if (command.contains("виключи") || command.contains("виключити")
                || command.contains("turn off")) {
            dev_cmd = DeviceCommand.TURN_OFF;
            roomNames = parseRoomName(command);
        } else if (command.contains("заблокуй") || command.contains("заблокувати")
                || command.contains("block")) {
            dev_cmd = DeviceCommand.DISABLE_BUTTON;
            roomNames = parseRoomName(command);
        } else if (command.contains("розблокуй") || command.contains("розблокувати")
                || command.contains("unblock")) {
            dev_cmd = DeviceCommand.ENABLE_BUTTON;
            roomNames = parseRoomName(command);
        }

        if (dev_cmd != DeviceCommand.STATUS && roomNames != null) {
            executeCommand(dev_cmd, roomNames);
        } else {
            onErrorMessage("Incorrect command or room names");
        }
    }

    private Set<Room> parseRoomName(String command) {
        Set<Room> result = new HashSet<>();

        if (command.contains("кухня") || command.contains("кухні")
                || command.contains("kitchen")) {
            result.add(Room.KITCHEN);
        }

        if (command.contains("ванні") || command.contains("ванну") || command.contains("ванній")
                || command.contains("bathroom")) {
            result.add(Room.BATHROOM);
        }

        if (command.contains("туалет") || command.contains("туалеті")
                || command.contains("toilet")) {
            result.add(Room.TOILET);
        }

        if (command.contains("коридор") || command.contains("коридорі")
                || command.contains("corridor")) {
            result.add(Room.CORRIDOR);
        }

        if (command.contains("зал") || command.contains("залі") || command.contains("кімнаті")
                || command.contains("hall")) {
            result.add(Room.HALL);
        }

        if (command.contains("лампу") || command.contains("лампа")
                || command.contains("lamp")) {
            result.add(Room.LAMP);
        }

        return result;
    }

    private void executeCommand(DeviceCommand cmd, Set<Room> rooms) {
        try {
            int time_left = 1000;
            final int iteration_time = 100;

            while (!m_app.connected() && time_left > 0) {
                Thread.sleep(iteration_time);
                time_left -= iteration_time;
            }
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        if (!m_app.connected()) {
            onErrorMessage("No connection");
            return;
        }

        for (Room room : rooms) {
            m_executor.executeCommand(cmd, room);
        }
    }
}
