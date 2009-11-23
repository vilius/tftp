package tftpclient;

import java.net.*;
import java.io.*;
import java.text.SimpleDateFormat;
import java.util.Calendar;

import tftp.*;
import tftpserver.TFTPServerClient;
import tftpserver.TFTPServerClient.TClientRequest;


public class TFTPClient extends Thread {
	
	public int server_port;
	public String server_ip;
	public Socket server_socket;
	
	public BufferedOutputStream out;
	public BufferedInputStream in;
	
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
	
	public boolean sendFile(String filename) {
		
		TFTPPacket packet_wrq, packet_ack;
		
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
	        	
	        packet_wrq.createWRQ(filename);
	        	
	        TFTPUtils.puts("Sending write request to server");
	        packet_wrq.sendPacket(out);
	        
	        // wait for ack
	        packet_ack = new TFTPPacket();
	        
	        while (true) {
	        	
	        	TFTPUtils.puts("Waiting for ACK");
		   		
		   		if (!packet_ack.getPacket(in)) {
	        	
		   			TFTPUtils.puts("End of file reached");
		   			break;
		   			
		   		}
		   		
		   		if (packet_ack.isACK()) {
		   			
		   			TFTPUtils.puts("ACK received, sending next packet");
		   			
		   		} else {
		   			
		   			TFTPUtils.fatalError("Unexpected packet");
		   			disconnect();
		   			
		   		}
		   		
	        }
	        	
        } catch (IOException ioException) {
        	
            TFTPUtils.puts("IO Exception in thread: " + ioException.getMessage());
        	
        }
		
		return true;
		
	}
	
	
	public void run() {
		
		if (server_socket.isConnected()) {
			TFTPUtils.puts("Connected to " + server_ip + " on port " + server_port);			
		} else {
			TFTPUtils.fatalError("Disconnected unexpectedly");
		}
		
        try {
        	
	        out = new BufferedOutputStream(server_socket.getOutputStream());
	        out.flush();
	        in = new BufferedInputStream(server_socket.getInputStream());
	        
	        if (request == TClientRequest.WRQ) {
	        	
	        	TFTPPacket packet_wrq = new TFTPPacket();
	        	
	        	packet_wrq.createWRQ("test.txt");
	        	
	        	TFTPUtils.puts("Sending write request to server");
	        	out.write(packet_wrq.data, 0, packet_wrq.getSize());
	        	out.flush();
	        	
	        	in.read();
	        	
	        }
	        
	        // TODO: jei tai RRQ

        } catch (IOException ioException) {
        	
            TFTPUtils.puts("IO Exception in thread: " + ioException.getMessage());
        	
        }
    }
	
	public void disconnect() {
		
		try {
			server_socket.close();
			out.close();
			in.close();
		} catch (Exception e) {
			
		}
		
	}

}
