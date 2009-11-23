package tftp;

import tftpserver.TFTPServerClient;

/**
 * Class for handling common events/actions in application
 * 
 * @author Vilius
 *
 */

public class TFTPUtils {

	public static void puts(String s) {
	 
		System.out.println(s);
		
	}
	
	public static void fatalError(String msg) {
		
		System.err.println("Fatal error: " + msg);
		System.exit(1);
		
	}
	
	public static void clientMessage(TFTPServerClient client, String message) {
		
		System.out.println("[" + client.socket.getInetAddress().getHostAddress() + "] " + message);
		
	}
	
}
