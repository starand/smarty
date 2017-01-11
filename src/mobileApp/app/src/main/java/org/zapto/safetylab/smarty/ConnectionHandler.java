package org.zapto.safetylab.smarty;

import android.util.Log;

import java.io.*;
import java.net.*;
import java.util.ArrayList;
import java.util.List;


interface CommandExecutor {
    boolean executeCommand(byte cmd, byte param);
    boolean executeCommand(DeviceCommand cmd, Room room);

    boolean requestModesUpdate(int state);
}

public class ConnectionHandler extends Thread implements CommandExecutor {

	private final String m_ipAddress;
	private final int m_port;

	private Socket socket;
	private DataOutputStream ostream;
    private DataInputStream istream;

    boolean m_isStopping = false;
    boolean m_connected = false;

    interface EventListener {
        void onConnect(CommandExecutor executor, String ipAddress, int port);
        void onDisconnect();

        void onError(final String errorMessage);

        void onHandshakeResponse(int state, byte lights, int event_modes);
        void onConfigUpdate(String config);

        void onMobileNotification(int desktop, int type, String params);
        void onLightNotification(byte state);
        void onModesNotification(int state);
    }

    private List<EventListener> m_listeners = new ArrayList<>();


	public ConnectionHandler(String ipAddress, int port) {
        this.m_ipAddress = ipAddress;
        this.m_port = port;
    }

    public void finish() {
        m_isStopping = true;
    }

    synchronized boolean addListener(EventListener listener) {
        if (m_listeners.contains(listener)) {
            return false;
        }

        m_listeners.add(listener);
        return true;
    }

    synchronized void removeListener(EventListener listener) {
        m_listeners.remove(listener);
    }

    synchronized void onError(String errorMessage) {
        for (EventListener listener : m_listeners) {
            listener.onError(errorMessage);
        }
    }

	private boolean connect() {
		try {
            socket = new Socket(InetAddress.getByName(m_ipAddress), m_port);
			ostream = new DataOutputStream(socket.getOutputStream());
            istream = new DataInputStream(socket.getInputStream());
		} catch (IOException e) {
            StringWriter errors = new StringWriter();
            e.printStackTrace(new PrintWriter(errors));
            onError(errors.toString());
			return false;
		}

		return true;
	}

	private void disconnect() {
		m_isStopping = true;

		Packets.makeCommandDisconnect().send(ostream);

		try {
			socket.close();
		} catch (IOException e) {
			//e.printStackTrace();
		}

		ostream = null;
		socket = null;
	}

	private boolean makeHandshake(String password) {
		return Packets.makeHandshakeRequest("TestPassword").send(ostream);
	}

	private boolean requestConfigUpdate() {
        return Packets.makeConfigUpdateRequest().send(ostream);
	}

	public boolean executeCommand(byte cmd, byte param) {
        Log.d("COMMAND", "Run " + cmd + " with param " + param);

		return Packets.makeCommandRequest(cmd, param).send(ostream);
	}

    public boolean executeCommand(DeviceCommand cmd, Room room) {
        Log.d("COMMAND", "Run " + cmd + " " + room + " command");

        byte param = (byte)(1 << room.value());
        return Packets.makeCommandRequest(cmd.value(), param).send(ostream);
    }

    public void run() {
        while (!m_isStopping) {
            while (!connect()) {
                onError("Cannot connect to " + m_ipAddress + ":" + m_port);

                try {
                    Thread.sleep(5000);
                } catch (InterruptedException e) {
                    StringWriter errors = new StringWriter();
                    e.printStackTrace(new PrintWriter(errors));
                    onError(errors.toString());
                }
            }

            if (!makeHandshake("TestPassword")) {
                onError("Cannot make handshake");
                return;
            }

            m_connected = true;

            synchronized (m_listeners) {
                for (EventListener listener : m_listeners) {
                    listener.onConnect(this, m_ipAddress, m_port);
                }
            }

            if (!requestConfigUpdate()) {
                onError("Cannot send config update request");
                return;
            }

            do_run();

            m_connected = false;

            synchronized (m_listeners) {
                for (EventListener listener : m_listeners) {
                    listener.onDisconnect();
                }
            }
        }

        if (m_isStopping && socket != null) {
            disconnect();
        }
    }

    public boolean connected() {
        return m_connected;
    }

