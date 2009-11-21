package tftpserver;

import java.io.*;
import java.net.*;

import tftp.TFTPPacket;
import tftp.TFTPUtils;

/**
 * @author Vilius
 *
 */

public class TFTPServerClient extends Thread {
	
	public enum TClientRequest { WRQ, RRQ, UNDEFINED };

	public TClientRequest request;
	protected TFTPServer server;
	
	public Socket socket;
	public TFTPPacket last_packet; 
	
	public TFTPServerClient(TFTPServer server, Socket socket) {
		
		this.server = server;
		this.socket = socket;
		
		last_packet = new TFTPPacket();
		
		TFTPUtils.clientMessage(this, "Client connected, waiting for request...");
	
		start();
		
	}
	
	public void run () {
		
		byte[] buf = new byte[TFTPPacket.TFTP_PACKET_MAX_SIZE];
		
		try {
	   		
	   		// Create a DataInputStream for communication; the client
	   		// is using a DataOutputStream to write to us
	   		InputStream data = new BufferedInputStream(socket.getInputStream());

	   		for (int i = 0; i < TFTPPacket.TFTP_PACKET_MAX_SIZE; i++) {
	   			buf[i] = 0;
	   		}

	   		int bytes_read = 0;
	   		
	   		bytes_read = data.read(last_packet.data, 0, TFTPPacket.TFTP_PACKET_MAX_SIZE);
	   		
	   		if (bytes_read == -1) {
	   			TFTPUtils.puts("EOF");
	   		}
	   		
	   		try {
	   			last_packet.setSize(bytes_read);
	   		} catch (Exception e) {
	   			TFTPUtils.puts("Too much data received");
	   			//TODO: shutdown client
	   		}

	   		if (last_packet.isWRQ()) {

	   			TFTPUtils.clientMessage(this, "Write request received");
	   			
	   			this.request = TClientRequest.WRQ;

				//- patikrinam ar egzistuoja toks failas
				//- jei taip sukuriam handler`i ir pasiunciam pirma paketa
	   			
	   			File file_exists = null;

	   			try {
	   				
	   				file_exists = new File(server.ftproot + last_packet.getString(2, last_packet.getSize()));
	   				
	   			} catch (Exception e) {
	   				
	   				TFTPUtils.fatalError(e.getMessage());
	   				
	   			}
	   			
				if (file_exists != null && file_exists.exists()) {
					
					TFTPUtils.clientMessage(this, "PUT failed. File already exists on server");
					//TODO: sendError(&clients[i], 6);
					//TODO: disconnectClient(&clients[i]);

				} else {

					TFTPUtils.clientMessage(this, "Starting PUT transfer");
					
					TFTPPacket packet_ack = new TFTPPacket();

					// Since the positive response to a write request is an acknowledgment packet, in this special case the
					// block number will be zero.
					packet_ack.createACK((char)0);
					
					BufferedOutputStream out = new BufferedOutputStream(socket.getOutputStream());
					out.write(packet_ack.data, 0, packet_ack.getSize());
					
					TFTPUtils.puts("ACK SENT" + packet_ack.getSize());
					
					packet_ack.dumpData();
					
					/*if (sendPacket(packet_ack, &clients[i])) {

						clientStatus(&clients[i], "Acknowledgement sent");

					} else {

						clientStatus(&clients[i], "Error in sending acknowledgement");

					}

					delete packet_ack;*/

				}

	   		}


	   		TFTPUtils.puts("Content: " + new String(last_packet.data));
	   		
	   		/*outerloop:
	   			
	   			while (bytes_read < TFTPPacket.TFTP_PACKET_MAX_SIZE) {
	   				r = data.read(buf, bytes_read, TFTPPacket.TFTP_PACKET_MAX_SIZE - bytes_read);
	   				if (r == -1) {
	   					// eof
	   					return;
	   				}
	   				int i = bytes_read;
	   				bytes_read += r;
	   				for (; i < bytes_read; i++) {
	   					if (buf[i] == (byte)'\n' || buf[i] == (byte)'\r') {
	   						break outerloop;
	   					}
	   				}
	   				
	   			}
	   		*/
	   		
	   		//TFTPUtils.puts("Bytes received: " + bytes_read);
	   		//TFTPUtils.puts(new String(buf));
	   		
	   		/*if (buf[6] == '.') {
	   			TFTPUtils.puts("WOOOOOHOOOOO");
	   		}*/
	   		
	   		/*while (true) {

	   			//b = data.readUnsignedByte();
	   			System.out.println("Available: " + data.available());
	   			System.out.println("Data: " + data.readUTF().getBytes());
	   			
	   			//System.out.println("Received" + message);

	   			//server.sendToAll( message );
	   			b++;
	   			//if (b == 10) break;
	   			
	   		}*/
	   		
		} catch( EOFException ie ) {
			// This doesn't need an error message
		} catch( IOException ie ) {
			// This does; tell the world!
			ie.printStackTrace();
		} finally {
			// The connection is closed for one reason or another,
			// so have the server dealing with it
			server.removeConnection(socket);
		}
	   
   }

}