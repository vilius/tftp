package tftpserver;

import java.io.File;
import tftp.*;

public class TFTPServerUI {

	/**
	 * @param args
	 */
	public static void main(String[] args) {

		File current_file = new File(".");
		
		TFTPUtils.puts("Starting server on port 5555");
		
		TFTPServer server = new TFTPServer(5555, current_file + "\\ftproot");
		
		server.shutdown();
		
	}

}
