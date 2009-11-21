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
	protected Socket clientSocket;
	
	public TFTPServerClient(TFTPServer server, Socket clientSocket) {
		
		this.server = server;
		this.clientSocket = clientSocket;
	
		start();
		
	}
	
	public void run () {
		
		int b = 0;
		String message = "";
		byte[] buf = new byte[TFTPPacket.TFTP_PACKET_MAX_SIZE];

		try {
	   		
	   		// Create a DataInputStream for communication; the client
	   		// is using a DataOutputStream to write to us
	   		InputStream data = new BufferedInputStream(clientSocket.getInputStream());

	   		for (int i = 0; i < TFTPPacket.TFTP_PACKET_MAX_SIZE; i++) buf[i] = 0;

	   		int nread = 0, r = 0;
	   		
	   		outerloop:
	   			while (nread < 512) {
	   				r = data.read(buf, nread, 512 - nread);
	   				if (r == -1) {
	   					/* EOF */
	   					return;
	   				}
	   				int i = nread;
	   				nread += r;
	   				for (; i < nread; i++) {
	   					if (buf[i] == (byte)'\n' || buf[i] == (byte)'\r') {
	   						break outerloop;
	   					}
	   				}
	   				
	   			}
	   		
	   		TFTPUtils.puts("Bytes received: " + nread);
	   		TFTPUtils.puts(new String(buf));
	   		
	   		if (buf[6] == '.') {
	   			TFTPUtils.puts("WOOOOOHOOOOO");
	   		}
	   		
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
			server.removeConnection(clientSocket);
		}
	   
   }

}