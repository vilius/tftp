package tftp;

public class TFTPPacket {

	static final char TFTP_OPCODE_READ 	= 1;
	static final char TFTP_OPCODE_WRITE = 2;
	static final char TFTP_OPCODE_DATA 	= 3;
	static final char TFTP_OPCODE_ACK 	= 4;
	static final char TFTP_OPCODE_ERROR = 5;
	
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

		//TODO: implement
		
		return false;
		
	}
	
	public boolean addMemory(byte[] buf, int buf_size) {

		if (current_packet_size + buf_size >= TFTP_PACKET_MAX_SIZE)	{
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
	
		//TODO: implement
		
		if (offset > current_packet_size) throw new Exception("getString() out of packet bounds");
		
		if (length < current_packet_size - offset) throw new Exception("getString() length is out of packet bounds");
		
		String output = new String();

		for (int i = offset; i < offset + length; i++) {
			
			output += data[i];
			
		}
		
		return output;
		
	}
	
	public char getPacketNumber() {
		
		return (isData() || isACK()) ? getWord(2) : 0;
		
	}
	
	public byte[] getData(int offset) {
		
		byte[] data_part = new byte[TFTP_OPCODE_ERROR];
		
		for (int i = offset; i < current_packet_size; i++)
			data_part[i] = data[i];
		
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
	

}


/**

	public:

		public boolean addByte(BYTE b);
		public boolean addWord(WORD w);
		public boolean addString(char* str);
		public boolean addMemory(char* buffer, int len);
		
		BYTE getByte(int offset);
		WORD getWord(int offset = 0);
		public boolean getString(int offset, char* buffer, int length);
		WORD getNumber();
		unsigned char* getData(int offset = 0);
		public boolean copyData(int offset, char* dest, int length);

		public boolean createRRQ(char* filename);
		public boolean createWRQ(char* filename);
		public boolean createACK(int packet_num);
		public boolean createData(int block, char* data, int data_size);
		public boolean createError(int error_code, char* message);

		public boolean sendPacket(TFTP_Packet*);

		public boolean isRRQ();
		public boolean isWRQ();
		public boolean isACK();
		public boolean isData();
		public boolean isError();

};

#endif


*/