package tftpserver;

import java.io.*;
import java.net.*;
import java.text.SimpleDateFormat;
import java.util.Calendar;

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
					
					packet_ack.sendPacket(out);
					
					TFTPUtils.puts("Initial ACK packet sent");
					
					packet_ack.dumpData();
					
					TFTPPacket packet_data = new TFTPPacket();
					
					while (true) {
						
						if (packet_data.getPacket(in)) {
						
							packet_data.dumpData();
							
						} else {
							
							TFTPUtils.puts("File transferred");
							break;
							
						}
						
					}

				}

	   		}

	   		TFTPUtils.puts("Content: " + new String(last_packet.data));
	   		
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