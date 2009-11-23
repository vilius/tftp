package tftpclient;

import java.net.*;
import java.io.*;

import tftp.*;
import tftpserver.TFTPServerClient;
import tftpserver.TFTPServerClient.TClientRequest;


public class TFTPClient extends Thread {
	
	public int server_port;
	public String server_ip;
	public Socket server_socket;
	
	public BufferedOutputStream out;
	public BufferedInputStream in;
	public DataInputStream fin;	
	public DataOutputStream fout;	
	
	public TFTPServerClient.TClientRequest request;
	
	public TFTPClient(int port, String ip) {
	
		server_port = port;
		server_ip = ip;
		
		try {
			
			server_socket = new Socket(server_ip, server_port);
			
		} catch (UnknownHostException unknownHost) {
			
			TFTPUtils.fatalError("Unknown host " + server_ip);
			
		} catch (IOException ioException) {
			
			TFTPUtils.fatalError("Unable to create a socket");
                
        }
        
	}
	
	public boolean sendFile(String filename, String dest) {
		
		TFTPPacket packet_wrq, packet_ack, packet_data;
		int bytes_read;
		int packet_no;
		byte[] data = new byte[TFTPPacket.TFTP_PACKET_DATA_SIZE];
		
		try {
			
			fin = new DataInputStream(new FileInputStream("D:\\Users\\Vilius\\Desktop\\tftp\\java\\src\\tftpclient\\" + filename));
			
		} catch (Exception e) {

			TFTPUtils.fatalError("Unable to open source file: " + filename);

			return false;

		}
		
		request = TClientRequest.WRQ;
		
		if (server_socket.isConnected()) {
			TFTPUtils.puts("Connected to " + server_ip + " on port " + server_port);			
		} else {
			TFTPUtils.fatalError("Disconnected unexpectedly");
		}
		
        try {
        	
	        out = new BufferedOutputStream(server_socket.getOutputStream());
	        in = new BufferedInputStream(server_socket.getInputStream());
	        
	        packet_wrq = new TFTPPacket();
	        	
	        packet_wrq.createWRQ(dest);
	        packet_wrq.dumpData();
	        	
	        TFTPUtils.puts("Sending write request to server");
	        packet_wrq.sendPacket(out);
	        
	        // wait for ack
	        packet_ack = new TFTPPacket();
	        packet_data = new TFTPPacket();
	        
	        packet_no = 0;
	        
	        mainloop:
	        while (true) {
	        	
	        	TFTPUtils.puts("Waiting for ACK");
		   		
		   		if (!packet_ack.getPacket(in)) {
	        	
		   			TFTPUtils.puts("End of file reached");
		   			break;
		   			
		   		}
		   		
		   		if (packet_ack.isACK()) {
		   			
		   			if (packet_ack.getPacketNumber() != (char)packet_no) {
		   				TFTPUtils.puts("WRONG ORDER");
		   			}
		   			
		   			TFTPUtils.puts("ACK received, sending next DATA packet");
		   			
		   			for (int i = 0; i < TFTPPacket.TFTP_PACKET_DATA_SIZE; i++) data[i] = 0;
		   			
		   			bytes_read = 0;
		   	   		
		   			try {
		   				
		   				bytes_read = fin.read(data, 0, TFTPPacket.TFTP_PACKET_DATA_SIZE);
		   				
		   				if (bytes_read == -1) {
		   					
		   					TFTPUtils.puts("EOF in sendFile()");
		   					
		   	   	   		} else {
		   	   	   			
		   	   	   			packet_no++;
		   	   	   			
		   	   	   			packet_data.createData(packet_no, data, bytes_read);
		   	   	   			packet_data.sendPacket(out);
		   	   	   			
		   	   	   			TFTPUtils.puts("Data packet sent");
		   	   	   			packet_data.dumpData();
		   	   	   			
		   	   	   		}
		   				
		   				if (bytes_read < TFTPPacket.TFTP_PACKET_DATA_SIZE) {
		   					
		   					//- this is our last packet
		   					
		   					TFTPUtils.puts("File was successfully sent!");
		   					disconnect();
		   					
		   					break mainloop;
		   					
		   				}
		   				
		   	   		} catch (Exception e) {
		   	   			TFTPUtils.puts("Exception in sendFile() isACK()");		   	   			
		   	   		}
		   			
		   		} else if (packet_ack.isError()) {
		   			
		   			try {
		   				TFTPUtils.puts("Error packet received with message: " + packet_ack.getString(4, packet_ack.getSize()));
		   			} catch (Exception e) {
					}
		   			disconnect();
		   			
		   			break;
		   			
		   		} else {
		   			
		   			TFTPUtils.puts("Unexpected packet");
		   			disconnect();
		   			
		   			break;
		   			
		   		}
		   		
	        }
	        	
        } catch (IOException ioException) {
        	
            TFTPUtils.puts("IO Exception in thread: " + ioException.getMessage());
        	
        }
		
		return true;
		
	}
	
