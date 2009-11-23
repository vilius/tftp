package tftpserver;

import tftp.*;

public class TFTPServerUI {

	/**
	 * @param args
	 */
	public static void main(String[] args) {

		TFTPUtils.puts("Starting server on port 5555");
		
		TFTPServer server = new TFTPServer(5555, "D:\\Users\\Vilius\\Desktop\\tftp\\java\\src\\tftpserver\\ftproot\\");
		
		server.shutdown();
		
	}

}
