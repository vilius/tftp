#ifndef TFTP_PACKET
#define TFTP_PACKET

typedef unsigned char BYTE;
typedef unsigned short WORD;

#define TFTP_OPCODE_READ     1
#define TFTP_OPCODE_WRITE    2
#define TFTP_OPCODE_DATA     3
#define TFTP_OPCODE_ACK      4
#define TFTP_OPCODE_ERROR    5

#define TFTP_PACKET_MAX_SIZE 1024
#define TFTP_PACKET_DATA_SIZE 512

		//"netascii", "octet", or "mail"
#define TFTP_DEFAULT_TRANSFER_MODE "octet"

class TFTP_Packet {

	private:
		

	protected:
		
		int current_packet_size;
		unsigned char data[TFTP_PACKET_MAX_SIZE];

	public:

		TFTP_Packet();
		~TFTP_Packet();
		void clear();

		int getSize();
		bool setSize(int size);

		void dumpData();

		bool addByte(BYTE b);
		bool addWord(WORD w);
		bool addString(char* str);
		bool addMemory(char* buffer, int len);
		
		BYTE getByte(int offset);
		WORD getWord(int offset = 0);
		bool getString(int offset, char* buffer, int length);
		WORD getNumber();
		unsigned char* getData(int offset = 0);
		bool copyData(int offset, char* dest, int length);

		bool createRRQ(char* filename);
		bool createWRQ(char* filename);
		bool createACK(int packet_num);
		bool createData(int block, char* data, int data_size);
		bool createError(int error_code, char* message);

		bool sendPacket(TFTP_Packet*);

		bool isRRQ();
		bool isWRQ();
		bool isACK();
		bool isData();
		bool isError();

};

#endif