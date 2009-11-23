package tftpserver;

import java.io.File;
import tftp.*;

public class TFTPServerUI {

	/**
	 * @param args
	 */
	public static void main(String[] args) {

		TFTPServer server;
		File current_file = new File(".");
		
		TFTPUtils.puts("Starting server on port 5555");
		
		try {
			server = new TFTPServer(5555, current_file.getCanonicalPath() + "\\ftproot\\");
			server.shutdown();
		} catch (Exception e) {
			TFTPUtils.puts("Unhandled exception was thrown");
		}
		
	}

}
