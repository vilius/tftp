package tftp;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;

public class TFTPPacket {

	static final char TFTP_OPCODE_READ 	= 1;
	static final char TFTP_OPCODE_WRITE = 2;
	static final char TFTP_OPCODE_DATA 	= 3;
	static final char TFTP_OPCODE_ACK 	= 4;
	static final char TFTP_OPCODE_ERROR = 5;
	
	public static final String TFTP_ERROR_0 = "Not defined, see error message (if any)";
	public static final String TFTP_ERROR_1 = "File not found";
	public static final String TFTP_ERROR_2 = "Access violation";
	public static final String TFTP_ERROR_3 = "Disk full or allocation exceeded";
	public static final String TFTP_ERROR_4 = "Illegal TFTP operation";
	public static final String TFTP_ERROR_5 = "Unknown transfer ID";
	public static final String TFTP_ERROR_6 = "File already exists";
	public static final String TFTP_ERROR_7 = "No such user";
	
	public static final int TFTP_PACKET_MAX_SIZE = 1024;
	public static final int TFTP_PACKET_DATA_SIZE = 512;
	
	static final String TFTP_DEFAULT_TRANSFER_MODE = "octet";
	
	protected int current_packet_size;
	public byte[] data; 
	
	protected int packet_num;
		
	public TFTPPacket() {
		
		current_packet_size = 0;
		packet_num = 0;
		
		data = new byte[TFTP_PACKET_MAX_SIZE];
		
	}
	
	public void clear() {
		
		current_packet_size = 0;
		
		for (int i = 0; i < TFTP_PACKET_MAX_SIZE; i++) data[i] = 0;
		
	}
	
	public int getSize() {
		
		return current_packet_size;
		
	}
	
	public void setSize(int size) throws Exception {
		
		if (size < TFTP_PACKET_MAX_SIZE) {
			current_packet_size = size;
		} else {
			throw new Exception("Packet size exceeded");
		}
		
	}
	
	public void dumpData() {
		
		TFTPUtils.puts("--------------DATA DUMP---------------------");
		TFTPUtils.puts("Packet Size: " + current_packet_size);

		TFTPUtils.puts(new String(data));
		
		TFTPUtils.puts("--------------------------------------------");

	}
	
	public boolean addByte(byte b) {
		
		if (current_packet_size >= TFTP_PACKET_MAX_SIZE) {
			return false;
		}

		data[current_packet_size] = b;
		current_packet_size++;

		return true;
		
	}
	
	public boolean addWord(char w) {

		if (!addByte((byte)((w&0xFF00)>>8))) {
			return false;
		}
		
		return addByte((byte)(w&0x00FF));
		
	}
	
	public boolean addString(String s) {
		
		byte[] b = new byte[s.length()];
		b = s.getBytes();

		for (int i = 0; i < s.length(); i++) {

			if (!addByte(b[i])) { 

				return false;

			}

		}

		return true;
		
	}
	
	public boolean addMemory(byte[] buf, int buf_size) {

		if (current_packet_size + buf_size >= TFTP_PACKET_MAX_SIZE)	{
			TFTPUtils.puts("Packet size exceeded");
			return false;
		}
		
		for (int i = 0; i < buf_size; i++) {
			
			 data[current_packet_size + i] = buf[i];
			
		}
		
		current_packet_size += buf_size;
		
		return false;
		
	}
	
	public byte getByte(int offset) {
		
		return data[offset];
		
	}
	
	public char getWord(int offset) {
		
		return (char)(data[offset] << 8 | data[offset + 1]);
		
	}
	
	public String getString(int offset, int length) throws Exception {

		if (offset > current_packet_size) throw new Exception("getString() out of packet bounds");

		if (length < current_packet_size - offset) throw new Exception("getString() length is out of packet bounds");

		String output = new String();

		for (int i = offset; i < offset + length; i++) {
			
			if (data[i] == 0) break; //zero-terminated

			output += (char)data[i];

		}
		
		return output;
		
	}
	
	public char getPacketNumber() {
		
		return (isData() || isACK()) ? getWord(2) : 0;
		
	}
	
	public byte[] getData(int offset) {
		
		if (!isData()) return null;
		
		byte[] data_part = new byte[getSize() - 4];
		
		for (int i = offset; i < current_packet_size; i++) {
			data_part[i-offset] = data[i];
		}
		
		return data_part;
		
	}
	
	
	
	public void createRRQ(String filename) {
		
		clear();
		addWord(TFTP_OPCODE_READ);
		addString(filename);
		addByte((byte)0);
		addString(TFTP_DEFAULT_TRANSFER_MODE);
		addByte((byte)0);

	}
	
	public void createWRQ(String filename) {
		
		clear();
		addWord(TFTP_OPCODE_WRITE);
		addString(filename);
		addByte((byte)0);
		addString(TFTP_DEFAULT_TRANSFER_MODE);
		addByte((byte)0);

	}
	
	public void createACK(char packet_num) {
		
		clear();
		addWord(TFTP_OPCODE_ACK);
		addWord(packet_num);

	}	

	public void createData(int block, byte[] data, int data_size) {
		
		clear();
		addWord(TFTP_OPCODE_DATA);
		addWord((char)block);
		addMemory(data, data_size);

	}

	public void createError(int error_code, String message) {
		
		clear();
		addWord(TFTP_OPCODE_ERROR);
		addWord((char)error_code);
		addString(message);
		addByte((byte)0);

	}
	
	public boolean isRRQ() {
		
		return ((int)getWord(0) == TFTP_OPCODE_READ);		
		
	}
	
	public boolean isWRQ() {

		return ((int)getWord(0) == TFTP_OPCODE_WRITE);
		
	}
	
	public boolean isACK() {
		
		return ((int)getWord(0) == TFTP_OPCODE_ACK);
		
	}
	
	public boolean isData() {
		
		return ((int)getWord(0) == TFTP_OPCODE_DATA);
		
	}
	
	public boolean isError() {
		
		return ((int)getWord(0) == TFTP_OPCODE_ERROR);
		
	}
	
	public boolean sendPacket(BufferedOutputStream out) {
		
		try {
			out.write(data, 0, getSize());
			out.flush();
			return true;
		} catch (Exception e) {
			TFTPUtils.puts("Exception in sendPacket()");
			return false;
		}
		
	}
	
	public boolean getPacket(BufferedInputStream in) {
		
		clear();

   		int bytes_read = 0;
   		
   		try {
   			bytes_read = in.read(data, 0, TFTP_PACKET_MAX_SIZE);
   			
   			if (bytes_read == -1) {
   	   			return false;
   	   		}
   			
   			setSize(bytes_read);
   			
   		} catch (Exception e) {
   			TFTPUtils.puts("Exception in getPacket()");
   			return false;
   		}
   		
   		return true;
		
	}
	

}
