package org.zapto.safetylab.smarty;


public class Packets
{
	static Packets factory = new Packets();

	public static final int MOBILE_HANDSHAKE_REQUEST = 0;
	public static final int MOBILE_HANDSHAKE_RESPONSE = 1;
	public static final int MOBILE_NOTIFICATION = 2;
	public static final int MOBILE_HEARTBEAT_REQUEST = 3;
	public static final int MOBILE_HEARTBEAT_RESPONSE = 4;
	public static final int STATE_REQUEST = 5;
	public static final int COMMAND_REQUEST = 6;
	public static final int COMMAND_DISCONNECT = 7;
	public static final int STATE_CHANGE_NOTIFICATION = 8;
	public static final int CONFIG_UPDATE_REQUEST = 9;
	public static final int CONFIG_UPDATE_RESPONSE = 10;
	public static final int DESKTOP_HANDHAKE_REQUEST = 11;
	public static final int DESKTOP_HANDSHAKE_RESPONSE = 12;
	public static final int DESKTOP_COMMAND_REQUEST = 13;
	public static final int DESKTOP_HEARTBEAT_REQUEST = 14;
	public static final int DESKTOP_HEARTBEAT_RESPONSE = 15;
	public static final int SERVER_COMMAND_REQUEST = 16;
	public static final int UPDATE_MODES_REQUEST = 17;
	public static final int MODES_UPDATE_NOTIFICATION = 18;


	public class HandshakeRequest extends PacketBase {
		String password;
		final int version = 1;

		HandshakeRequest(String pass) {
			super(MOBILE_HANDSHAKE_REQUEST, new String[]{"password", "version"});
			password = pass;
		}
	}

	static HandshakeRequest makeHandshakeRequest(String password) {
		return factory.new HandshakeRequest(password);
	}

	public class HandshakeResponse extends PacketBase {
		int state;
		byte light_state;
		//int hb_timeout;
		int event_modes;

		HandshakeResponse() {
			super(MOBILE_HANDSHAKE_RESPONSE, new String[]{"state", "light_state", "event_modes"});
		}
	}

	static HandshakeResponse makeHandshakeResponse() {
		return factory.new HandshakeResponse();
	}

	public class MobileNotification extends PacketBase {
		int desktop_index;
		int type;
		String params;

		MobileNotification() {
			super(MOBILE_NOTIFICATION, new String[]{"desktop_index", "type", "params"});
		}
	}

	static MobileNotification makeMobileNotification() {
		return factory.new MobileNotification();
	}

	public class MobileHeartbeatRequest extends PacketBase {
		MobileHeartbeatRequest() {
			super(MOBILE_HEARTBEAT_REQUEST, new String[]{});
		}
	}

	static MobileHeartbeatRequest makeMobileHeartbeatRequest() {
		return factory.new MobileHeartbeatRequest();
	}

	public class StateRequest extends PacketBase {
		StateRequest() {
			super(STATE_REQUEST, new String[]{});
		}
	}

	static StateRequest makeStateRequest() {
		return factory.new StateRequest();
	}

	public class CommandRequest extends PacketBase {
		byte command;
		byte param;

		CommandRequest(byte cmd, byte par) {
			super(COMMAND_REQUEST, new String[]{"command", "param"});
			command = cmd;
			param = par;
		}
	}

	static CommandRequest makeCommandRequest(byte cmd, byte param) {
		return factory.new CommandRequest(cmd, param);
	}

	public class CommandDisconnect extends PacketBase {
		CommandDisconnect() {
			super(COMMAND_DISCONNECT, new String[]{});
		}
	}

	static CommandDisconnect makeCommandDisconnect() {
		return factory.new CommandDisconnect();
	}

	public class MobileHeartbeatResponse extends PacketBase {
		MobileHeartbeatResponse() {
			super(MOBILE_HEARTBEAT_RESPONSE, new String[]{});
		}
	}

	static MobileHeartbeatResponse makeMobileHeartbeatResponse() {
		return factory.new MobileHeartbeatResponse();
	}

	public class LightNotification extends PacketBase {
		byte state;

		LightNotification() {
			super(STATE_CHANGE_NOTIFICATION, new String[]{"state"});
		}
	}

	static LightNotification makeLightNotification() {
		return factory.new LightNotification();
	}

	public class ConfigUpdateRequest extends PacketBase {
		ConfigUpdateRequest() {
			super(CONFIG_UPDATE_REQUEST, new String[]{});
		}
	}

	static ConfigUpdateRequest makeConfigUpdateRequest() {
		return factory.new ConfigUpdateRequest();
	}

	public class ConfigUpdateResponse extends PacketBase {
		String config;

		ConfigUpdateResponse() {
			super(CONFIG_UPDATE_RESPONSE, new String[]{"config"});
		}
	}

	static ConfigUpdateResponse makeConfigUpdateResponse() {
		return factory.new ConfigUpdateResponse();
	}

	// UPDATE_MODES_REQUEST
	public class UpdateModesRequest extends PacketBase {
		int state;

		UpdateModesRequest(int state) {
			super(UPDATE_MODES_REQUEST, new String[]{"state"});
			this.state = state;
		}
	}

	static UpdateModesRequest makeUpdateModesRequest(int state) {
		return factory.new UpdateModesRequest(state);
	}

	// MODES_UPDATE_NOTIFICATION
	public class ModesUpdateNotification extends PacketBase {
		int state;

		ModesUpdateNotification() {
			super(MODES_UPDATE_NOTIFICATION, new String[]{"state"});
		}
	}

	static ModesUpdateNotification makeModesUpdateNotification() {
		return factory.new ModesUpdateNotification();
	}
}
