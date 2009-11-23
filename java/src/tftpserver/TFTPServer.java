package tftpserver;

import java.net.*;
import java.io.*;

import tftp.*;

public class TFTPServer {
	
	static final int MAX_CONNECTIONS = 10; 
	
	protected String ftproot;
	protected int port;
	
	protected ServerSocket serverSocket;
	protected Socket clientSocket = null;

	public TFTPServer(int port, String ftproot) {
		
		//- initialize settings
		this.port = port;
		this.ftproot = ftproot;
		
	    try {
	    	serverSocket = new ServerSocket(port);
	    } catch (IOException e) {
	    	TFTPUtils.fatalError("Could not listen on port: " + port);
	    }
	    
	    TFTPUtils.puts("Server successfully started. Listening on port " + port + " started");

	    while (true) {
	    	
	    	try {
	    		clientSocket = serverSocket.accept();
	    	} catch (Exception e) {}

	        TFTPServerClient client = new TFTPServerClient(this, clientSocket);

	    }
	    
	}
	
	void removeConnection(Socket clientSocket) {
		
		// Synchronize so we don't mess up sendToAll() while it walks
		// down the list of all output streamsa
		/*synchronized (outputStreams) {
			// Tell the world
			System.out.println( "Removing connection to "+clientSocket);
			// Remove it from our hashtable/list
				outputStreams.remove(clientSocket);
			// Make sure it's closed
			try {
				clientSocket.close();
			} catch( IOException ie ) {
				System.out.println( "Error closing "+s );
				ie.printStackTrace();
			}
		}*/
	}
	
	public void shutdown() {
	
		try {
			serverSocket.close();
		} catch (IOException e) {}
		
	}
	
}
