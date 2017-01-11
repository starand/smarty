package org.zapto.safetylab.smarty;

import android.util.Log;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.lang.reflect.Field;
import java.nio.ByteBuffer;


class PacketBase
{
    int PACKET_MAGIC = 0;
    String[] m_fileds;

    static private boolean m_traceReceived = false;
    static private boolean m_traceSend = false;

    PacketBase(int magic, String[] fields) {
        PACKET_MAGIC = magic;
        m_fileds = fields;
    }

    public boolean send(DataOutputStream out) {
        try {
            // send PACKET_MAGIC and structure size
            sendInt(out, this.PACKET_MAGIC);
            sendInt(out, this.getSize());

            // send packet fields
            for (String fieldName : m_fileds) {
                Field field = this.getClass().getField(fieldName);

                Class type = field.getType();
                if (type == int.class) {
                    int value = (int)field.get(this);
                    sendInt(out, value);
                } else if (type == String.class) {
                    String value = (String)field.get(this);
                    sendString(out, value);
                } else if (type == byte.class) {
                    byte value = (byte)field.get(this);
                    sendByte(out, value);
                } else if (type == short.class) {
                    short value = (short)field.get(this);
                    sendShort(out, value);
                } else if (type == float.class) {
                    float value = (float)field.get(this);
                    sendFloat(out, value);
                }
            }
        } catch (IllegalAccessException | IOException | NoSuchFieldException e ) {
            e.printStackTrace();
            return false;
        }
        return true;
    }

    public boolean recv(DataInputStream in) {
        try {
            // receive only packet declared fields here
            for (String fieldName : m_fileds) {
                Field field = this.getClass().getField(fieldName);
                Class type = field.getType();

                if (type == int.class) {
                    int value = recvInt(in);
                    field.set(this, value);
                } else if (type == String.class) {
                    String value = recvString(in);
                    field.set(this, value);
                } else if (type == byte.class) {
                    byte value = recvByte(in);
                    field.set(this, value);
                } else if (type == short.class) {
                    short value = recvShort(in);
                    field.set(this, value);
                } else if (type == float.class) {
                    float value = recvFloat(in);
                    field.set(this, value);
                }
            }
            //System.out.println("=== Bytes received: " + getSize());
        } catch (NoSuchFieldException | IllegalAccessException | IOException e) {
            e.printStackTrace();
            return false;
        }

        return true;
    }

    int getSize() throws IllegalArgumentException, IllegalAccessException {
        int size = 0;
        for (Field field : this.getClass().getDeclaredFields()) {
            Class type = field.getType();
            if (type == int.class) {
                size += 4;
            } else if (type == String.class) {
                String value = (String)field.get(this);
                size += getStringSize(value);
            } else if (type == byte.class) {
                ++size;
            } else if (type == short.class) {
                size += 2;
            } else if (type == float.class) {
                size += 4;
            }
        }

        return size;
    }

    static int getStringSize(String value) {
        byte[] byte_arr = null;

        try {
            byte_arr = value.getBytes("UTF8");
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        }

        return byte_arr != null ? byte_arr.length + 4 : 0;
    }

    static void sendInt(DataOutputStream stream, int value) throws IOException {
        if (m_traceSend) Log.d("Send", "Int: " + value);
        stream.writeInt(ntohl(value));
    }

    static void sendShort(DataOutputStream stream, short value) throws IOException {
        if (m_traceSend) Log.d("Send", "Short: " + value);
        stream.writeShort(ntohs(value));
    }

    static void sendByte(DataOutputStream stream, byte value) throws IOException {
        if (m_traceSend) Log.d("Send", "Byte: " + value);
        stream.writeByte(value);
    }

    static void sendFloat(DataOutputStream stream, float value) throws IOException {
        if (m_traceSend) Log.d("Send", "Float: " + value);
        byte[] buf = htonf(value).array();
        stream.write(buf[3]);
        stream.write(buf[2]);
        stream.write(buf[1]);
        stream.write(buf[0]);
    }

    static int recvInt(DataInputStream stream) throws IOException {
        int value = ntohl(stream.readInt());
        if (m_traceReceived) Log.d("Received", "Int: " + value);
        return value;
    }

    static short recvShort(DataInputStream stream) throws IOException {
        short value = ntohs(stream.readShort());
        if (m_traceReceived) Log.d("Received", "Short: " + value);
        return value;
    }

    static byte recvByte(DataInputStream stream) throws IOException {
        byte value = stream.readByte();
        if (m_traceReceived) Log.d("Received", "Byte: " + value);
        return value;
    }

    static float recvFloat(DataInputStream stream) throws IOException {
        float value = stream.readFloat();
        if (m_traceReceived) Log.d("Received", "Float: " + value);
        return value;
    }

    static void sendString(DataOutputStream stream, String string) throws IOException {
        if (m_traceSend) Log.d("Send", "String: " + string + ", Len: " + string.length());
        int len = string.length();

        stream.writeInt( ntohl(len) );
        byte[] byte_arr = string.getBytes( "UTF8" );
        stream.write( byte_arr, 0, len );
    }

    static String recvString(DataInputStream stream) throws IOException {
        int len = recvInt(stream);
        if (m_traceReceived) Log.d("Received", "String len: " + len);

        byte[] byte_arr = new byte[len];

        int bytes_recevied = 0;
        do {
            bytes_recevied += stream.read(byte_arr, bytes_recevied, len - bytes_recevied);
        } while(bytes_recevied < len);

        String value = new String(byte_arr);
        if (m_traceReceived) Log.d("Received", "String: " + value);
        return value;
    }

    static int ntohl(int input) {
        return (input >>> 24) | (input >> 8) & 0x0000ff00 |
                (input << 8) & 0x00ff0000 | (input << 24);
    }

    static short ntohs(short input) {
        return (short)((input >> 8) & 0x00FF | (input << 8) & 0xFF00);
    }

    static ByteBuffer htonf(float value) {
        ByteBuffer buf = ByteBuffer.allocate(4);
        buf.asFloatBuffer().put(value);
        return buf;
    }
}