	public boolean getFile(String filename, String dest) {
		
		TFTPPacket packet_rrq, packet_ack, packet_data;
		int packet_no;
		
		File file_exists = null;

		try {

			file_exists = new File(dest);

		} catch (Exception e) {

			TFTPUtils.fatalError(e.getMessage());

		}

		if (file_exists != null && file_exists.exists()) {

			TFTPUtils.fatalError("Unable to download file to " + dest + ". Destination already exists in " + file_exists.getAbsolutePath());
			
		}
		
		try {
			
			fout = new DataOutputStream(new FileOutputStream(dest));
			
		} catch (Exception e) {

			TFTPUtils.fatalError("Unable to download file to specified destination: " + dest);

			return false;

		}
		
		request = TClientRequest.RRQ;
		
		if (server_socket.isConnected()) {
			TFTPUtils.puts("Connected to " + server_ip + " on port " + server_port);			
		} else {
			TFTPUtils.fatalError("Disconnected unexpectedly");
		}
		
        try {
        	
	        out = new BufferedOutputStream(server_socket.getOutputStream());
	        in = new BufferedInputStream(server_socket.getInputStream());
	        
	        packet_rrq = new TFTPPacket();
	        	
	        packet_rrq.createRRQ(filename);
	        packet_rrq.dumpData();
	        	
	        TFTPUtils.puts("Sending read request to server");
	        packet_rrq.sendPacket(out);
	        
	        // wait for data packet
	        packet_ack = new TFTPPacket();
	        packet_data = new TFTPPacket();
	        
	        packet_no = 0;
	        
	        mainloop:
	        while (true) {
	        	
	        	TFTPUtils.puts("Waiting for DATA packet");
		   		
		   		if (!packet_data.getPacket(in)) {
	        	
		   			TFTPUtils.puts("End of file reached");
		   			break;
		   			
		   		}
		   		
		   		//- gavom duomenu paketa
		   		if (packet_data.isData()) {
		   			
		   			if (packet_data.getPacketNumber() != (char)packet_no) {
		   				TFTPUtils.puts("WRONG ORDER");
		   			}
		   			
					fout.write(packet_data.getData(4));

					TFTPUtils.puts("DATA received, sending ACK packet");
					packet_ack.createACK(packet_data.getPacketNumber());
					packet_ack.sendPacket(out);
						
					if (packet_data.getSize() < 4 + TFTPPacket.TFTP_PACKET_DATA_SIZE) {
						
						TFTPUtils.puts("File transferred");
						break;
						
					}
					
		   		} else if (packet_data.isError()) {
						
		   			try {
		   				TFTPUtils.puts("Error packet received with message: " + packet_data.getString(4, packet_data.getSize()));
		   			} catch (Exception e) {
					}
		   			disconnect();
		   			
		   		} else {
		   			
		   			TFTPUtils.puts("Unexpected packet");
		   			disconnect();
		   			
		   			break mainloop;
		   			
		   		}
		   		
	        }
	        	
        } catch (IOException ioException) {
        	
            TFTPUtils.puts("IO Exception in thread: " + ioException.getMessage());
        	
        }
		
		return true;
		
	}
	
	public void disconnect() {
		
		TFTPUtils.puts("Disconnected from server");
		
		try {
			server_socket.close();
			out.close();
			in.close();
			fin.close();
			fout.close();
		} catch (Exception e) {
			
		}
		
	}

}
