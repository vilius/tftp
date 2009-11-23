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
	
	public BufferedOutputStream out;
	public BufferedInputStream in;
	public DataOutputStream fout;
	public DataInputStream fin;
	
	public TFTPServerClient(TFTPServer server, Socket socket) {
		
		this.server = server;
		this.socket = socket;
		
		last_packet = new TFTPPacket();
		
		TFTPUtils.clientMessage(this, "Client connected, waiting for request...");
	
		start();
		
	}
	
	public void run () {
		
		try {
	   		
	   		in = new BufferedInputStream(socket.getInputStream());
	   		out = new BufferedOutputStream(socket.getOutputStream());
	   		
	   		last_packet.getPacket(in);

	   		if (last_packet.isWRQ()) {

	   			TFTPUtils.clientMessage(this, "Write request received");
	   			
	   			this.request = TClientRequest.WRQ;

	   			File file_exists = null;

	   			try {

	   				file_exists = new File(server.ftproot + last_packet.getString(2, last_packet.getSize()));

	   			} catch (Exception e) {

	   				TFTPUtils.fatalError(e.getMessage());

	   			}

				if (file_exists != null && file_exists.exists()) {

					TFTPUtils.clientMessage(this, "PUT failed. File already exists on server");
					
					//- inform client about the error
					TFTPPacket packet_error = new TFTPPacket();
		    		packet_error.createError(6, TFTPPacket.TFTP_ERROR_6);
		    		packet_error.sendPacket(out);
		    		
		    		server.removeConnection(this.socket);
		    		
				} else {

					try {
						fout = new DataOutputStream(
								new FileOutputStream(server.ftproot + last_packet.getString(2, last_packet.getSize()))
						);
					} catch (Exception e) {
						TFTPUtils.clientMessage(this, "Unable to open file for writing");

						//- inform client about the error
						TFTPPacket packet_error = new TFTPPacket();
			    		packet_error.createError(0, "Internal file access error");
			    		packet_error.sendPacket(out);
			    		
			    		server.removeConnection(this.socket);
			    		
					}
					
					TFTPUtils.clientMessage(this, "Starting PUT transfer");
					
					TFTPPacket packet_ack = new TFTPPacket();

					// Since the positive response to a write request is an acknowledgment packet, in this special case the
					// block number will be zero.
					packet_ack.createACK((char)0);
					
					packet_ack.sendPacket(out);
					TFTPUtils.clientMessage(this, "Initial ACK packet sent");
					
					packet_ack.dumpData();
					
					TFTPPacket packet_data = new TFTPPacket();
					
					while (true) {
						
						if (packet_data.getPacket(in)) {
							
							if (!packet_data.isData()) {
								
								TFTPUtils.clientMessage(this, "Unexpected packet arrived. Expecting DATA packet.");
								server.removeConnection(this.socket);
								
							}
							
							fout.write(packet_data.getData(4));
							
							packet_ack.createACK(packet_data.getPacketNumber());
							packet_ack.sendPacket(out);
							
							if (packet_data.getSize() < 4 + TFTPPacket.TFTP_PACKET_DATA_SIZE) {
								
								TFTPUtils.clientMessage(this, "File transferred");
								break;
								
							}
							
						} else {
							
							TFTPUtils.clientMessage(this, "File transferred");
							break;
							
						}
						
					}

				}

	   		} else if (last_packet.isRRQ()) {
	   			
	   			TFTPPacket packet_ack, packet_data, packet_error;
	   			int bytes_read;
	   			int packet_no;
	   			byte[] data = new byte[TFTPPacket.TFTP_PACKET_DATA_SIZE];
	   			
	   			request = TClientRequest.WRQ;
	   			
	   			try {
	   				
	   				fin = new DataInputStream(new FileInputStream(server.ftproot + last_packet.getString(2, last_packet.getSize())));
	   				
	   			} catch (Exception e) {

	   				try {
	   					TFTPUtils.clientMessage(this, "Unable to open source file: " + last_packet.getString(2, last_packet.getSize()));
	   				} catch (Exception et) {
	   					TFTPUtils.puts("Unable to get file name");
					}

	   				packet_error = new TFTPPacket();
	   				packet_error.createError(1, TFTPPacket.TFTP_ERROR_1);
	   				packet_error.sendPacket(out);
	   				server.removeConnection(socket);

	   			}
	   			
	   	        try {
	   	        	
	   		        // wait for ack
	   		        packet_data = new TFTPPacket();
	   		        packet_ack = new TFTPPacket();
	   		        
	   		        packet_no = 0;
	   		        
	   		        mainloop:
	   		        while (true) {
	   		        	
	   		        	TFTPUtils.clientMessage(this, "Sending DATA packet");
	   		        	
	   		        	for (int i = 0; i < TFTPPacket.TFTP_PACKET_DATA_SIZE; i++) data[i] = 0;
	   		        	
	   		        	bytes_read = 0;
   			   	   		
   			   			try {
   			   				
   			   				bytes_read = fin.read(data, 0, TFTPPacket.TFTP_PACKET_DATA_SIZE);
   			   				
   			   				if (bytes_read > 0) {
   			   					
   			   	   	   			packet_no++;
   			   	   	   			
   			   	   	   			packet_data.createData(packet_no, data, bytes_read);
   			   	   	   			packet_data.sendPacket(out);
   			   	   	   			
   			   	   	   			TFTPUtils.clientMessage(this, "Data packet sent, waiting ACK for packet no " + packet_no);
   			   	   	   			
   			   	   	   		}
   			   				
		   	   	   			//wait for acknoledgement
			   	   	   			
   			   				packet_ack.getPacket(in);
   			   				
   			   				if (packet_ack.isACK()) {
   			   					
   			   					TFTPUtils.clientMessage(this, "ACK received for packet no" + packet_ack.getPacketNumber());
   			   					continue;
   			   					
   			   				}
   			   				
   			   				if (bytes_read < TFTPPacket.TFTP_PACKET_DATA_SIZE) {
   			   					
   			   					//- this is our last packet
   			   					
   			   					TFTPUtils.clientMessage(this, "File was successfully sent!");
   			   					server.removeConnection(socket);
   			   					
   			   					break mainloop;
   			   					
   			   				}
   			   				
   			   	   		} catch (Exception e) {
   			   	   			
   			   	   			TFTPUtils.clientMessage(this, "Exception in sending file to client");
   			   	   			
   			   	   		}
	   			   		
	   		        }
	   		        	
	   	        } catch (Exception ioException) {
	   	        	
	   	        	TFTPUtils.clientMessage(this, "IO Exception in thread: " + ioException.getMessage());
	   	        	
	   	        }
	   			
	   		}

		} catch( EOFException ie ) {
			
			
			
		} catch( IOException ie ) {
			
			ie.printStackTrace();
			
		} finally {

			server.removeConnection(socket);
			
		}
	   
   }
	
}