    private boolean do_run() {
        try {
            while (!m_isStopping) {
                int magic = PacketBase.recvInt(istream);
                int len = PacketBase.recvInt(istream);

                //Log.d("Packet", "Len: " + len + ", Magic: " + magic);

                switch (magic) {
                    case Packets.MOBILE_HANDSHAKE_RESPONSE:
                        Packets.HandshakeResponse hs_res = Packets.makeHandshakeResponse();
                        if (!hs_res.recv(istream)) {
                            onError("Cannot receive hs_req packet");
                            return false;
                        }
                        onHandshakeResponse(hs_res);
                        break;
                    case Packets.CONFIG_UPDATE_RESPONSE:
                        Packets.ConfigUpdateResponse config_res = Packets.makeConfigUpdateResponse();
                        if (!config_res.recv(istream)) {
                            onError("Cannot receive config_req packet");
                            return false;
                        }
                        onConfigUpdateResponse(config_res);
                        break;
                    case Packets.MOBILE_NOTIFICATION:
                        Packets.MobileNotification mobile_notification = Packets.makeMobileNotification();
                        if (!mobile_notification.recv(istream)) {
                            onError("Cannot receive mobile_notification packet");
                            return false;
                        }
                        onMobileNotification(mobile_notification);
                        break;
                    case Packets.MOBILE_HEARTBEAT_REQUEST:
                        Packets.MobileHeartbeatRequest mobile_hb_req = Packets.makeMobileHeartbeatRequest();
                        if (!mobile_hb_req.recv(istream)) {
                            onError("Cannot receive mobile_hb_req packet");
                            return false;
                        }
                        onMobileHeartbeatRequest(mobile_hb_req);
                        break;
                    case Packets.STATE_CHANGE_NOTIFICATION:
                        Packets.LightNotification ln = Packets.makeLightNotification();
                        if (!ln.recv(istream)) {
                            onError("Cannot receive light_notification packet");
                            return false;
                        }
                        onLightNotification(ln);
                        break;
                    case Packets.MODES_UPDATE_NOTIFICATION:
                        Packets.ModesUpdateNotification mun = Packets.makeModesUpdateNotification();
                        if (!mun.recv(istream)) {
                            onError("Cannot receive modes_update_notification packet");
                            return false;
                        }
                        onModesUpdateNotification(mun);
                        break;
                    default:
                        onError("Unknown magic: " + magic + ", len: " + len);
                        byte[] bytes = new byte[len];
                        istream.read(bytes, 0, len);
                        //onError("Bytes: " + new String(bytes));
                }
            }
        } catch (SocketException e) {
            if (!m_isStopping) {
                onError("Connection lost. Trying to reconnect..");
            }
            return false;
        } catch (IOException e) {
            StringWriter errors = new StringWriter();
            e.printStackTrace(new PrintWriter(errors));
            onError(errors.toString());

            return false;
        }

        return true;
    }

    private void onHandshakeResponse(Packets.HandshakeResponse hs_res) {
        synchronized (m_listeners) {
            for (EventListener listener : m_listeners) {
                listener.onHandshakeResponse(hs_res.state, hs_res.light_state, hs_res.event_modes);
            }
        }
    }

    private void onConfigUpdateResponse(Packets.ConfigUpdateResponse config_res) {
        synchronized (m_listeners) {
            for (EventListener listener : m_listeners) {
                listener.onConfigUpdate(config_res.config);
            }
        }
    }

    private void onMobileNotification(Packets.MobileNotification mn) {
        synchronized (m_listeners) {
            for (EventListener listener : m_listeners) {
                listener.onMobileNotification(mn.desktop_index, mn.type, mn.params);
            }
        }
    }

    private void onMobileHeartbeatRequest(Packets.MobileHeartbeatRequest mobile_hb_req) {
        Packets.MobileHeartbeatResponse mobile_hb_res = Packets.makeMobileHeartbeatResponse();
        if (!mobile_hb_res.send(ostream)) {
            onError("Cannot send mobile_hb_res");
        }
    }

    private void onLightNotification(Packets.LightNotification ln) {
        synchronized (m_listeners) {
            for (EventListener listener : m_listeners) {
                listener.onLightNotification(ln.state);
            }
        }
    }

    public boolean requestModesUpdate(int state) {
        return Packets.makeUpdateModesRequest(state).send(ostream);
    }

    private void onModesUpdateNotification(Packets.ModesUpdateNotification mun) {
        synchronized (m_listeners) {
            for (EventListener listener : m_listeners) {
                listener.onModesNotification(mun.state);
            }
        }
    }
}
