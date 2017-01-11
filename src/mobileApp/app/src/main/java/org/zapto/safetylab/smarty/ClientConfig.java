package org.zapto.safetylab.smarty;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;


public class ClientConfig {

    private String m_configString;
    private JSONObject m_jsonConfig;

    private ConfigLightInfo[] m_lights;
    private ConfigModeInfo[] m_modes;


    public ClientConfig(String config) {
        m_configString = config;
    }

    boolean parse() {
        try {
            m_jsonConfig = new JSONObject(m_configString);
            if (!parseLights()) {
                return false;
            }

            if (!parseModes()) {
                return false;
            }
        } catch (JSONException e) {
            e.printStackTrace();
            return false;
        }
        return true;
    }

    private boolean parseLights() {
        assert(m_jsonConfig != null);

        try {
            final JSONArray lightsNode = m_jsonConfig.getJSONArray("lights");

            final int len = lightsNode.length();
            m_lights = new ConfigLightInfo[len];

            for (int i = 0; i < len; ++i) {
                JSONObject lightNode = lightsNode.getJSONObject(i);

                int pin = lightNode.getInt("pin");
                String name = lightNode.getString("name");

                m_lights[i] = new ConfigLightInfo(pin, name);
            }
        } catch (JSONException e) {
            e.printStackTrace();
            return false;
        }

        return true;
    }

    public ConfigLightInfo[] getLights() {
        return m_lights;
    }

    private boolean parseModes() {
        assert(m_jsonConfig != null);

        try {
            final JSONArray modesNode = m_jsonConfig.getJSONArray("modes");

            final int len = modesNode.length();
            m_modes = new ConfigModeInfo[len];

            for (int i = 0; i < len; ++i) {
                JSONObject modeNode = modesNode.getJSONObject(i);

                String name = modeNode.getString("name");

                m_modes[i] = new ConfigModeInfo(name);
            }
        } catch (JSONException e) {
            e.printStackTrace();
            return false;
        }

        return true;
    }

    public ConfigModeInfo[] getModes() {
        return m_modes;
    }
}
